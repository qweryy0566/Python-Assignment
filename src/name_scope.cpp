#include "name_scope.h"

RealAny &NameScope::operator[](const std::string &name) {
  return global[name];
}
