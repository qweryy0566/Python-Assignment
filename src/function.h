#ifndef PYTHON_INTERPRETER_VARIABLE_H
#define PYTHON_INTERPRETER_VARIABLE_H

#include "Python3BaseVisitor.h"
#include "real_any.h"
using std::unordered_map;
using std::vector;

class Function {
 private:
  unordered_map<std::string, Python3Parser::SuiteContext *> suite;
  vector<std::pair<std::string, RealAny>> parameters;
 
};

#endif