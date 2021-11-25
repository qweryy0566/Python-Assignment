#ifndef PYTHON_INTERPRETER_SCOPE_H_
#define PYTHON_INTERPRETER_SCOPE_H_

#include "Python3Parser.h"
#include "real_any.h"
using std::make_pair;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;
typedef pair<string, RealAny> ArguType;
typedef vector<ArguType> ParametersType;

class Variable {
 private:
  int dep;
  vector<unordered_map<string, RealAny>> stack;
  // 第 0 层为全局变量

 public:
  Variable();
  RealAny &LeftValue(const string &);   // 用于定义变量
  RealAny &operator[](const string &);  // 用于取变量的值
  bool Find(const string &);            // 用于之后添加报错功能
  void AddLevel();
  void DelLevel();
};

class Function {
 private:
  unordered_map<string, Python3Parser::SuiteContext *> suite;
  unordered_map<string, ParametersType> parameters;

 public:
  Python3Parser::SuiteContext *&Suite(const string &);
  ParametersType &Parameters(const string &);
};

#endif