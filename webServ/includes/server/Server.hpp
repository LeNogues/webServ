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

# include <iostream>
# include <vector>
# include <cstdio>
# include <sstream>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include  <unistd.h>
# include "../config/ServerConfig.hpp"

class Server
{
    private:
        std::map<int, const ServerConfig&>      _socketToServerConfig;
        std::vector<int>                        _listeningSockets;
        const ServerConfig&                     _config;
        int                                     _epollFD;
        Server(const Server& other);
        Server& operator=(const Server& other);


            
 
    public:

        void initServer(const ServerConfig& config);

        Server(const ServerConfig& config);
        ~Server();

};

#endif