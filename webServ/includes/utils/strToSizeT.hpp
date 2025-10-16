#ifndef STRTOSIZET_HPP
#define STRTOSIZET_HPP

#include <string>
#include <cstdlib>
#include <climits>
#include <cerrno>
#include <sstream>

bool strToSizeT(const std::string& str, size_t& size, int base);
std::string intToString(int intToConvert);

#endif
