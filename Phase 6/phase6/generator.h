/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include "Scope.h"
# include "Tree.h"

void generateGlobals(Scope *scope);
static void compute(Expression *result, Expression *left, Expression *right, const std::string &opcode);
static void computeDivOrRem(Expression *result, Expression *left, Expression *right, const std::string &op);
void assign(Expression *expr, Register *reg);
void load(Expression *expr, Register *reg);
Register *getreg();

# endif /* GENERATOR_H */
