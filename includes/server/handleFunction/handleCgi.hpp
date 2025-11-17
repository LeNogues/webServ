#ifndef HANDLECGI_HPP
# define HANDLECGI_HPP

# include "../Client.hpp"

# include <vector>
# include <string>

void	CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env);

#endif
