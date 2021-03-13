/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		If a symbol is redeclared, the redeclaration is discarded
 *		and the original declaration is retained.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static Scope *outermost, *toplevel;
static const Type error, integer(INT);

static string invalid_operands = "invalid operands to binary %s";
static string invalid_operand = "invalid operand to unary %s";
static string invalid_arguments = "invalid arguments to called function";

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";
static string void_object = "'%s' has type void";


/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;
    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);
    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, type);
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();

    } else
	delete type.parameters();

    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    cout << name << ": " << type << endl;
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	if (type.specifier() == VOID && type.indirection() == 0)
	    report(void_object, name);

	symbol = new Symbol(name, type);
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkLogical(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if (left.isValue() && right.isValue()){
        return integer;
    }
    report(invalid_operands, op);
    return error;
}
Type checkLogicalOr(const Type &left, const Type &right){
    return checkLogical(left, right, "||");
}
Type checkLogicalAnd(const Type &left, const Type &right){
    return checkLogical(left, right, "&&");
}

Type checkEquality(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if(left.isCompatibleWith(right)){
        return integer;
    }else{
        report(invalid_operands, op);
        return error;
    }
}

Type checkEqual(const Type &left, const Type &right){
    return checkEquality(left, right, "==");
}
Type checkNotEqual(const Type &left, const Type &right){
    return checkEquality(left, right, "!=");
}

Type checkRelational(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if (left == right){
        return integer;
    }else{
        report(invalid_operands, op);
        return error;
    }
}

Type checkLEQ(const Type &left, const Type &right){
    return checkRelational(left, right, "<=");
}

Type checkGEQ(const Type &left, const Type &right){
    return checkRelational(left, right, ">=");
}

Type checkLT(const Type &left, const Type &right){
    return checkRelational(left, right, "<");
}
Type checkGT(const Type &left, const Type &right){
    return checkRelational(left, right, ">");
}

Type checkAdditive(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if (left.isInteger() && right.isInteger()){
        return integer;
    }
    if (left.isPointerToT() && right.isInteger()){
        return Type(left.specifier(), left.indirection());
    }
    if(op == "+"){
        if(left.isInteger() && right.isPointerToT()){
            return Type(right.specifier(), right.indirection());
        }
    }
    if(op == "-"){
        if(left.isPointerToT() && left == right){
            return integer;
        }
    }
    report(invalid_operands, op);
    return error;
}

Type checkAddition(const Type &left, const Type &right){
    return checkAdditive(left, right, "+");
}

Type checkSubtraction(const Type &left, const Type &right){
    return checkAdditive(left, right, "-");
}

Type checkMultiplicative(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if (left.isInteger() && right.isInteger()){
        return integer;
    }
    report(invalid_operands, op);
    return error;
}

Type checkMultiplication(const Type &left, const Type &right){
    return checkMultiplicative(left, right, "*");
}

Type checkDivision(const Type &left, const Type &right){
    return checkMultiplicative(left, right, "/");
}
Type checkRemainder(const Type &left, const Type &right){
    return checkMultiplicative(left, right, "%");
}

Type checkDeref(const Type &operand){
    if(operand == error){
        return error;
    }
    if (operand.isPointerToT()){
        return Type(operand.specifier(), operand.indirection() - 1);
    }
    report(invalid_operand, "*");
    return error;
}

Type checkAddress(const Type &operand){
    if(operand == error){
        return error;
    }
    return Type(operand.specifier(), operand.indirection() + 1);
}

Type checkNot(const Type &operand){
    if(operand == error){
        return error;
    }
    if(operand.isValue()){
        return Type(INT);
    }
    report(invalid_operand, "!");
    return error;
}

Type checkNegate(const Type &operand){
    if(operand == error){
        return error;
    }
    if(operand.isInteger()){
        return Type(INT);
    }
    report(invalid_operand, "-");
    return error;
}

Type checkSizeOf(const Type &operand){
    if(operand == error){
        return error;
    }
    if(operand.isValue()){
        return Type(INT);
    }
    report(invalid_operand, "sizeof");
    return error;
}

Type checkPostfix(const Type &left, const Type &right){
    if(left == error && right == error){
        return error;
    }
    if(left.isPointerToT() && right.isInteger()){
        return Type(left.specifier(), left.indirection() - 1);
    }
    report(invalid_operands, "[]");
    return error;
}

bool checkArguments(const Type &param, const Type &arg){
    if(param == error || arg == error){
        return false;
    }
    if(arg.isValue() && arg.isCompatibleWith(param)){
        return true;
    }
    return false;
}

Type checkFunction(const Type &function, const Parameters *args){
    if(function == error){
        return error;
    }
    if (!function.isFunction()){
        report("called object is not a function");
        return error;
    }
    Parameters *params = function.parameters();
    if (params == nullptr){
        return Type(function.specifier(), function.indirection());
    }
    if(params->size() == args->size()){
        for(int i = 0; i < params->size(); i++){
            if(!params->at(i).isCompatibleWith(args->at(i))){
                report(invalid_arguments, "");
                return error;
            }
        }
    }else{
        report(invalid_arguments, "");
        return error;
    }
   
    return Type(function.specifier(), function.indirection());

}

Type checkAssignment(const Type &left, const Type &right){
    if(left == error || right == error){
        return error;
    }
    cout << left << right << endl;
    if(left.isCompatibleWith(right)){
        return left;
    }
    report(invalid_operands, "=");
    return error;
}

Type checkWhileForIf(const Type &expr){
    if(expr == error){
        return error;
    }
    if (expr.isValue())
    {
        return expr;
    }
    report("invalid type for test expression");
    return error;
    
}

void checkReturnType(const Type &retType){
    Type funcType = outermost->symbols().back()->type();
    cout <<"return type expected: " << outermost->symbols().back()->type() << endl;
    cout <<"return type got: " << retType << endl;
    if(Type(funcType.specifier(), funcType.indirection()).isCompatibleWith(retType)){
        return;
    }
    report("invalid return type");
    return;
}