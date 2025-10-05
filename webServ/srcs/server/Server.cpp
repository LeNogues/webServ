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
    initServer(_config);
}

Server::~Server()
{
    for (size_t i = 0; i < _listeningSockets.size(); ++i)
        close(this->_listeningSockets[i]);
    if (_epollFD != -1)
        close (_epollFD);
}

Server::Server(const Server& other) 
    : _listeningSockets(other._listeningSockets), _config(other._config) 
{

}

Server& Server::operator=(const Server& other)
{
    if (this != &other)
    {
        this->_listeningSockets = other._listeningSockets;
    }
    return (*this);
}

////////////////////////////////////////////////////////////////////////////////

//Member Function///////////////////////////////////////////////////////////////


static void setNonBlocking(int serverFD)
{
    int flags = fcntl(serverFD, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("ERROR: fcntl failed");
    
    flags |= O_NONBLOCK;
    int s = fcntl(serverFD, F_SETFL, flags);
    if (s == -1)
        throw std::runtime_error("ERROR: fcntl failed");
}

static std::string intToString(int intToConvert)
{
    std::string str;
    std::ostringstream convertStream;
    convertStream << intToConvert;
    str = convertStream.str();
    return (str);
}

static void errorInit(const std::string& Error, const std::string& value, const int& serverFd)
{
    close(serverFd);
    throw std::runtime_error(Error + value);
}

void Server::initServer(const ServerConfig& config)
{
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1)
        throw std::runtime_error("ERROR: could not create socket for " + config._serverName[0]);

    int opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        errorInit("ERROR: setsockopt failed for", config._serverName[0], serverFd);

    struct sockaddr_in serverAdress;
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(config._listenOn.second);
    serverAdress.sin_addr.s_addr = inet_addr(config._listenOn.first.c_str());
    if (serverAdress.sin_addr.s_addr == INADDR_NONE)
        errorInit("ERROR: Adresse IP invalide: ", config._listenOn.first, serverFd);

    if (bind(serverFd, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0)
        errorInit("ERROR: failed to bind to port ", intToString(config._listenOn.second), serverFd);

    setNonBlocking(serverFd);
    if (listen(serverFd, SOMAXCONN) < 0)
        errorInit("ERROR: listen failed for ", config._serverName[0], serverFd);
    _listeningSockets.push_back(serverFd);
}
