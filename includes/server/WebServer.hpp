/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                        webServ                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   this is just                                   +#+  +:+       +#+        */
/*   to fill it up                                +#+#+#+#+#+   +#+           */
/*   so it look                                        #+#    #+#             */
/*   ✨ aestheticaly pleasing ✨                      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# ifndef MAX_EVENTS
#  define MAX_EVENTS 10
#  define CLIENT_TIMEOUT_SECONDS 500
#  define CGI_BUFFER_SIZE 100
#  define CGI_TIMEOUT_SECONDS 10
# endif

# include "../httpGen/generateErrorPage.hpp"
# include "../httpGen/generateAutoIndex.hpp"
# include "../utils/setNonBlocking.hpp"
# include "../config/ServerConfig.hpp"
# include "handleFunction/handleCgi.hpp"
# include "../utils/converters.hpp"
# include "../utils/parsePath.hpp"
# include "../request/Request.hpp"
# include "Client.hpp"

# include <map>
# include <vector>
# include <cerrno>
# include <cstdio>
# include <cstring>
# include <ctime>
# include <sstream>
# include <fstream>
# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/epoll.h>
# include <arpa/inet.h>
# include <netinet/in.h>

struct CgiHandler
{
	pid_t	pid;
	int		pipeReadFd;
	int		pipeWriteFd;
	size_t	bytesWritten;
	std::time_t	startTime;

	CgiHandler() : pid(0), pipeReadFd(-1), pipeWriteFd(-1), bytesWritten(0), startTime(0) {}
};

class WebServer
{
	private:
		std::vector<ServerConfig>			_servers;
		std::map<int, const ServerConfig>	_listeningSockets;
		std::map<int, Client>				_clients;
		int									_epollFD;
		std::string							_selfPath;
		std::vector<std::string>			_envp;

		void	handleNewConnection(int currentFd, const ServerConfig& config);
		void	setServerAdress(const int& serverFd, sockaddr_in& serverAdress, size_t i);
		void	handleClientDisconnection(int currentFd);
		void	handleClientWrite(int currentFd);
		void	handleClientRead(int currentFd);
		void	switchToWrite(int clientFd);
		void	switchToRead(int clientFd);
		void	executeRequest(Client& currentClient);
		void	failedRequest(const HttpStatus& status, Client& currentClient);
		void	checkClientTimeouts();
		void	handleCgiRequest(Client& currentClient);
		void	handlePostRequest(Client& currentClient);
		void	addFdToEpoll(int fd, uint32_t events);
		void	handleCgiEvent(int pipeFd, uint32_t events);
		void	writeCgiBody(int clientFd, Client& currentClient, CgiHandler& cgi);
		void	readCgiOutput(int clientFd, Client& currentClient, CgiHandler& cgi);
		void	finalizeCgiResponse(int clientFd, Client& currentClient, CgiHandler& cgi);
		void	abortCgi(int clientFd, CgiHandler& cgi);
		void	detachPipeFd(int fd);
		void	checkCgiTimeouts();

	public:
		void	init();
		void	run();
		void	cleanup();
		std::map<int, int>			_cgiFd;
		std::map<int, int>			_pipeToClient;
		std::map<int, CgiHandler>	_clientToCgi;
		class signalException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		WebServer(const std::vector<ServerConfig>& configs, char **envp);
		~WebServer();
};

void handleDeleteRequest(Client& currentClient, const std::string& filePath);
void handleGetRequest(Client& currentClient);

#endif
