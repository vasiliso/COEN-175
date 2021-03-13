#include "symbol.h"
#include <string>
using namespace std;

Symbol::Symbol(const string &name, const Type &type)
    : _name(name), _type(type)
{}

bool Symbol::operator==(const Symbol &rhs) const {
    if (_name == rhs._name && _type == rhs._type){
        return true;
    }else{
        return false;
    }
}

bool Symbol::operator!=(const Symbol &rhs) const { 
    return !operator==(rhs);
}

ostream &operator <<(ostream &ostr, const Symbol &symbol){
    return ostr << "symbol: " << symbol.name() << " type: " << symbol.type();
}