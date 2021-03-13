//Vasilis Odysseos
//COEN 175 Phase 2
# include <iostream>
# include "lexer.h"
# include "tokens.h"


void functionOrGLobal();
void remainingGlobalDeclarators();
void globalDeclarator();
void specifier();
void parameters();
void remainingParameters();
void parameter(); 
void pointers();
void declarations();
void remainingDeclarations();
void declarators();
void remainingDeclarators();
void statements();
int statement();
void assignment();
void expressionList();
void expression();
void logicalAndExpression();
void equalityExpression();
void relationalExpression();
void additiveExpression();
void multiplicativeExpression();
void prefixExpression();
void postfixExpression();
void primaryExpression();
void id();

using namespace std;

int lookahead;
string lexbuf;

static void error()
{
	if (lookahead == DONE)
		report("syntax error at end of file");
	else
		report("syntax error at '%s'", lexbuf);

	exit(EXIT_FAILURE);
}

static void match(int t)
{
	if (lookahead != t)
		error();

	lookahead = lexan(lexbuf);
}

void translationUnit(){
	functionOrGLobal();
	//translationUnit();
}
void functionOrGLobal(){
	specifier();
	pointers();
	match(ID);

	if (lookahead == LPAREN){
		match(LPAREN);
		if (lookahead != RPAREN){
			parameters();
			match(RPAREN);
			match(LBRA);
			declarations();
			statements();
			match(RBRA);
		}else{
			match(RPAREN);
			remainingGlobalDeclarators();
		}
	}else if (lookahead == LBRK){
		match(LBRK);
		match(NUM);
		match(RBRK);
		remainingGlobalDeclarators();
	}else{
		remainingGlobalDeclarators();
	}
}

void remainingGlobalDeclarators(){
	if(lookahead == SEMI){
		match(SEMI);
	}else{
		match(COM);
		globalDeclarator();
		remainingGlobalDeclarators();
	}
	return;
}


void globalDeclarator(){
	pointers();
	match(ID);
	if (lookahead == LPAREN){
		match(LPAREN);
		match(RPAREN);
	} else if (lookahead == LBRK){
		match(LBRK);
		match(NUM);
		match(RBRK);
	}
	return;
}

void pointers(){
	while (lookahead == STAR){
		match(STAR);
	}
	return;
}

void specifier(){
	if(lookahead == INT){
		match(INT);
	}else if(lookahead == CHAR){
		match(CHAR);
	}else if(lookahead == VOID){
		match(VOID);
	}
	return;
}

void parameters(){
	if(lookahead == VOID){
		match(VOID);
		if (lookahead == RPAREN){
			return;
		}
		pointers();
		match(ID);
		remainingParameters();
	}else if(lookahead == CHAR){
		match(CHAR);
		pointers();
		match(ID);
		remainingParameters();
	}else if(lookahead == INT){
		match(INT);
		pointers();
		match(ID);
		remainingParameters();
	}
	return;
}

void remainingParameters(){
	if(lookahead == COM){
		match(COM);
		parameter();
		remainingParameters();
	}
	return;
}


void parameter(){
	specifier();
	pointers();
	match(ID);
	return;
}

void declarations(){
	if (lookahead == VOID || lookahead == INT || lookahead == CHAR){
		match(lookahead);
		declarators();
		declarations();
	}
	return;
}


void declarators(){
	pointers();
	match(ID);
	if(lookahead == LBRK){
		match(LBRK);
		match(NUM);
		match(RBRK);
	}
	if(lookahead == COM){
		match(COM);
		declarators();
	}
	if (lookahead == SEMI)
	{
		match(SEMI);
	}
	return;
}

void remainingDeclarators(){
	
	return;
}

// void statements(){
// 	cout << "statements" << endl;
// 	while(statement())
// 		continue;
// }

void statements(){
	if (lookahead == LBRA){
		match(LBRA);
		declarations();
		statements();
		match(RBRA);
	}else if (lookahead == RETURN){
		match(RETURN);
		expression();
		match(SEMI);
	}else if (lookahead == WHILE){
		match(WHILE);
		match(LPAREN);
		expression();
		match(RPAREN);
		statements();
	}else if (lookahead == FOR){
		match(FOR);
		match(LPAREN);
		assignment();
		match(SEMI);
		expression();
		match(SEMI);
		assignment();
		match(RPAREN);
		statements();
	}else if(lookahead == IF){
		match(IF);
		match(LPAREN);
		expression();
		match(RPAREN);
		statements();
		if (lookahead == ELSE){
			match(ELSE);
			statements();
		}
	}else{
		assignment();
		if(lookahead == SEMI){
			match(SEMI);
		}else
		{
			return;
		}
		
	}
	statements();
	return;
}

void assignment(){
	expression();
	while (lookahead == ASSIGN){
		match(ASSIGN);
		//cout << "assignment" << endl;
		expression();
	}
	return;
}

void expressionList(){
	expression();
	while (lookahead == COM){
		match(COM);
		//cout << "Expression List" << endl;
		expression();
	}
	return;
}

void expression()
{
	//cout << "in expressson" << endl;
	/* Implement recursive-descent parser.
	 * Check slides to get started.
	 */
	logicalAndExpression();

	while (lookahead == OR){
		match(OR);
		logicalAndExpression();
		cout << "or" << endl;
	}
	return;
}

void logicalAndExpression()
{
	//cout << "in logicalAndExpressson" << endl;

	equalityExpression();

	while (lookahead == AND){
		match(AND);
		equalityExpression();
		cout << "and" << endl;
	}
	return;
}

void equalityExpression(){
	//cout << "in equialityExpression" << endl;
	relationalExpression();

	while (1){
		if (lookahead == EQL)
		{
			match(lookahead);
			relationalExpression();
			cout << "eql" << endl;

		}else if(lookahead == NEQ){
			match(lookahead);
			relationalExpression();
			cout << "neq" << endl;
		}else{
			break;
		}
			
	}
	
}

void relationalExpression(){
	//cout << "in relationalExpression" << endl;
	additiveExpression();

	while (1){
		if (lookahead == LTN){
			match(lookahead);
			additiveExpression();
			cout << "ltn" << endl;
		}
		else if (lookahead == LEQ){
			match(lookahead);
			additiveExpression();
			cout << "leq" << endl;
		}
		else if (lookahead == GTN){
			match(lookahead);
			additiveExpression();
			cout << "gtn" << endl;
		}
		else if (lookahead == GEQ){
			match(lookahead);
			additiveExpression();
			cout << "geq" << endl;
		}
		else{
			break;
		}
	}
	return;
}

void additiveExpression(){

	//cout << "in additiveExpression" << endl;
	multiplicativeExpression();
	while (1){
		if (lookahead == PLUS)
		{
			match(lookahead);
			multiplicativeExpression();
			cout << "add" << endl;

		}else if(lookahead == MINUS){
			match(lookahead);
			multiplicativeExpression();
			cout << "sub" << endl;
		}else{
			break;
		}
	}
	return;
}

void multiplicativeExpression(){
	//cout << "in multiplicativeExpression" << endl;
	prefixExpression();
	while (1){
		if (lookahead == STAR)
		{
			match(lookahead);
			prefixExpression();
			cout << "mul" << endl;

		}else if(lookahead == SLASH){
			match(lookahead);
			prefixExpression();
			cout << "div" << endl;
		}else if(lookahead == PERC){
			match(lookahead);
			prefixExpression();
			cout << "rem" << endl;
		}else{
			break;
		}
		
	}
	return;
}

void prefixExpression(){
	//cout << "in prefixExpression" << endl;
	
	if (lookahead == AMP || lookahead == NOT || lookahead == STAR || lookahead == SIZEOF || lookahead == MINUS){
		int temp = lookahead;
		match(lookahead);
		prefixExpression();
		switch(temp){
			case AMP:
				cout << "addr" << endl;
				break;
			case NOT:
				cout << "not" << endl;
				break;
			case STAR:
				cout << "deref" << endl;
				break;
			case SIZEOF:
				cout << "sizeof" << endl;
				break;
			case MINUS:
				cout << "neg" << endl;
				break;

		}
	}else{
		postfixExpression();
	}
	return;
}

void postfixExpression(){
	primaryExpression();
	while(lookahead == LBRK){
		match(LBRK);
		expression();
		match(RBRK);
		cout << "index" << endl;
	}
	return;
}

void primaryExpression(){
	if(lookahead == LPAREN){
		match(LPAREN);
		expression();
		match(RPAREN);
	}else if(lookahead == ID){
		match(ID);
		if (lookahead == LPAREN){
		match(LPAREN);
		expressionList();
		match(RPAREN);
	}
	}else if(lookahead == NUM){
		match(NUM);
	}else if(lookahead == STRING){
		match(STRING);
	}
	return;
}




int main()
{
	lookahead = lexan(lexbuf);

	while (lookahead != DONE)
		translationUnit();

	exit(EXIT_SUCCESS);
}
