#include "tokens.h"
#include "type.h"
#include <string>

using namespace std;

Type::Type(int specifier, unsigned indirection, unsigned length)
    : _specifier(specifier), _indirection(indirection), _length(length), _kind(ARRAY)
{}
Type::Type(int specifier, unsigned indirection)
    : _specifier(specifier), _indirection(indirection), _kind(SCALAR)
{}
Type::Type(int specifier, unsigned indirection, Parameters *parameters)
    : _specifier(specifier), _indirection(indirection), _parameters(parameters) , _kind(FUNCTION)
{}
Type::Type()
    : _kind(ERROR)
{}


bool Type::operator==(const Type &rhs) const {
    if (_kind != rhs._kind)
        return false;
        
    if (_kind == ERROR)
        return true; 

    if (_specifier != rhs._specifier)
        return false;

    if (_indirection != rhs._indirection)
        return false;

    if (_kind == SCALAR)
        return true;

    if (_kind == ARRAY)
        return (_length == rhs._length);

    if (!_parameters || !rhs._parameters)
        return true;

    return *_parameters == *rhs._parameters;
    
}

bool Type::operator!=(const Type &rhs) const { 
    return !operator==(rhs);
}

ostream &operator <<(ostream &ostr, const Type &type){
    string specifier;
    string kind;
    int length = 0;
    if (type.specifier() == VOID){
        specifier = "VOID";
    }else if(type.specifier() == INT){
        specifier = "INT";
    } else if(type.specifier() == CHAR){
        specifier = "CHAR";
    }

    if (type.isArray()){
        kind = "ARRAY";
        length = type.length();
    }else if(type.isError()){
        kind = "ERROR";
    }else if(type.isFunction()){
        kind = "FUNCTION";
    }else if(type.isScalar()){
        kind = "SCALAR";
    }

    return ostr << "(" << specifier << ", " << type.indirection() << ", " << kind <<  ", " << length << ")";
}