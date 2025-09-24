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

Server::Server(const Config& config) : _config(config)
{

}

Server::~Server()
{
    for (size_t i = 0; i < _listeningSockets.size(); ++i)
        close(this->_listeningSockets[i]);
}

Server::Server(const Server& other) 
    : _config(other._config), _listeningSockets(other._listeningSockets)
{

}

Server& Server::operator=(const Server& other)
{
    if (this != &other)
    {
        this->_listeningSockets = other._listeningSockets;
    }
}