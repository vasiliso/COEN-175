#include "scope.h"
#include "iostream"
using namespace std;

Scope::Scope(Scope *enclosing): _enclosing(enclosing)
{}

Symbol *Scope::lookup(const string &name) const {
    Symbol *symbol;
    if ((symbol = find(name)) != nullptr)
        return symbol;

    return _enclosing != nullptr ? _enclosing->lookup(name) : nullptr;
}

Symbol *Scope::find(const string &name) const {
    for (unsigned i = 0; i < _symbols.size(); ++i){
        if (name == _symbols[i]->name()){
            return _symbols[i];
        }
    }
    return nullptr;
}

void Scope::insert(Symbol *symbol) {
    _symbols.push_back(symbol);
    return;
}

void Scope::remove(const string &name){
    for (unsigned i = 0; i < _symbols.size(); ++i){
        if (name == _symbols[i]->name()){
            _symbols.erase(_symbols.begin() + i);
        }
        return;
    }
}