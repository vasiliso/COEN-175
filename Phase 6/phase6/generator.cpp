/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <cassert>
# include <iostream>
# include "generator.h"
# include "machine.h"
# include "Tree.h"
# include "Label.h"
# include "Register.h"
# include <map>
# include <iterator>

using namespace std;

std::ostream& os = std::cout;

static int offset;
static string funcname;
static ostream &operator <<(ostream &ostr, Expression *expr);

static Register *eax = new Register("%eax", "%al");
static Register *ecx = new Register("%ecx", "%cl");
static Register *edx = new Register("%edx", "%dl");

static vector<Register *> registers = {eax, ecx, edx};

map<std::string, Label> strings;


/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	operator << (private)
 *
 * Description:	Convenience function for writing the operand of an
 *		expression using the output stream operator.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register != nullptr)
	return ostr << expr->_register;

    expr->operand(ostr);
    return ostr;
}


/*
 * Function:	Expression::operand
 *
 * Description:	Write an expression as an operand to the specified stream.
 */

void Expression::operand(ostream &ostr) const
{
    assert(_offset != 0);
    ostr << _offset << "(%ebp)";
}


/*
 * Function:	Identifier::operand
 *
 * Description:	Write an identifier as an operand to the specified stream.
 */

void Identifier::operand(ostream &ostr) const
{
    if (_symbol->_offset == 0)
	ostr << global_prefix << _symbol->name();
    else
	ostr << _symbol->_offset << "(%ebp)";
}


/*
 * Function:	Number::operand
 *
 * Description:	Write a number as an operand to the specified stream.
 */

void Number::operand(ostream &ostr) const
{
    ostr << "$" << _value;
}

void String::operand(ostream &ostr) const
{
    map<std::string, Label>::iterator it = strings.find(_value);

    if(it == strings.end()){
        Label str;
        strings[_value] = str;
        ostr << str;
    }else{
        ostr << it->second;
    }
}


/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 *
 * 		On a 32-bit Linux platform, the stack needs to be aligned
 * 		on a 4-byte boundary.  (Online documentation seems to
 * 		suggest that 16 bytes are required, but 4 bytes seems to
 * 		work and is much easier.)  Since all arguments are 4-bytes
 *		wide, the stack will always be aligned.
 *
 *		On a 32-bit OS X platform, the stack needs to aligned on a
 *		16-byte boundary.  So, if the stack will not be aligned
 *		after pushing the arguments, we first adjust the stack
 *		pointer.  However, this trick only works if none of the
 *		arguments are themselves function calls.
 *
 *		To handle nested function calls, we need to generate code
 *		for the nested calls first, which requires us to save their
 *		results and then push them on the stack later.  For
 *		efficiency, we only first generate code for the nested
 *		calls, but generate code for ordinary arguments in place.
 */

void Call::generate()
{
    unsigned numBytes;


    /* Generate code for any nested function calls first. */

    numBytes = 0;

    for (int i = _args.size() - 1; i >= 0; i --) {
	numBytes += _args[i]->type().size();

	if (STACK_ALIGNMENT != SIZEOF_ARG && _args[i]->_hasCall)
	    _args[i]->generate();
    }


    /* Align the stack if necessary. */

    if (align(numBytes) != 0) {
	cout << "\tsubl\t$" << align(numBytes) << ", %esp" << endl;
	numBytes += align(numBytes);
    }


    /* Generate code for any remaining arguments and push them on the stack. */

    for (int i = _args.size() - 1; i >= 0; i --) {
	if (STACK_ALIGNMENT == SIZEOF_ARG || !_args[i]->_hasCall)
	    _args[i]->generate();

	cout << "\tpushl\t" << _args[i] << endl;
	assign(_args[i], nullptr);
    }


    /* Call the function and then reclaim the stack space. */

    load(nullptr, eax);
    load(nullptr, ecx);
    load(nullptr, edx);

    cout << "\tcall\t" << global_prefix << _id->name() << endl;

    if (numBytes > 0)
	cout << "\taddl\t$" << numBytes << ", %esp" << endl;

    assign(this, eax);
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (auto stmt : _stmts) {
	stmt->generate();

	// for (auto reg : registers)
	//     assert(reg->_node == nullptr);
    }
}


/*
 * Function:	Simple::generate
 *
 * Description:	Generate code for a simple (expression) statement, which
 *		means simply generating code for the expression.
 */

void Simple::generate()
{
    _expr->generate();
    assign(_expr, nullptr);
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
    int param_offset;


    /* Assign offsets to the parameters and local variables. */

    param_offset = 2 * SIZEOF_REG;
    offset = param_offset;
    allocate(offset);


    /* Generate our prologue. */

    funcname = _id->name();
    cout << global_prefix << funcname << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << funcname << ".size, %esp" << endl;


    /* Generate the body of this function. */

    _body->generate();


    /* Generate our epilogue. */

    cout << endl << global_prefix << funcname << ".exit:" << endl;
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    offset -= align(offset - param_offset);
    cout << "\t.set\t" << funcname << ".size, " << -offset << endl;
    cout << "\t.globl\t" << global_prefix << funcname << endl << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();

    for (auto symbol : symbols)
        if (!symbol->type().isFunction()) {
            cout << "\t.comm\t" << global_prefix << symbol->name() << ", ";
            cout << symbol->type().size() << endl;
        }

    cout << ".data" << endl;
    for (auto str: strings){
        cout << str.second << ":\t.asciz\t\"" << str.first <<"\"" << endl;
    }

}


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only works if the right-hand side is an
 *		integer literal and the left-hand side is an integer
 *		scalar.
 */

void Assignment::generate() {
    Expression *pointer;
    _right->generate();

    if(_left->isDereference(pointer)){
        pointer->generate();
        if(pointer->_register == nullptr){
            load(pointer, getreg());
        }
        if(_right->_register == nullptr){
            load(_right, getreg());
        }
        
        if(_left->type().size() == SIZEOF_CHAR){
            cout << "\tmovb\t" << _right->_register->byte() << ", (" << pointer << ")" << endl;
         }else{
            cout << "\tmovl\t" << _right << ", (" << pointer << ")" << endl;
        }
        
        assign(pointer, nullptr);
        
    }else{
        if(_right->_register == nullptr){
            load(_right, getreg());
        }
        if(_left->type().size() == SIZEOF_CHAR){
            cout << "\tmovb\t" << _right->_register->byte() << ", " << _left << endl;
         }else{
            cout << "\tmovl\t" << _right << ", " << _left << endl;
        }
    }
    
    assign(_right, nullptr);
}

static void compute(Expression *result, Expression *left, Expression *right, const string &opcode){
    left->generate();
    right->generate();

    if (left->_register == nullptr)
        load(left, getreg());

    cout << "\t"<< opcode <<"\t" << right << ", " << left << endl;

    assign(right, nullptr);
    assign(result, left->_register);
}

void Add::generate() {
    compute(this, _left, _right, "addl");
}

void Subtract::generate() {
    compute(this, _left, _right, "subl");
}

void Multiply::generate() {
    compute(this, _left, _right, "imull");
}

void Divide::generate() {
    computeDivOrRem(this, _left, _right, "div");
}

void Remainder::generate() {
    computeDivOrRem(this, _left, _right, "rem");
}
static void computeDivOrRem(Expression *result, Expression *left, Expression *right, const string &op){
    left->generate();
    right->generate();
    load(left, registers[0]); // allocate eax
    load(nullptr, registers[2]); // ensure edx empty

    //cout << "\tmovl\t%eax, %edx" << endl;
    cout << "\tcltd" << endl;
    load(right, registers[1]);
    cout << "\tidivl\t" << right << endl;
    assign(right, nullptr);
    if (op == "div"){
        assign(result, registers[0]);
    }else{
        assign(result, registers[2]);
    }

}
void Cast::generate() {
    if (_expr->type().size() < _type.size()){
        _expr->generate();
        if (_expr->_register == nullptr){
            load(_expr, getreg());
        }
        assign(this, _expr->_register);
    }
}

void Address::generate() {
    Expression *pointer;

    if (_expr->isDereference(pointer)){
        pointer->generate();
        if (pointer->_register == nullptr)
            load(pointer, getreg());

        assign(this, pointer->_register);
    } else{
        assign(this, getreg());
        cout << "\tleal\t" << _expr << ", " << this << endl;
    }
}

void Dereference::generate(){
    _expr->generate();
    if(_expr->_register == nullptr)
        load(_expr, getreg());

    if(_type.size() == SIZEOF_CHAR){
        cout << "\tmovsbl\t(" << _expr << "), " << _expr << endl;
    }else{
        cout << "\tmovl\t(" << _expr << "), " << _expr << endl;
    }
    assign(this, _expr->_register);
}

void Negate::generate() {
    _expr->generate();
    if (_expr->_register == nullptr){
        load(_expr, getreg());
    }

    cout << "\tnegl\t" << _expr << endl;
    assign(this, _expr->_register);
}

void Not::generate() {
    _expr->generate();
    if (_expr->_register == nullptr){
        load(_expr, getreg());
    }

    cout << "\tcmpl\t$0, " << _expr << endl;
    cout << "\tsete\t" << _expr->_register->byte() << endl;
    cout << "\tmovzbl\t" << _expr->_register->byte() << ", " << _expr->_register << endl;
    assign(this, _expr->_register);
}

void Not::test(const Label &label, bool ifTrue){
    _expr -> test(label, !ifTrue);
    assign(this, nullptr);
}

void Expression::test(const Label &label, bool ifTrue) {
    generate();
    if (_register == nullptr){
        load(this, getreg());
    }

    cout << "\tcmpl\t$0, " << this << endl;
    cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;

    assign(this, nullptr);

}

void comparative(Expression *result, Expression *left, Expression *right, const Label &label, bool ifTrue, const string &op1, const string &op2){
    left->generate();
    right->generate();
    if (left->_register == nullptr){
        load(left, getreg());
    }
    if (right->_register == nullptr){
        load(right, getreg());
    }
    
    cout << "\tcmpl\t" << right << ", " << left << endl;
    cout << "\t";
    cout << (ifTrue ? op1 : op2 ) << "\t" << label << endl;

    assign(left, nullptr);
    assign(right, nullptr);

    assign(result, nullptr);
}

void Equal::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "je", "jne");
}

void NotEqual::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "jne", "je");
}

void LessThan::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "jl", "jnl");
}


void LessOrEqual::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "jle", "jnle");
}

void GreaterThan::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "jg", "jng");
}

void GreaterOrEqual::test(const Label &label, bool ifTrue) {
    comparative(this, _left, _right, label, ifTrue, "jge", "jnge");
}

void LogicalAnd::test(const Label &label, bool ifTrue){
    if(ifTrue){
        Label skip;
        _left->test(skip, false);
        _right->test(label, true);
        cout << skip << ":" << endl;
    }else{
        _left->test(label, false);
        _right->test(label, false);
    }
    assign(this, nullptr);
}

void LogicalOr::test(const Label &label, bool ifTrue){ 
    if(ifTrue){
        _left->test(label, true);
        _right->test(label, true);
    }else{
        Label skip;
        _left->test(skip, true);
        _right->test(label, false);
        cout << skip << ":" << endl;
    }
    assign(this, nullptr);
}

void While::generate(){
    Label loop, exit;
    
    cout << loop << ":" << endl;

    _expr->test(exit, false);
    _stmt->generate();

    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;
}

void If::generate(){
    Label exit, elseL;
    if (_elseStmt != nullptr){
        _expr->test(elseL, false);
    }else{
        _expr->test(exit, false);
    }
    _thenStmt->generate();
    cout << "\tjmp\t" << exit << endl;
    if (_elseStmt != nullptr){
        cout << elseL << ":" << endl;
        _elseStmt->generate();
        cout << "\tjmp\t" << exit << endl;
    }
    

    cout << exit << ":" << endl;

}

void For::generate(){
    Label loop, exit;

    _init->generate();
    cout << loop << ":" << endl;
    _expr->test(exit, false);
    _stmt->generate();
    _incr->generate();
    cout << "\tjmp\t" << loop << endl;
    cout << exit << ":" << endl;    
}

void Return::generate(){
    _expr->generate();
    
    load(_expr, eax);
    
    cout << "\tjmp\t" << global_prefix << funcname << ".exit" << endl;
    assign(_expr, nullptr);

}


void assign(Expression *expr, Register *reg){
    if (expr != nullptr) {
        if (expr->_register != nullptr)
            expr->_register->_node = nullptr;
        expr->_register = reg;
    }

    if (reg != nullptr){
        if (reg->_node != nullptr)
            reg->_node->_register = nullptr;
        
        reg->_node = expr;
    }
}

void load(Expression *expr, Register *reg){
    if (reg->_node != expr){
        if (reg->_node != nullptr){
            offset -= reg->_node->type().size();
            reg->_node->_offset = offset;
            cout << "\tmovl\t" << reg << ", ";
            cout << offset << "(%ebp)" << endl;
        }

        if (expr != nullptr){
            cout << (expr->type().size() == 1 ? "\tmosbl\t" : "\tmovl\t");
            cout << expr << ", " << reg << endl;
        }

        assign(expr, reg);
    }
}

Register *getreg(){
    for (auto reg : registers)
        if (reg->_node == nullptr)
            return reg;
    
    load(nullptr, registers[0]);
    return registers[0];
}

