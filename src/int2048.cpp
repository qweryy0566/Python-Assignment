#include "int2048.h"

int2048::int2048(const std::string &s) {
  Read(s);
}
int2048::int2048(long long x) {
  if (x < 0) is_negative = 1, x = -x;
  for (num.push_back(x % kBase), x /= kBase; x; x /= kBase)
    num.push_back(x % kBase);
}
int2048::operator double() const {
  double ans = 0;
  for (auto it = num.rbegin(); it != num.rend(); ++it)
    ans = ans * kBase + *it;
  return is_negative ? -ans : ans;
}

void int2048::Read(const std::string &s) {
  // TODO 是否存在 +3 情况
  num.clear();
  int s_len = s.length() - 1, bound = s[0] == '+' || s[0] == '-';
  for (is_negative = s[0] == '-'; s_len >= bound; s_len -= kBit) {
    int tmp = 0;
    for (int i = 0; i < kBit && s_len - i >= bound; ++i)
      tmp += (s[s_len - i] - '0') * kPow10[i];  // 从低到高位
    num.push_back(tmp);
  }
  while (!num.back() && num.size() > 1) num.pop_back();
  if (IsZero()) is_negative = 0;
}
bool int2048::IsZero() const {
  return num.size() == 1 && !num[0];
}
int2048 Abs(int2048 ans) {
  ans.is_negative = 0;
  return ans;
}

const int2048 int2048::operator-() const {
  int2048 ans = *this;
  if (!ans.IsZero()) ans.is_negative ^= 1;
  return ans;
}
int2048 &int2048::operator+=(const int2048 rhs) {
  if (is_negative ^ rhs.is_negative) return *this = *this - (-rhs);
  num.resize(max(num.size(), rhs.num.size()) + 1);
  for (int i = 0; i + 1 < num.size(); ++i) {
    if (i < rhs.num.size()) num[i] += rhs.num[i];
    if (num[i] >= kBase) num[i] -= kBase, ++num[i + 1];
  }
  if (!num.back() && num.size() > 1) num.pop_back();
  return *this;
}
int2048 operator+(int2048 lhs, const int2048 &rhs) {
  return lhs += rhs;
}

int2048 &int2048::operator-=(const int2048 rhs) {
  if (is_negative ^ rhs.is_negative) return *this = *this + (-rhs);
  if (Abs(*this) < Abs(rhs)) return *this = -(rhs - *this);
  for (int i = 0; i < num.size(); ++i) {
    if (i < rhs.num.size()) num[i] -= rhs.num[i];
    if (num[i] < 0) num[i] += kBase, --num[i + 1];
  }
  while (!num.back() && num.size() > 1) num.pop_back();
  if (IsZero()) is_negative = 0;
  return *this;
}
int2048 operator-(int2048 lhs, const int2048 &rhs) {
  return lhs -= rhs;
}

int2048 &int2048::operator*=(const int2048 &rhs) {
  return *this = *this * rhs;
}
int2048 operator*(const int2048 &lhs, const int2048 &rhs) {
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

int2048 Times(const int &lhs, const int2048 &rhs) {
  int2048 ans;
  ans.num.resize(rhs.num.size() + 1);
  for (int i = 0; i + 1 < ans.num.size(); ++i) {
    ans.num[i] += lhs * rhs.num[i];
    ans.num[i + 1] += ans.num[i] / kBase, ans.num[i] %= kBase;
  }
  if (!ans.num.back() && ans.num.size() > 1) ans.num.pop_back();
  return ans;
}
int2048 &int2048::operator/=(const int2048 &rhs) {
  return *this = *this / rhs;
}
int2048 operator/(int2048 lhs, const int2048 &rhs) {
  int2048 ans;
  ans.is_negative = lhs.is_negative ^ rhs.is_negative;
  if (ans.is_negative)  // python 中总是向下取整
    lhs += lhs.is_negative ? -Abs(rhs) + 1 : Abs(rhs) - 1;
  ans.num.resize(max(1ul, lhs.num.size() - rhs.num.size() + 1));
  int dl = rhs.num.back() + 1, dr = rhs.num.back();  // 试根的除数
  for (int i = lhs.num.size() - rhs.num.size(); i >= 0; --i) {
    int2048 div;
    for (int j = 0; i + j < lhs.num.size(); ++j)
      div.num.push_back(lhs.num[i + j]);
    if (div >= Abs(rhs)) {
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
      for (int j = 0; j < div.num.size(); ++j)
        lhs.num[i + j] = div.num[j];
      lhs.num.resize(i + div.num.size());
      i = lhs.num.size() - rhs.num.size() + 1;
    }
  }
  if (!ans.num.back() && ans.num.size() > 1) ans.num.pop_back();
  if (ans.IsZero()) ans.is_negative = 0;
  return ans;
}

int2048 &int2048::operator%=(const int2048 &rhs) {
  return *this = *this % rhs;
}
int2048 operator%(const int2048 &lhs, const int2048 &rhs) {
  return lhs - (lhs / rhs) * rhs;  // definition != math
}

std::istream &operator>>(std::istream &lhs, int2048 &rhs) {
  std::string s;
  lhs >> s, rhs.Read(s);
  return lhs;
}
std::ostream &operator<<(std::ostream &lhs, int2048 rhs) {
  if (rhs.is_negative) lhs << '-';
  if (rhs.num.empty())
    lhs << '0';
  else {
    int i = rhs.num.size() - 1;
    for (lhs << rhs.num[i--]; ~i;)
      lhs << std::setw(8) << std::setfill('0') << rhs.num[i--];
  }
  return lhs;
}

inline bool operator==(const int2048 &lhs, const int2048 &rhs) {
  return !(lhs < rhs) && !(rhs < lhs);
}
inline bool operator!=(const int2048 &lhs, const int2048 &rhs) {
  return lhs < rhs || rhs < lhs;
}
inline bool operator<(const int2048 &lhs, const int2048 &rhs) {
  if (lhs.is_negative ^ rhs.is_negative) return lhs.is_negative;
  if (lhs.num.size() != rhs.num.size())
    return lhs.num.size() < rhs.num.size() ^ lhs.is_negative;
  for (int i = lhs.num.size() - 1; ~i; --i)
    if (lhs.num[i] != rhs.num[i])
      return lhs.num[i] < rhs.num[i] ^ lhs.is_negative;
  return 0;
}
inline bool operator>(const int2048 &lhs, const int2048 &rhs) {
  return rhs < lhs;
}
inline bool operator<=(const int2048 &lhs, const int2048 &rhs) {
  return !(rhs < lhs);
}
inline bool operator>=(const int2048 &lhs, const int2048 &rhs) {
  return !(lhs < rhs);
}