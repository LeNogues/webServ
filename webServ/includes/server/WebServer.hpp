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
# include "../config/ServerConfig.hpp"

class WebServer
{
    private:
        int                                 _epollFD;    
        std::vector<ServerConfig>&          _servers;
        std::map<int, const ServerConfig&>  _listeningSockets;
        void handleClientDisconnection(int currentFd);

    public:
        WebServer(std::vector<ServerConfig>& configs);
        ~WebServer();
        void    init();
        void    run();
};

#endif
