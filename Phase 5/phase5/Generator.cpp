#include "Generator.h"
#include "Tree.h"
#include <iostream>
#include <algorithm>

using namespace std;

void Block::generate() {
    for(auto stmt: _stmts)
        stmt->generate();
    return;
}
static ostream &operator <<(ostream &ostr, Expression *expr){
    expr->operand(ostr);
    return ostr;
}

void Function::generate(){
    int param_num = _id->type().parameters()->size();
    Symbols symbols = _body->declarations()->symbols();
    int total_var = symbols.size();
    int param_offset = 8;
    int local_offset = 0;
    int first_offset = 0;
    int size = 0;
    for(int i = 0; i < total_var; ++i){
        if (i < param_num){
            symbols[i]->setOffset(param_offset);
            param_offset += symbols[i]->type().size();
        }else{
            size += symbols[i]->type().size();
            local_offset -= symbols[i]->type().size();
            if(first_offset == 0){
                first_offset = -local_offset;
            }
            symbols[i]->setOffset(local_offset);
        }
    }
    cout << _id->name() << ":" << endl;
    prologue(_id->name(), size);
    _body->generate();
    cout << "\t\t" << "jmp\t" << _id->name() << ".exit" << endl;
    epilogue(_id->name());
    return;
}
void Simple::generate() {
    _expr->generate();
}

void Call::generate() {
    unsigned size = 0;
    Expressions args = _args;
    reverse(args.begin(), args.end());
    for(auto arg: args){
        size += arg->type().size();
        cout << "\t\t" << "pushl\t" << arg << endl;
    }
    cout << "\t\t" << "call\t" << _id->name() << endl;
    cout << "\t\t" << "addl\t$" << size << ", %esp" << endl;
}

void Assignment::generate() {
    cout << "\t\t" << "movl" << "\t" << _right << ", " << _left << endl;
}


void Number::operand(ostream &ostr){
    ostr << "$" << _value;
}

void Identifier::operand(ostream &ostr){
    int offset = _symbol->getOffset();
    if (offset == 0){
        ostr << _symbol->name();
    }else{
        ostr << offset << "(%ebp)";
    }
}

void prologue(const string &name, int size){
    cout << "\t\t" << "pushl\t" << "%ebp" << endl;
    cout << "\t\t" << "movl\t" << "%esp, %ebp" << endl;
    cout << "\t\t" << "subl\t" << "$" << size << ", %esp" << endl;
}
void epilogue(const string &name){
    cout << name << ".exit:" << endl;
    cout << "\t\t" << "movl\t" << "%ebp, %esp" << endl;
    cout << "\t\t" << "popl\t" << "%ebp" << endl;
    cout << "\t\t" << "ret" << endl;
    cout << ".globl " << name << endl;

}

void generateGlobals(Scope *scope){
    Symbols symbols = scope->symbols();
    for(auto symbol: symbols){
        if (symbol->type().isFunction())
            break;
        string name = symbol->name();
        unsigned size = symbol->type().size();
        cout << ".comm " << name << ", " << size << endl;

    }

}

