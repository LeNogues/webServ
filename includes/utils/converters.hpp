#ifndef CONVERTERS_HPP
#define CONVERTERS_HPP

#include <string>
#include <cstdlib>
#include <climits>
#include <cerrno>
#include <sstream>
#include <fstream>

std::string intToString(int intToConvert);
bool fileToString(const std::string& path, std::string& out);
bool strToSizeT(const std::string& str, size_t& size, int base);

#endif
