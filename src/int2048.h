#ifndef PYTHON_INTERPRETER_INTEGER_H_
#define PYTHON_INTERPRETER_INTEGER_H_

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
using std::max;
using std::min;
using std::string;

const int kBase = 1e8, kBit = 8;
const int kPow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000};

class int2048 {
 private:
  bool is_negative = 0;
  std::vector<long long> num;  // 初始为空，不为 0.

  bool IsZero() const;
  friend int2048 Times(const int &lhs, const int2048 &rhs);

 public:
  // 构造函数。其实可以加 explicit 规定所有转换，简化代码。
  int2048() = default;
  int2048(const long long &x);
  int2048(const string &s);
  int2048(const int2048 &) = default;
  explicit operator double() const;
  explicit operator bool() const;

  void Read(const string &s);
  friend int2048 Abs(int2048 ans);
  friend int2048 FloatToInt(const double &);

  const int2048 operator-() const;
  int2048 &operator+=(const int2048 rhs);
  friend int2048 operator+(int2048 lhs, const int2048 &rhs);
  int2048 &operator-=(const int2048 rhs);
  friend int2048 operator-(int2048 lhs, const int2048 &rhs);
  int2048 &operator*=(const int2048 &rhs);
  friend int2048 operator*(const int2048 &lhs, const int2048 &rhs);
  int2048 &operator/=(const int2048 &rhs);
  friend int2048 operator/(int2048 lhs, const int2048 &rhs);
  int2048 &operator%=(const int2048 &rhs);
  friend int2048 operator%(const int2048 &lhs, const int2048 &rhs);
  friend std::istream &operator>>(std::istream &lhs, int2048 &rhs);
  friend std::ostream &operator<<(std::ostream &lhs, int2048 rhs);
  friend bool operator==(const int2048 &lhs, const int2048 &rhs);
  friend bool operator!=(const int2048 &lhs, const int2048 &rhs);
  friend bool operator<(const int2048 &lhs, const int2048 &rhs);
  friend bool operator>(const int2048 &lhs, const int2048 &rhs);
  friend bool operator<=(const int2048 &lhs, const int2048 &rhs);
  friend bool operator>=(const int2048 &lhs, const int2048 &rhs);
};

#endif