#ifndef PYTHON_INTERPRETER_INTEGER_H
#define PYTHON_INTERPRETER_INTEGER_H

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
using std::max;
using std::min;

static const int kBase = 1e8, kBit = 8;
static const int kPow10[] = {1,     10,     100,     1000,
                             10000, 100000, 1000000, 10000000};

class int2048 {
 private:
  bool is_negative = 0;
  std::vector<long long> num;  // 初始为空，不为 0.

  inline bool IsZero() const { return num.size() == 1 && !num[0]; }

 public:
  // 构造函数
  int2048() = default;
  int2048(long long x) {
    if (x < 0) is_negative = 1, x = -x;
    for (num.push_back(x % kBase), x /= kBase; x; x /= kBase)
      num.push_back(x % kBase);
  }
  int2048(const std::string &s) { Read(s); }
  int2048(const int2048 &) = default;

  // 读入一个大整数
  void Read(const std::string &s) {
    num.clear();
    int s_len = s.length() - 1;
    for (is_negative = s[0] == '-'; s_len >= is_negative; s_len -= kBit) {
      int tmp = 0;
      for (int i = 0; i < kBit && s_len - i >= is_negative; ++i)
        tmp += (s[s_len - i] - '0') * kPow10[i];  // 从低到高位
      num.push_back(tmp);
    }
    while (!num.back() && num.size() > 1) num.pop_back();
    if (IsZero()) is_negative = 0;
  }
  // 输出储存的大整数
  void Print() const {
    is_negative ? putchar('-') : 0;
    if (num.empty())
      putchar('0');
    else {
      int i = num.size() - 1;
      for (printf("%lld", num[i--]); ~i;) printf("%08lld", num[i--]);
    }
  }

  friend int2048 Inverse(int2048 ans) {
    ans.is_negative ^= 1;
    return ans;
  }
  friend int2048 Abs(int2048 ans) {
    ans.is_negative = 0;
    return ans;
  }
  int2048 &operator+=(int2048 rhs) {
    if (is_negative ^ rhs.is_negative) return *this = *this - Inverse(rhs);
    num.resize(max(num.size(), rhs.num.size()) + 1);
    for (int i = 0; i + 1 < num.size(); ++i) {
      if (i < rhs.num.size()) num[i] += rhs.num[i];
      if (num[i] >= kBase) num[i] -= kBase, ++num[i + 1];
    }
    if (!num.back() && num.size() > 1) num.pop_back();
    return *this;
  }
  friend int2048 operator+(int2048 lhs, const int2048 &rhs) {
    return lhs += rhs;
  }

  int2048 &operator-=(int2048 rhs) {
    if (is_negative ^ rhs.is_negative) return *this = *this + Inverse(rhs);
    if (Abs(*this) < Abs(rhs)) return *this = Inverse(rhs - *this);
    for (int i = 0; i < num.size(); ++i) {
      if (i < rhs.num.size()) num[i] -= rhs.num[i];
      if (num[i] < 0) num[i] += kBase, --num[i + 1];
    }
    while (!num.back() && num.size() > 1) num.pop_back();
    if (IsZero()) is_negative = 0;
    return *this;
  }
  friend int2048 operator-(int2048 lhs, const int2048 &rhs) {
    return lhs -= rhs;
  }

  int2048 &operator*=(const int2048 &rhs) { return *this = *this * rhs; }
  friend int2048 operator*(const int2048 &lhs, const int2048 &rhs) {
    int2048 ans(0);
    if (lhs.IsZero() || rhs.IsZero()) return ans;
    ans.is_negative = lhs.is_negative ^ rhs.is_negative;
    ans.num.resize(lhs.num.size() + rhs.num.size());
    for (int i = 0; i < lhs.num.size(); ++i)
      for (int j = 0; j < rhs.num.size(); ++j) {
        ans.num[i + j] += lhs.num[i] * rhs.num[j];
        ans.num[i + j + 1] += ans.num[i + j] / kBase;
        ans.num[i + j] %= kBase;
      }
    if (!ans.num.back() && ans.num.size() > 1) ans.num.pop_back();
    return ans;
  }

 private:
  friend int2048 Times(const int &lhs, const int2048 &rhs) {
    int2048 ans;
    ans.num.resize(rhs.num.size() + 1);
    for (int i = 0; i + 1 < ans.num.size(); ++i) {
      ans.num[i] += lhs * rhs.num[i];
      ans.num[i + 1] += ans.num[i] / kBase, ans.num[i] %= kBase;
    }
    if (!ans.num.back() && ans.num.size() > 1) ans.num.pop_back();
    return ans;
  }

 public:
  int2048 &operator/=(const int2048 &rhs) { return *this = *this / rhs; }
  friend int2048 operator/(int2048 lhs, const int2048 &rhs) {
    int2048 ans;
    ans.is_negative = lhs.is_negative ^ rhs.is_negative;
    ans.num.resize(max(1ul, lhs.num.size() - rhs.num.size() + 1));
    int dl = rhs.num.back() + 1, dr = rhs.num.back();  // 试根的除数
    for (int i = lhs.num.size() - rhs.num.size(); i >= 0; --i) {
      int2048 div;
      for (int j = 0; i + j < lhs.num.size(); ++j)
        div.num.push_back(lhs.num[i + j]);
      if (div >= rhs) {
        int l, r, mid;
        if (div.num.size() > rhs.num.size())
          l = (div.num.back() * kBase + div.num[rhs.num.size() - 1]) / dl,
          r = (div.num.back() * kBase + div.num[rhs.num.size() - 1]) / dr;
        else
          l = div.num.back() / dl, r = div.num.back() / dr;
        while (l < r) {
          mid = l + r + 1 >> 1;
          if (Times(mid, rhs) <= div)
            l = mid;
          else
            r = mid - 1;
        }
        ans.num[i] = l, div -= Times(l, rhs);
        for (int j = 0; j < div.num.size(); ++j) lhs.num[i + j] = div.num[j];
        lhs.num.resize(i + div.num.size());
        i = lhs.num.size() - rhs.num.size() + 1;
      }
    }
    if (!ans.num.back() && ans.num.size() > 1) ans.num.pop_back();
    if (ans.IsZero()) ans.is_negative = 0;
    return ans;
  }

  friend std::istream &operator>>(std::istream &lhs, int2048 &rhs) {
    std::string s;
    std::cin >> s;
    rhs.Read(s);
    return lhs;
  }
  friend std::ostream &operator<<(std::ostream &lhs, int2048 rhs) {
    rhs.Print();
    return lhs;
  }

  inline friend bool operator==(const int2048 &lhs, const int2048 &rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
  }
  inline friend bool operator!=(const int2048 &lhs, const int2048 &rhs) {
    return lhs < rhs || rhs < lhs;
  }
  inline friend bool operator<(const int2048 &lhs, const int2048 &rhs) {
    if (lhs.is_negative ^ rhs.is_negative) return lhs.is_negative;
    if (lhs.num.size() != rhs.num.size())
      return lhs.num.size() < rhs.num.size() ^ lhs.is_negative;
    for (int i = lhs.num.size() - 1; ~i; --i)
      if (lhs.num[i] != rhs.num[i])
        return lhs.num[i] < rhs.num[i] ^ lhs.is_negative;
    return 0;
  }
  inline friend bool operator>(const int2048 &lhs, const int2048 &rhs) {
    return rhs < lhs;
  }
  inline friend bool operator<=(const int2048 &lhs, const int2048 &rhs) {
    return !(rhs < lhs);
  }
  inline friend bool operator>=(const int2048 &lhs, const int2048 &rhs) {
    return !(lhs < rhs);
  }
};

#endif