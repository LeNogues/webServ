#ifndef STRTOSIZET_HPP
#define STRTOSIZET_HPP

#include <string>
#include <cstdlib>
#include <climits>
#include <cerrno>

bool strToSizeT(const std::string& str, size_t& size, int base);

#endif
