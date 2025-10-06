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

# include <map>
# include <vector>
# include "Server.hpp"
# include "../config/ServerConfig.hpp"

class WebServer
{
    private:
        int                                 _epollFD;    
        std::vector<ServerConfig>&          _servers;
        std::map<int, const ServerConfig&>  _listeningSockets;

    public:
        WebServer(std::vector<ServerConfig>& configs);
        ~WebServer();
        void    init();
};

#endif
