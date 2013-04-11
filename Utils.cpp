#include "Utils.hpp"
#include <glog/logging.h>
#include <stdlib.h>
#include <algorithm>

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

void splitStrIntoTwoFloatLists(const std::string& str, std::vector<float>* arr) {
  const size_t p = str.find(':');
  if ( p == std::string::npos) {
    LOG(FATAL) << "Bad option format. Expected format: float_list:float_list";
  }
  const std::string p1 = str.substr(0, p);
  const std::string p2 = str.substr(p + 1);
  parseFloatList(p1.c_str(), &arr[0]);
  parseFloatList(p1.c_str(), &arr[1]);
}

void splitStrIntoTwoFloat(const std::string& str, float* arr) {
  const size_t p = str.find(':');
  if (p == std::string::npos) {
    LOG(FATAL) << "Bad option format. Expected format: float:float";
  }
  arr[0] = strtof(str.substr(0, p).c_str(), NULL);
  arr[1] = strtof(str.substr(p + 1).c_str(), NULL);
}

void splitStrIntoTwoSize_t(const std::string& str, size_t* arr) {
  const size_t p = str.find(':');
  if (p == std::string::npos) {
    LOG(FATAL) << "Bad option format. Expected format: uint64:uint64";
  }
  arr[0] = strtoul(str.substr(0, p).c_str(), NULL, 10);
  arr[1] = strtoul(str.substr(p + 1).c_str(), NULL, 10);
}

void splitStrIntoTwoStr(const std::string& str, std::string* arr) {
  const size_t p = str.find(':');
  if (p == std::string::npos) {
    LOG(FATAL) << "Bad option format. Expected format: string:string";
  }
  arr[0] = str.substr(0, p);
  arr[1] = str.substr(p + 1);
}

void splitStrIntoTwoBool(const std::string& str, bool* arr) {
  const size_t p = str.find(':');
  if (p == std::string::npos) {
    LOG(FATAL) << "Bad option format. Expected format: boolean:boolean";
  }
  std::string a = str.substr(0, p);
  std::string b = str.substr(p + 1);
  std::transform(a.begin(), a.end(), a.begin(), ::toupper);
  std::transform(b.begin(), b.end(), b.begin(), ::toupper);
  arr[0] = (a == "TRUE" || a == "1");
  arr[1] = (b == "TRUE" || b == "1");
}
