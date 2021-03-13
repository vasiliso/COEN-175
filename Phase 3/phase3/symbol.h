#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "type.h"

class Symbol {
    typedef std::string string;
    string _name;
    Type _type;

    public:
    Symbol(const string &name, const Type &type);
    const string &name() const{
        return _name;
    }
    const Type &type() const{
        return _type;
    }

    bool operator==(const Symbol &rhs) const;
    bool operator!=(const Symbol &rhs) const;

};

std::ostream &operator <<(std::ostream &ostr, const Symbol &symbol); //Overload output stream to output Symbol

#endif /*SYMBOL_H*/