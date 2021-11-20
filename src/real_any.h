#ifndef PYTHON_INTERPRETER_REAL_ANY_H
#define PYTHON_INTERPRETER_REAL_ANY_H

#include "int2048.h"
using std::string;

enum Types { kNone, kBool, kInt, kFloat, kStr };

int2048 FloatToInt(const double &);
double StringToFloat(const string &);

class RealAny {
 private:
  bool bool_data = 0;
  int2048 int_data = 0;
  double float_data = 0;
  string str_data;

 public:
  Types type = kNone;
  RealAny() = default;
  RealAny(const string &);
  RealAny(const bool &);
  RealAny(const int2048 &);
  RealAny(const double &);

  bool ToBool() const;
  int2048 ToInt() const;
  double ToFloat() const;
  string ToStr() const;

  const RealAny operator-() const;
  RealAny &operator+=(const RealAny &rhs);
  friend RealAny operator+(const RealAny &lhs, const RealAny &rhs);
  RealAny &operator-=(const RealAny &rhs);
  friend RealAny operator-(const RealAny &lhs, const RealAny &rhs);
  RealAny &operator*=(const RealAny &rhs);
  friend RealAny operator*(const RealAny &lhs, const RealAny &rhs);
  friend RealAny FloatDiv(const RealAny &lhs, const RealAny &rhs);
  friend RealAny IntDiv(const RealAny &lhs, const RealAny &rhs);
  RealAny &operator%=(const RealAny &rhs);
  friend RealAny operator%(const RealAny &lhs, const RealAny &rhs);
  friend std::istream &operator>>(std::istream &lhs, RealAny &rhs);
  friend std::ostream &operator<<(std::ostream &lhs, RealAny rhs);
  friend bool operator==(const RealAny &lhs, const RealAny &rhs);
  friend bool operator!=(const RealAny &lhs, const RealAny &rhs);
  friend bool operator<(const RealAny &lhs, const RealAny &rhs);
  friend bool operator>(const RealAny &lhs, const RealAny &rhs);
  friend bool operator<=(const RealAny &lhs, const RealAny &rhs);
  friend bool operator>=(const RealAny &lhs, const RealAny &rhs);
  const bool operator!() const;  // 注意运算结果类型
  friend bool operator||(const RealAny &lhs, const RealAny &rhs);
  friend bool operator&&(const RealAny &lhs, const RealAny &rhs);
};

#endif