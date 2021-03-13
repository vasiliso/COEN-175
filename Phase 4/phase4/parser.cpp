//Vasilis Odysseos
/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "checker.h"
# include "tokens.h"
# include "lexer.h"

using namespace std;

static int lookahead;
static string lexbuf;

static Type expression(bool &lvalue);
static Type statement();

static const Type typeError;

static string lvalue_required = "lvalue required in expression";


/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    lookahead = lexan(lexbuf);
}


/*
 * Function:	number
 *
 * Description:	Match the next token as a number and return its value.
 */

static unsigned number()
{
    string buf;


    buf = lexbuf;
    match(NUM);
    return strtoul(buf.c_str(), NULL, 0);
}


/*
 * Function:	identifier
 *
 * Description:	Match the next token as an identifier and return its name.
 */

static string identifier()
{
    string buf;


    buf = lexbuf;
    match(ID);
    return buf;
}

static int stringID(){
	string buf;
	buf = lexbuf;
	match(STRING);
	return buf.length();
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == INT || token == CHAR || token == VOID;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only ints, chars, and
 *		void types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  void
 */

static int specifier()
{
    int typespec = ERROR;


    if (isSpecifier(lookahead)) {
	typespec = lookahead;
	match(lookahead);
    } else
	error();

    return typespec;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static unsigned pointers()
{
    unsigned count = 0;


    while (lookahead == '*') {
	match('*');
	count ++;
    }

    return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');
    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    int typespec;


    typespec = specifier();
    declarator(typespec);

    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static Type primaryExpression(bool &lvalue)
{
    if (lookahead == '(') {
	match('(');
	Type type = expression(lvalue);
	match(')');
	return type;

    } else if (lookahead == STRING) {
	int length = stringID();
	lvalue = false;
	return Type(CHAR, 0, length);
    } else if (lookahead == NUM) {
	match(NUM);
	lvalue = false;
	return Type(INT);

    } else if (lookahead == ID) {
	Symbol *symbol = checkIdentifier(identifier());

	if (lookahead == '(') {
		Parameters *args = new Parameters();
	    match('(');

	    if (lookahead != ')') {
		args->push_back(expression(lvalue).promote());
		while (lookahead == ',') {
		    match(',');
		    args-> push_back(expression(lvalue).promote());
		}
	    }
		Type retType = checkFunction(symbol->type(), args);
	    match(')');
		lvalue = false;
		return retType;
		
	}
	if(symbol->type().isScalar()){
		lvalue = true;
	}else{
		lvalue = false;
	}
	return symbol->type();

    } else
	error();
	return typeError;
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 */

static Type postfixExpression(bool &lvalue)
{
	Type left = primaryExpression(lvalue);

    while (lookahead == '[') {
	match('[');
	Type right = expression(lvalue);
	match(']');
	left = checkPostfix(left.promote(), right.promote());
	lvalue = true;
	cout << "check []" << endl;
    }
	return left;
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 */

static Type prefixExpression(bool &lvalue)
{
    if (lookahead == '!') {
	match('!');
	Type operand = prefixExpression(lvalue);
	operand = checkNot(operand);
	cout << "check !" << endl;
	lvalue = false;
	return operand;
    } else if (lookahead == '-') {
	match('-');
	Type operand = prefixExpression(lvalue);
	operand = checkNegate(operand.promote());
	cout << "check -" << endl;
	lvalue = false;
	return operand;
    } else if (lookahead == '*') {
	match('*');
	Type operand = prefixExpression(lvalue);
	operand = checkDeref(operand.promote());
	lvalue = true;
	cout << "deref" << endl;
	return operand;
    } else if (lookahead == '&') {
	match('&');
	Type operand = prefixExpression(lvalue);
	if(lvalue == false) {
		report(lvalue_required, "");
		return typeError;
	}
	operand = checkAddress(operand);
	cout << "check &" << endl;
	lvalue = false;
	return operand;
    } else if (lookahead == SIZEOF) {
	match(SIZEOF);
	Type operand = prefixExpression(lvalue);
	operand = checkSizeOf(operand);
	cout << "check sizeof" << endl;
	lvalue = false;
	return operand;
    } else
	return postfixExpression(lvalue);
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.  Simple C does not have
 *		cast expressions, so we go immediately to prefix
 *		expressions.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */

static Type multiplicativeExpression(bool &lvalue)
{
    Type left = prefixExpression(lvalue);

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    Type right = prefixExpression(lvalue);
		left = checkMultiplication(left.promote(), right.promote());
	    cout << "check *" << endl;
		lvalue = false;
	} else if (lookahead == '/') {
	    match('/');
	    Type right = prefixExpression(lvalue);
		left = checkDivision(left.promote(), right.promote());
	    cout << "check /" << endl;
		lvalue = false;
	} else if (lookahead == '%') {
	    match('%');
	    Type right = prefixExpression(lvalue);
		left = checkRemainder(left.promote(), right.promote());
	    cout << "check %" << endl;
		lvalue = false;
	} else
	    break;
    }
	return left;
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static Type additiveExpression(bool &lvalue)
{
    Type left = multiplicativeExpression(lvalue);

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    Type right = multiplicativeExpression(lvalue);
		left = checkAddition(left.promote(), right.promote());
	    cout << "check +" << endl;
		lvalue = false;
	} else if (lookahead == '-') {
	    match('-');
	    Type right = multiplicativeExpression(lvalue);
		left = checkSubtraction(left.promote(), right.promote());
	    cout << "check -" << endl;
		lvalue = false;
	} else
	    break;
    }
	return left;
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static Type relationalExpression(bool &lvalue)
{
    Type left = additiveExpression(lvalue);

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    Type right = additiveExpression(lvalue);
		left = checkLT(left.promote(), right.promote());
	    cout << "check <" << endl;
		lvalue = false;
	} else if (lookahead == '>') {
	    match('>');
	    Type right = additiveExpression(lvalue);
		left = checkGT(left.promote(), right.promote());
	    cout << "check >" << endl;
		lvalue = false;
	} else if (lookahead == LEQ) {
	    match(LEQ);
	    Type right = additiveExpression(lvalue);
		left = checkLEQ(left.promote(), right.promote());
	    cout << "check <=" << endl;
		lvalue = false;
	} else if (lookahead == GEQ) {
	    match(GEQ);
	    Type right = additiveExpression(lvalue);
		left = checkGEQ(left.promote(), right.promote());
	    cout << "check >=" << endl;
		lvalue = false;
	} else
	    break;
    }
	return left;
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static Type equalityExpression(bool & lvalue)
{
    Type left = relationalExpression(lvalue);

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    Type right = relationalExpression(lvalue);
		left = checkEqual(left.promote(), right.promote());
	    cout << "check ==" << endl;
		lvalue = false;
	} else if (lookahead == NEQ) {
	    match(NEQ);
	    Type right = relationalExpression(lvalue);
		left = checkNotEqual(left.promote(), right.promote());
	    cout << "check !=" << endl;
		lvalue = false;

	} else
	    break;
    }
	return left;
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static Type logicalAndExpression(bool &lvalue)
{
    Type left = equalityExpression(lvalue);

    while (lookahead == AND) {
		match(AND);
		Type right = equalityExpression(lvalue);
		left = checkLogicalAnd(left.promote(), right.promote());
		cout << "check &&" << endl;
		lvalue = false;
    }
	return left;
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static Type expression(bool &lvalue)
{
    Type left = logicalAndExpression(lvalue);

    while (lookahead == OR) {
		match(OR);
		Type right = logicalAndExpression(lvalue);
		left = checkLogicalOr(left.promote(), right.promote());
		lvalue = false;
		cout << "check ||" << endl;
		lvalue = false;
    }
	return left;
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static Type statements()
{
	Type type;
    while (lookahead != '}')
		type = statement();
	return type;
}


/*
 * Function:	Assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment()
{
	bool lvalue = false;
    Type left = expression(lvalue);
	

    if (lookahead == '=') {
	match('=');
	if(lvalue == false){
		report("lvalue required in expression");
	}
	Type right = expression(lvalue);
	left = checkAssignment(left.promote(), right.promote());
	
    }
	return;
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 */

static Type statement()
{
	bool lvalue = false;
    if (lookahead == '{') {
	match('{');
	openScope();
	declarations();
	statements();
	closeScope();
	match('}');
    } else if (lookahead == RETURN) {
	match(RETURN);
	Type type = expression(lvalue);
	checkReturnType(type);
	match(';');
	return type;
    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	checkWhileForIf(expression(lvalue));
	match(')');
	statement();
	
    } else if (lookahead == FOR) {
	match(FOR);
	match('(');
	assignment();
	match(';');
	checkWhileForIf(expression(lvalue));
	match(';');
	assignment();
	match(')');
	statement();
	
    } else if (lookahead == IF) {
	match(IF);
	match('(');
	checkWhileForIf(expression(lvalue));
	match(')');
	statement();
	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}
    } else {
	assignment();
	match(';');
    }
	return typeError;
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static Type parameter()
{
    int typespec;
    unsigned indirection;
    string name;
    Type type;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    type = Type(typespec, indirection);
    declareVariable(name, type);
    return type;
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  void pointers identifier remaining-parameters
 *		  char pointers identifier remaining-parameters
 *		  int pointers identifier remaining-parameters
 *
 *		remaining-parameters:
 *		  empty
 *		  , parameter remaining-parameters
 */

static Parameters *parameters()
{
    int typespec;
    unsigned indirection;
    Parameters *params;
    string name;
    Type type;


    params = new Parameters();

    if (lookahead == VOID) {
	typespec = VOID;
	match(VOID);

	if (lookahead == ')')
	    return params;

    } else
	typespec = specifier();

    indirection = pointers();
    name = identifier();

    type = Type(typespec, indirection);
    declareVariable(name, type);
    params->push_back(type);

    while (lookahead == ',') {
	match(',');
	params->push_back(parameter());
    }

    return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, with optional pointer
 *		declarators.
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 */

static void globalDeclarator(int typespec)
{
    unsigned indirection;
    string name;


    indirection = pointers();
    name = identifier();

    if (lookahead == '(') {
	match('(');
	declareFunction(name, Type(typespec, indirection, nullptr));
	match(')');

    } else if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');

    } else
	declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators:
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(int typespec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	globalOrFunction
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier ( ) remaining-decls 
 * 		  specifier pointers identifier [ num ] remaining-decls
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void globalOrFunction()
{
    int typespec;
    unsigned indirection;
    string name;


    typespec = specifier();
    indirection = pointers();
    name = identifier();

    if (lookahead == '[') {
	match('[');
	declareVariable(name, Type(typespec, indirection, number()));
	match(']');
	remainingDeclarators(typespec);

    } else if (lookahead == '(') {
	match('(');

	if (lookahead == ')') {
	    declareFunction(name, Type(typespec, indirection, nullptr));
	    match(')');
	    remainingDeclarators(typespec);

	} else {
	    openScope();
		Type funcType = Type(typespec, indirection, parameters());
	    defineFunction(name, funcType);
	    match(')');
	    match('{');
	    declarations();
		statements();
	    closeScope();
	    match('}');
	}

    } else {
	declareVariable(name, Type(typespec, indirection));
	remainingDeclarators(typespec);
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    openScope();
    lookahead = lexan(lexbuf);

    while (lookahead != DONE)
	globalOrFunction();

    closeScope();
    exit(EXIT_SUCCESS);
}
