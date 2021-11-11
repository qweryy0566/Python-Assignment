#include "real_any.h"

inline bool Equal(const double &lhs, const double &rhs) {
  return -eps < lhs - rhs && lhs - rhs < eps;
}
int2048 FloatToInt(const double &src) {
  std::string tmp = std::to_string(src);
  tmp.resize(tmp.length() - 7);  // 默认有 6 位小数
  return int2048(tmp);
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
    case kFloat: return float_data;
    case kStr: return !str_data.empty();
  }
}
int2048 RealAny::ToInt() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return int_data;
    case kFloat: return FloatToInt(float_data);
    case kStr: return str_data;
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
    case kBool: return std::to_string(bool_data);
    case kInt: {
      std::stringstream ret;
      ret << int_data;
      return ret.str();
    }
    case kFloat: return std::to_string(float_data);
    case kStr: return str_data;
  }
}

const RealAny RealAny::operator-() const {
  return RealAny(int2048(0)) - *this;
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
  }
  return ans;
}
RealAny FloatDiv(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = kFloat;
  ans.float_data = lhs.ToFloat() / rhs.ToFloat();
  return ans;
}
RealAny IntDiv(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = kInt;
  ans.int_data = lhs.ToInt() / rhs.ToInt();
  return ans;
}
RealAny &RealAny::operator%=(const RealAny &rhs) {
  return *this = *this % rhs;
}
RealAny operator%(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = kInt;
  ans.int_data = lhs.ToInt() % rhs.ToInt();
  return ans;
}
std::ostream &operator<<(std::ostream &lhs, RealAny rhs) {
  switch (rhs.type) {
    case kNone: lhs << "None"; break;
    case kBool: lhs << (rhs.bool_data ? "True" : "False"); break;
    case kInt: lhs << rhs.int_data; break;
    case kFloat: lhs << std::fixed << std::setprecision(6) << rhs.float_data; break;
    // 本题要求 6 位
    case kStr: lhs << rhs.str_data; break;
  }
  return lhs;
}

const bool RealAny::operator!() const {
  return !ToBool();
}
bool operator||(const RealAny &lhs, const RealAny &rhs) {
  return lhs.ToBool() || rhs.ToBool();
}
bool operator&&(const RealAny &lhs, const RealAny &rhs) {
  return lhs.ToBool() && rhs.ToBool();
}