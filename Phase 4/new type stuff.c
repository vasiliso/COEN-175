static Type error, integer(INT);
static string invalid_operands = "invalid operands to binary %s";

bool Type::isInteger() const
{
    return _kind == SCALAR && _specifier != VOID && _inderection == 0;
}

bool Type::isPointer() const
{
    reurn (_kind == SCALAR && _inderection > 0) || _kind == ARRAY;
}

bool Type::isValue() const
{
    return isInteger() || isPointer();
}

Type Type::promote() const
{
    if (_kind == SCALAR && _inderection == 0 && _specifier == CHAR){
        return Type(INT);
    }
    if (_kind == ARRAY){
        return Type(_specifier, _inderection + 1);
    }

    return *this;
}

Type checkLogical(const Type &left, const Type &right, const string &op){
    if (left == error || right == error){
        return error;
    }
    if (left.isValue() && right.isValue())){
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
    if(compatible(left, right)){
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
    if (left.isPointer() && left.specifier() != VOID && right.isInteger()){
        return Type(left.specifier(), left.indirection());
    }
    if(op == "+"){
        if(left.isInteger() && right.specifier() != VOID && right.indirection() > 0){
            return Type(right.specifier(), right.indirection());
        }
    }
    if(op == "-"){
        if(left.isPointer() && left.specifier() != VOID && left == right){
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
    return checkAdditive(const Type &left, const Type &right,"-");
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



bool compatible(const Type &left, const Type &right){
    if (left.isValue() && right.isValue())
    {
        if(left == right){
            return true;
        }
        return false;
    }else if(left.indirection() > 0 && left.specifier() != VOID && right.indirection() > 0 && right.specifier() == VOID){
        return true;
    }else if (left.indirection() > 0 && left.specifier() == VOID && right.indirection() > 0 && right.specifier() != VOID){
        return true;
    }else{
        return false;
    }
    
}