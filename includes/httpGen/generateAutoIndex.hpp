#ifndef GENERATEAUTOINDEX_HPP
#define GENERATEAUTOINDEX_HPP

#include "../httpGen/httpStatus.hpp"

#include <string>
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>

std::string	generateAutoIndex(const std::string &path);

#endif
