#include "real_any.h"

int2048 FloatToInt(const double &src) {
  string tmp = std::to_string(src);
  tmp.resize(tmp.length() - 7);  // 默认有 6 位小数
  return int2048(tmp);
}

RealAny::RealAny(const string &rhs) {
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
RealAny::RealAny(const vector<RealAny> &rhs) {
  array = rhs, type = kTuple;
}

bool RealAny::ToBool() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return (bool)int_data;
    case kFloat: return float_data;
    case kStr: return !str_data.empty();
    case kTuple: return array.size();
    default: return false;
  }
}
int2048 RealAny::ToInt() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return int_data;
    case kFloat: return FloatToInt(float_data);
    case kStr: return str_data;
    default: return int2048(0);
  }
}
double RealAny::ToFloat() const {
  switch (type) {
    case kBool: return bool_data;
    case kInt: return (double)int_data;
    case kFloat: return float_data;
    case kStr: return std::stod(str_data);
    default: return 0;
  }
}
string RealAny::ToStr() const {
  if (type == kStr) return str_data;
  std::stringstream ret;
  ret << *this;
  return ret.str();
}

const RealAny RealAny::operator-() const {
  return RealAny(int2048(0)) - *this;
}
RealAny &RealAny::operator+=(const RealAny &rhs) {
  return *this = *this + rhs;
}
RealAny operator+(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = max(kInt, max(lhs.type, rhs.type));  // 注意至少 int
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() + rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() + rhs.ToFloat(); break;
    case kStr:
      ans.str_data = lhs.str_data + rhs.str_data; break;
    default:  // That means kTuple.
      if (lhs.type != rhs.type) break;
      ans.array.insert(ans.array.end(), lhs.array.begin(), lhs.array.end());
      ans.array.insert(ans.array.end(), rhs.array.begin(), rhs.array.end());
      // TODO 非法
  }
  return ans;
}
RealAny &RealAny::operator-=(const RealAny &rhs) {
  return *this = *this - rhs;
}
RealAny operator-(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = max(kInt, max(lhs.type, rhs.type));  // 注意至少 int
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() - rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() - rhs.ToFloat(); break;
    default: break;  // TODO : 减法的非法类型报错
  }
  return ans;
}
RealAny &RealAny::operator*=(const RealAny &rhs) {
  return *this = *this * rhs;
}
RealAny operator*(const RealAny &lhs, const RealAny &rhs) {
  RealAny ans;
  ans.type = max(kInt, max(lhs.type, rhs.type));  // 注意至少 int
  switch (ans.type) {
    case kBool: case kInt:
      ans.int_data = lhs.ToInt() * rhs.ToInt(); break;
    case kFloat:
      ans.float_data = lhs.ToFloat() * rhs.ToFloat(); break;
    default: {  // That means kStr
      string str;
      int2048 cnt;
      if (lhs.type == kStr)
        str = lhs.str_data, cnt = rhs.ToInt();
      else
        str = rhs.str_data, cnt = lhs.ToInt();
      while ((bool)cnt)
        ans.str_data += str, cnt -= 1;
    }
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
    case kTuple:  // 添加 tuple 功能
      lhs << '(';
      for (int i = 0; i < rhs.array.size(); ++i)
        lhs << (i ? " " : "") << rhs.array[i] << ',';
      lhs << ')';
      break;
  }
  return lhs;
}

// 注意运算完变为了 bool 而非 RealAny
const bool RealAny::operator!() const {
  return !ToBool();
}
bool operator||(const RealAny &lhs, const RealAny &rhs) {
  return lhs.ToBool() || rhs.ToBool();
}
bool operator&&(const RealAny &lhs, const RealAny &rhs) {
  return lhs.ToBool() && rhs.ToBool();
}
bool operator==(const RealAny &lhs, const RealAny &rhs) {
  // None == False is false.
  Types check = max(lhs.type, rhs.type);
  if (check <= kFloat) return !(lhs < rhs) && !(rhs < lhs);
  if (lhs.type != rhs.type) return false;
  switch (check) {
    case kStr:
      return lhs.str_data == rhs.str_data;
    // TODO : other type
    default: return true;  // NoneType
  }
}
bool operator!=(const RealAny &lhs, const RealAny &rhs) {
  return !(lhs == rhs);
}
bool operator<(const RealAny &lhs, const RealAny &rhs) {
  // Only int, float, string
  Types check = max(lhs.type, rhs.type);
  switch(check) {
    case kBool: case kInt:
      return lhs.ToInt() < rhs.ToInt();
    case kFloat:
      return lhs.ToFloat() < rhs.ToFloat();
    default:  // That means both are str.
      return lhs.str_data < rhs.str_data;
  }
}
bool operator>(const RealAny &lhs, const RealAny &rhs) {
  return rhs < lhs;
}
bool operator<=(const RealAny &lhs, const RealAny &rhs) {
  return !(rhs < lhs);
}
bool operator>=(const RealAny &lhs, const RealAny &rhs) {
  return !(lhs < rhs);
}