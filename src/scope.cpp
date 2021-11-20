#include "scope.h"

// class Variable

Variable::Variable() : dep(0) {
  stack.push_back(unordered_map<string, RealAny>());
}
RealAny &Variable::operator[](const string &name) {
  if (stack[dep].find(name) != stack[dep].end()) return stack[dep][name];
  return stack[0][name];  // 假设 Find 过
}

bool Variable::Find(const string &name) {
  return stack[dep].find(name) != stack[dep].end() ||
         stack[0].find(name) != stack[0].end();
}
void Variable::AddLevel() {
  ++dep, stack.push_back(unordered_map<string, RealAny>());
}
void Variable::DelLevel() {
  --dep, stack.pop_back();
}

// class Function

Python3Parser::SuiteContext *&Function::Suite(const string &name) {
  return suite[name];
}
ParametersType &Function::Parameters(const string &name) {
  return parameters[name];
}
