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

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <cstdio>
# include <fcntl.h>
# include <sstream>
# include <iostream>
# include <unistd.h>
# include <sys/epoll.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include "../config/ServerConfig.hpp"

class Server
{
    private:
        const ServerConfig&                     _config;
        int                                     _listeningSocket;
        int                                     _epollFD;

        Server(const Server& other);
        Server& operator=(const Server& other);

    public:

        void    runServer();

        Server(const ServerConfig& config);
        ~Server();

};


void    runServer(std::vector<ServerConfig> &servers);

#endif