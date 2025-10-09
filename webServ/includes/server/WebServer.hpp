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
# include "Server.hpp"
# include "Client.hpp"
# include "../config/ServerConfig.hpp"
# include "../request/Request.hpp"

class WebServer
{
    private: 
        std::vector<ServerConfig>&          _servers;
        std::map<int, const ServerConfig&>  _listeningSockets;
        std::map<int, Client>               _clients;
        int                                 _epollFD;   

        void handleNewConnection(int currentFd, const ServerConfig& config);
        void setServerAdress(const int& serverFd, sockaddr_in& serverAdress, size_t i);
        void handleClientDisconnection(int currentFd);
        void handleClientWrite(int currentFd);
        void handleClientRead(int currentFd);
        void switchToWrite(int clientFd);
        void switchToRead(int clientFd);
        

    public:
        void    init();
        void    run();
        
        WebServer(std::vector<ServerConfig>& configs);
        ~WebServer();
};

#endif
