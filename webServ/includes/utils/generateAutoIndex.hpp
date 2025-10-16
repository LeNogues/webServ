#ifndef GENERATEAUTOINDEX_HPP
#define GENERATEAUTOINDEX_HPP

#include <string>
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include "../utils/httpStatus.hpp"


std::string generateAutoIndex(const std::string& path);

#endif
