#include <iostream>
#include <cctype>

using namespace std;

// set of Simple C operators
const string OPERATOR_SYMBOLS[] = { 
	"=", "|", "||", "&&", "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", 
	"%", "&", "!", "++", "--", ".", "->", "(", ")", "[", "]", "{", "}", ";", ":", "," 
};

// set of simple C keywords
const string KEYWORDS[] = {
	"auto", "break", "case", "char", "const", "continue", "default", "do", "double", 
	"else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", 
	"return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
	"union", "unsigned", "void", "volatile", "while"
}; // cannot be used as identifiers

int main(void){
	string token;
	int c = cin.get();
	int prev, next;

	while (!cin.eof()) {
		token.clear();

		if (isspace(c)){
			c = cin.get();
		}
		else if (isdigit(c)) {
			//while loop here
			token += c;
			c = cin.get();
			while(isdigit(c)){
				token += c;
				c = cin.get();
			}
			cout << "int:" << token << endl;
			continue;
			
		}
		else if (c == '\"'){
			token += c;
			prev = c;
			c = cin.get();
			while((c != '\"') || (c == '\"' && prev == '\\')){
				token += c;
				prev = c;
				c = cin.get();
			}
			token += c;
			cout << "string:" << token <<endl;
			c = cin.get();
			continue;
		}
		else if (isalpha(c) ||  c == '_'){
			token += c;
			c = cin.get();
			bool only_alpha = true;
			bool keyword = false;
			while(isalnum(c) ||  c == '_'){
				if(!isalpha(c)){
					only_alpha = false;
				}
				token += c;
				c = cin.get();
			}
			if(only_alpha){
				for (int i = 0; i < sizeof(KEYWORDS)/sizeof(string); ++i){
					if(KEYWORDS[i].compare(token) == 0){
						keyword = true;
						cout << "keyword:" << token << endl;
						break;
					}
				}

			}
			if(keyword == false){
				cout << "identifier:" << token << endl;
			}
			continue;

		}
		else if(ispunct(c)){
			next = cin.get();
			switch(c){
				case '/':
					if (next == '*'){
						c = cin.get();
						next = cin.get();
						while(!(c == '*' && next == '/')){
							token += c;
							c = next;
							next = cin.get();
						}
						//cout << "comment:" << token << endl;
						c = cin.get();
						continue;
					}else{
						token += c;
						c = next;
					}
					break;
				case '&':
					if (next == '&'){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
					break;
				case '-':
					if (next == '-' || next == '>'){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
					break;
				case '+':
					if (next == '+'){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
					break;
				case '|':
					if (next == '|'){
						token += c;
						token += next;						
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
						break;
				case '!':
					if (next == '='){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
						break;
				case '<':
					if (next == '='){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
						break;
				case '>':
					if (next == '='){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
						break;	
				case '=':
					if (next == '='){
						token += c;
						token += next;
						c = cin.get();
					}else{
						token += c;
						c = next;
					}
						break;	
				default:
					token += c;
					c = next;
			}
			for(int i = 0; i < sizeof(OPERATOR_SYMBOLS)/sizeof(string); ++i){
				if(token.compare(OPERATOR_SYMBOLS[i]) == 0){
					cout << "operator:" << token << endl;
					break;
				}
			}
		}
	}
	return 1;
}
