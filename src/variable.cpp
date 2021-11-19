#include "variable.h"

Variable::Variable() : dep(0) {
  stack.push_back(unordered_map<std::string, RealAny>());
}
RealAny &Variable::operator[](const std::string &name) {
  if (stack[dep].find(name) != stack[dep].end())
    return stack[dep][name];
  return stack[0][name];  // 假设 Find 过
}

bool Variable::Find(const std::string &name) {
  return stack[dep].find(name) != stack[dep].end() ||
         stack[0].find(name) != stack[0].end();
}
void Variable::AddLevel() {
  ++dep, stack.push_back(unordered_map<std::string, RealAny>());
}
void Variable::DelLevel() {
  --dep, stack.pop_back();
}