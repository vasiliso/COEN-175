#include "checker.h"
#include "iostream"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include "lexer.h"
#include <string>

using namespace std;

Scope *current = nullptr;
Scope *global = nullptr;

static string redefinition = "redefinition of \'%s\'";
static string conflicting = "conflicting types for \'%s\'";
static string redeclaration = "redeclaration of \'%s\'";
static string undeclared = "\'%s\' undeclared";
static string typeVoid = "\'%s\' has type void";


void openScope(){
    string type;
    Scope *newScope = new Scope(current);
    if(global == nullptr){
        type = "global";
        global = newScope;
    }else{
        type = "local";
    }
    current = newScope;
    cout << "open " << type << " scope" << endl;
    return;
}


void closeScope(){
    string type;
    if(current == global){
        type = "global";
    }else{
        type = "local";
    }
    Scope *oldScope = current;
    current = current->enclosing();
    delete oldScope;
    cout << "close " << type << " scope" << endl;
    return;
}

void declareVariable(const string &id, const Type &type){
    /*Symbol symbol(id, type);*/ 
    if(type.specifier() == VOID && type.indirection() == 0){
        report(typeVoid, id);
    }
    Symbol *symbol = current->find(id);
    if (symbol == nullptr){
        Symbol *newSym = new Symbol(id, type);
        current->insert(newSym);
        cout << "Declare variable " << *newSym << endl;
    }else if(current->enclosing() != nullptr){
        report(redeclaration,id);
    }else if(symbol->type() != type){
        report(conflicting, id);
    }
}

void declareFunction(const string &id, const Type &type){
    Symbol *symbol = global->find(id);
    if (symbol == nullptr){
        Symbol *newSym = new Symbol(id, type);
        global->insert(newSym);
    }else if(symbol->type() != type){
        report(conflicting, id);
    }
    cout << "Declare function " << id << " as " << type << endl;
}

void defineFunction(const string &id, const Type &type){
    Symbol *symbol = global->find(id);
    if (symbol == nullptr){
        Symbol *newSym = new Symbol(id, type);
        global->insert(newSym);
    }else if(symbol->type() != type){
        report(redefinition, id);
    }

    cout << "Define function " << id << " as " << type;
    if(type.params() != NULL){
        cout << " parameters: " << endl;
        if(type.params()->size() == 0){
            cout << "VOID" << endl;
        }else{
            for(int i = 0; i < type.params()->size(); ++i){
                cout << type.params()->at(i) << ", ";
            }
            cout << endl;
        }
    }
}

void checkID(const string &id){
    Symbol *symbol = current->lookup(id);
    if(symbol == nullptr){
        cout << id << endl;
        report(undeclared, id);
    }
    cout << "check " << id << endl;
}
