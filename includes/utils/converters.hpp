#ifndef CONVERTERS_HPP
#define CONVERTERS_HPP

#include <cstdlib>
#include <climits>
#include <sstream>
#include <fstream>
#include <string>
#include <cerrno>

std::string	intToString(int intToConvert);
bool		fileToString(const std::string& path, std::string& out);
bool		strToSizeT(const std::string& str, size_t& size, int base);

#endif
