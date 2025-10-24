#ifndef HANDLECGI_HPP
# define HANDLECGI_HPP

# include "../server/Client.hpp"

# include <map>
# include <string>
# include <cstring>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <vector>
# include <algorithm>

void handleCgiRequest(Client& currentClient, std::vector<std::string> env);

#endif
