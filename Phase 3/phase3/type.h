#ifndef TYPE_H
#define TYPE_H
#include <vector>
#include <ostream>
#include "tokens.h"

typedef std::vector<class Type> Parameters;


class Type{
    int _specifier;
    unsigned _indirection;
    unsigned _length;
    Parameters *_parameters; //NULL = unspecified = (), Empty array = [] = (void)

    enum {ARRAY, ERROR, FUNCTION, SCALAR} _kind;

public:
Type(); //ERROR
Type(int specifier, unsigned indirection = 0); //SCALAR
Type(int specifier, unsigned indirection, unsigned length); //ARRAY
Type(int specifier, unsigned indirection, Parameters *params); //FUNCTION
    bool isArray() const { return _kind == ARRAY; }
    bool isError() const { return _kind == ERROR; }
    bool isFunction() const { return _kind == FUNCTION; }
    bool isScalar() const { return _kind == SCALAR; }

    bool operator==(const Type &rhs) const;
    bool operator!=(const Type &rhs) const;

    int specifier() const { return _specifier;};
    unsigned indirection() const { return _indirection;};
    unsigned length() const { return _length;};
    int kind() const { return _kind;};
    Parameters *params() const { return  _parameters;};

};

std::ostream &operator <<(std::ostream &ostr, const Type &type); //Overload output stream to output Type

#endif /* TYPE_H */