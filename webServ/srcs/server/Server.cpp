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

#include "../../includes/server/Server.hpp"

#ifndef MAX_EVENTS
# define MAX_EVENTS 64
#endif


//Cannonical class//////////////////////////////////////////////////////////////

Server::Server(const ServerConfig& config) : _config(config)
{

}

Server::~Server()
{
    close(this->_listeningSocket);
    if (_epollFD != -1)
        close (_epollFD);
}

Server::Server(const Server& other) 
    : _listeningSocket(other._listeningSocket), _config(other._config) 
{

}

Server& Server::operator=(const Server& other)
{
    if (this != &other)
    {
        this->_listeningSocket = other._listeningSocket;
    }
    return (*this);
}

////////////////////////////////////////////////////////////////////////////////

//Member Function///////////////////////////////////////////////////////////////






