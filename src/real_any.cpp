#include "real_any.h"

inline bool Equal(const double &lhs, const double &rhs) {
  return -eps < lhs - rhs && lhs - rhs < eps;
}

int2048 StringToInt(const std::string &s) {
  return int2048(s);
}
double StringToFloat(const std::string &s) {
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

RealAny::RealAny(const std::string &rhs) {
  str_data = rhs, type = kStr;
}
RealAny::RealAny(const bool &rhs) {
  bool_data = rhs, type = kBool;
}
RealAny::RealAny(const int2048 &rhs) {
  int_data = rhs, type = kInt;
}
RealAny::RealAny(const double &rhs) {
  float_data = rhs, type = kFloat;
}

bool RealAny::ToBool() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return !int_data.IsZero();
    case kFloat: return !Equal(float_data, 0);
    case kStr: return !str_data.empty();
  }
}
int2048 RealAny::ToInt() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return int_data;
    case kFloat:;  // TODO FloatToInt
    case kStr: return StringToInt(str_data);
  }
}
double RealAny::ToFloat() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return (double)int_data;
    case kFloat: return float_data;
    case kStr: return StringToFloat(str_data);
  }
}
std::string RealAny::ToStr() const {
  switch (type) {
    case kBool: return bool_data ? "True" : "False";
    case kInt:;  // TODO IntToString
    case kFloat:;  // TODO FloatToString
    case kStr: return str_data;
  }
}

RealAny &RealAny::operator+=(const RealAny &rhs) {
  return *this = *this + rhs;
}
RealAny operator+(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = std::max(lhs.type, rhs.type);
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() + rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() + rhs.ToFloat(); break;
    default:;  // TODO 非法
  }
  return ans;
}
RealAny &RealAny::operator-=(const RealAny &rhs) {
  return *this = *this - rhs;
}
RealAny operator-(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = std::max(lhs.type, rhs.type);
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() - rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() - rhs.ToFloat(); break;
    default:;  // TODO 非法
  }
  return ans;
}
RealAny &RealAny::operator*=(const RealAny &rhs) {
  return *this = *this * rhs;
}
RealAny operator*(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = std::max(lhs.type, rhs.type);
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() * rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() * rhs.ToFloat(); break;
    default:;  // TODO 非法
  }
  return ans;
}