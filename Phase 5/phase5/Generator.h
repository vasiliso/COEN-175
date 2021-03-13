# ifndef GENERATOR_H
# define GENERATOR_H
#include "Generator.h"
#include "Tree.h"
void prologue(const std::string &name, int size);
void epilogue(const std::string &name);
void generateGlobals(Scope *scope);

# endif