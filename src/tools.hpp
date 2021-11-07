#ifndef PYTHON_INTERPRETER_TOOLS_H
#define PYTHON_INTERPRETER_TOOLS_H

#include "int2048.h"

static int2048 StringToInt(const std::string &s) {
  return int2048(s);
}
static double StringToFloat(const std::string &s) {
  bool is_negative = 0;
  double ans = 0, p = 0.1;
  auto it = s.begin();
  if (*it == '+' || *it == '-') {  // TODO 是否存在 +3 情况
    if (*it == '-') is_negative = 1;
    ++it;
  }
  for (; *it != '.'; ++it) ans = ans * 10.0 + *it - '0';
  for (++it; it != s.end(); ++it, p *= 0.1) ans += (*it - '0') * p;
  return is_negative ? -ans : ans;
}

#endif