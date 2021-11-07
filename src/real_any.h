#ifndef PYTHON_INTERPRETER_REAL_ANY_H
#define PYTHON_INTERPRETER_REAL_ANY_H

#include "int2048.h"

enum Types { kString, kBool, kInt, kFloat };

class RealAny {
 private:
  bool bool_data = 0;
  int2048 int_data = 0;
  double float_data = 0;
  std::string str_data;
  Types type;

 public:
  RealAny();
  RealAny(const std::string&);
  RealAny(const bool&);
  RealAny(const int2048&);
  RealAny(const double&);
  
};

#endif