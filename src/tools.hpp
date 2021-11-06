#ifndef PYTHON_INTERPRETER_TOOLS_H
#define PYTHON_INTERPRETER_TOOLS_H

#include "int2048.hpp"

static int2048 StringToInt(const std::string &s) {
  int2048 ans(s);
  return ans;
}
static double StringToFloat(const std::string &s) {
  double ans = 0, p = 0.1;
  auto it = s.begin();
  for (; *it != '.'; ++it) ans = ans * 10.0 + *it - '0';
  for (++it; it != s.end(); ++it, p *= 0.1) ans += (*it - '0') * p;
  return ans;
}

#endif