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
# include "../utils/trim.hpp"

void interpretResponse(int pipeOut[2], int pipeIn[2], Request &request, std::string &cgiResponse, pid_t pid, Client &currentClient);

void closeAndThrow(int pipeOut[2], int pipeIn[2]);

void CreateResponseAndGenerate(std::string &cgiResponse, Client &currentClient);

void createResponse(int pipeOut[2], std::string &cgiResponse);

void CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env, Request &request, const char *test, int pipeOut[2], int pipeIn[2]);

void test(int pipeOut[2], pid_t pid);

#endif
