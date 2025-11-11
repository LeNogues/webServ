#ifndef HANDLECGI_HPP
# define HANDLECGI_HPP

# include "../server/Client.hpp"
# include "../utils/trim.hpp"

# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <cstring>
# include <string>
# include <vector>

void	CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env, Request &request, const char *test, int pipeOut[2], int pipeIn[2]);
void	interpretResponse(int pipeOut[2], int pipeIn[2], Request &request, std::string &cgiResponse, pid_t pid, Client &currentClient);
void	CreateResponseAndGenerate(std::string &cgiResponse, Client &currentClient);
void	createResponse(int pipeOut[2], std::string &cgiResponse);
void	closeAndThrow(int pipeOut[2], int pipeIn[2]);

#endif
