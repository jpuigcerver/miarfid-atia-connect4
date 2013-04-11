#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <vector>
#include <string>

void parseFloatList(const char* str, std::vector<float>* v);
void splitStrIntoTwoFloatLists(const std::string& str, std::vector<float>* arr);
void splitStrIntoTwoFloat(const std::string& str, float* arr);
void splitStrIntoTwoSize_t(const std::string& str, size_t* arr);
void splitStrIntoTwoStr(const std::string& str, std::string* arr);
void splitStrIntoTwoBool(const std::string& str, bool* arr);

#endif
