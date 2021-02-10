/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkLogical(const Type &left, const Type &right, const std::string &op);
Type checkLogicalOr(const Type &left, const Type &right);
Type checkLogicalAnd(const Type &left, const Type &right);
Type checkEquality(const Type &left, const Type &right, const std::string &op);
Type checkEqual(const Type &left, const Type &right);
Type checkNotEqual(const Type &left, const Type &right);
Type checkRelational(const Type &left, const Type &right, const std::string &op);
Type checkLEQ(const Type &left, const Type &right);
Type checkGEQ(const Type &left, const Type &right);
Type checkLT(const Type &left, const Type &right);
Type checkGT(const Type &left, const Type &right);
Type checkAdditive(const Type &left, const Type &right, const std::string &op);
Type checkAddition(const Type &left, const Type &right);
Type checkSubtraction(const Type &left, const Type &right);
Type checkMultiplicative(const Type &left, const Type &right, const std::string &op);
Type checkMultiplication(const Type &left, const Type &right);
Type checkDivision(const Type &left, const Type &right);
Type checkRemainder(const Type &left, const Type &right);
Type checkDeref(const Type &operand);
Type checkAddress(const Type &operand);
Type checkNot(const Type &operand);
Type checkNegate(const Type &operand);
Type checkSizeOf(const Type &operand);
Type checkPostfix(const Type &left, const Type &right);
bool checkArguments(const Type &param, const Type &arg);
Type checkFunction(const Type &function, const Parameters *args);
Type checkAssignment(const Type &left, const Type &right);
Type checkWhileForIf(const Type &expr);
void checkReturnType(const Type &retType);

# endif /* CHECKER_H */
