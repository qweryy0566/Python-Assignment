#include "name_scope.h"

RealAny &NameScope::operator[](const std::string &name) {
  if(global.find(name) != global.end()) return global[name];
}
