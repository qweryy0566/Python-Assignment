#ifndef PYTHON_INTERPRETER_NAME_SCOPE_H
#define PYTHON_INTERPRETER_NAME_SCOPE_H

#include <unordered_map>

#include "real_any.h"

// TODO : stack
class NameScope {
 private:
  int deep = 0;
  std::unordered_map<std::string, RealAny> global;
  std::vector<std::unordered_map<std::string, RealAny>> var;
 public:
  RealAny &operator[](const std::string &);
};

#endif