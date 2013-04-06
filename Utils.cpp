#include "Utils.hpp"
#include <glog/logging.h>
#include <stdlib.h>

void parseFloatList(const char* str, std::vector<float>* v) {
  CHECK_NOTNULL(str);
  CHECK_NOTNULL(v);
  v->clear();
  const char* c = str; char* n = NULL;
  float f = 0.0;
  do {
    f = strtof(c, &n);
    if (c == n) {
      LOG(FATAL) << "Bad float list. Expected format: float;float...";
    }
    c = n + 1;
    v->push_back(f);
  } while (*n != '\0');
}
