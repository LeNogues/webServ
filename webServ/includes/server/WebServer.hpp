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
# endif

# include <map>
# include <vector>
# include <cerrno>
# include <cstdio>
# include <fcntl.h>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include <sys/stat.h>
# include <sys/wait.h>
# include "Server.hpp"
# include "Client.hpp"
# include "../httpGen/generateErrorPage.hpp"
# include "../config/ServerConfig.hpp"
# include "../utils/converters.hpp"
# include "../request/Request.hpp"
# include "../utils/parsePath.hpp"
# include "../utils/setNonBlocking.hpp"
# include "../httpGen/generateAutoIndex.hpp"
# include "../server/handleCgi.hpp"


class WebServer
{
    private:
        std::vector<ServerConfig>           _servers;
        std::map<int, const ServerConfig>   _listeningSockets;
        std::map<int, Client>               _clients;
        int                                 _epollFD;
		std::string							_selfPath;
		std::vector<std::string>			_envp;

        void	 handleNewConnection(int currentFd, const ServerConfig& config);
        void	 setServerAdress(const int& serverFd, sockaddr_in& serverAdress, size_t i);
        void	 handleClientDisconnection(int currentFd);
        void	 handleClientWrite(int currentFd);
        void	 handleClientRead(int currentFd);
        void	 switchToWrite(int clientFd);
        void	 switchToRead(int clientFd);
        void     executeRequest(Client& currentClient, int& currentFd);
        void     failedRequest(const HttpStatus& status, Client& currentClient, int& currentFd);


    public:
        void    init();
        void    run();

        class signalException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };

        WebServer(const std::vector<ServerConfig>& configs, char **envp);
        ~WebServer();
};

void handleDeleteRequest(Client& currentClient, const std::string& filePath);
void handlePostRequest(Client& currentClient, const std::vector<std::string>& envp);
void handleGetRequest(Client &);

#endif
