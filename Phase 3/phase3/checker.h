#ifndef CHECKER_H
#define CHECKER_H
#include <string>
#include "type.h"


void openScope();
void closeScope();
void declareVariable(const std::string &id, const Type &type);
void declareFunction(const std::string &id, const Type &type);
void defineFunction(const std::string &id, const Type &type);
void checkID(const std::string &id);

#endif