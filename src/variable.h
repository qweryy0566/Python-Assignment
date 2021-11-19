#ifndef PYTHON_INTERPRETER_VARIABLE_H
#define PYTHON_INTERPRETER_VARIABLE_H

#include <unordered_map>

#include "real_any.h"
using std::unordered_map;
using std::vector;

class Variable {
 private:
  int dep;
  vector<unordered_map<std::string, RealAny>> stack;
  // 第 0 层为全局变量

 public:
  Variable();
  RealAny &operator[](const std::string &);
  bool Find(const std::string &);  // 用于之后添加报错功能
  void AddLevel();
  void DelLevel();
};

#endif