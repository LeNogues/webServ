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

#include "../../includes/server/WebServer.hpp"


WebServer::WebServer(std::vector<ServerConfig>& configs)
    : _servers(configs)
{

}

WebServer::~WebServer()
{

}

static void errorInit(const std::string& Error, const std::string& value, const int& serverFd)
{
    close(serverFd);
    throw std::runtime_error(Error + value);
}


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

void WebServer::init()
{
    _epollFD = epoll_create1(0);
    if (_epollFD == -1)
        throw std::runtime_error("ERROR:can't create epoll instance");
    
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)
            throw std::runtime_error("ERROR: could not create socket for " + _servers[i]._serverName[0]);

        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            errorInit("ERROR: setsockopt failed for", _servers[i]._serverName[0], serverFd);

        struct sockaddr_in serverAdress;
        serverAdress.sin_family = AF_INET;
        serverAdress.sin_port = htons(_servers[i]._listenOn.second);
        serverAdress.sin_addr.s_addr = inet_addr(_servers[i]._listenOn.first.c_str());
        if (serverAdress.sin_addr.s_addr == INADDR_NONE)
            errorInit("ERROR: Adresse IP invalide: ", _servers[i]._listenOn.first, serverFd);

        if (bind(serverFd, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0)
            errorInit("ERROR: failed to bind to port ", intToString(_servers[i]._listenOn.second), serverFd);

        setNonBlocking(serverFd);
        
        if (listen(serverFd, SOMAXCONN) < 0)
            errorInit("ERROR: listen failed for ", _servers[i]._serverName[0], serverFd);

        struct epoll_event event = {}; // Initialisation à zéro
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = serverFd;
        if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, serverFd, &event) == -1) {
            throw std::runtime_error("ERROR: epoll_ctl failed to add listening socket");
        }
        _listeningSockets.insert(std::make_pair(serverFd, _servers[i]));
    }
}

void handleNewConnection(int currentFd)
{
    (void)currentFd;
}

void WebServer::handleClientDisconnection(int currentFd)
{
    if (epoll_ctl(_epollFD, EPOLL_CTL_DEL, currentFd, NULL) == -1)
        std::cerr << "Warning: epoll-ctl(DEL) failed for fd : " << currentFd << std::endl;
    close(currentFd);
    size_t erased_count = _listeningSockets.erase(currentFd);
    if (erased_count > 0)
        std::cout << "Client on fd " << currentFd << " disconnected and cleaned up." << std::endl;
    else
        std::cerr << "Warning: tried to erase non-existant client for fd :" << currentFd << std::endl; 
}

void handleClientWrite(int currentFd)
{
    (void)currentFd;
}

void handleClientRead(int currentFd)
{
    (void)currentFd;
}

void WebServer::run()
{

    epoll_event events[MAX_EVENTS];

    while(1)
    {
        int numEvent = epoll_wait(_epollFD, events, MAX_EVENTS, -1);
        if (numEvent == -1)
        {
            if (errno == EINTR)
                continue ;
            else
            {
                //pensez a fermer les fd ou faire une classe de catch special
                throw std::runtime_error("ERROR: erreur critique de epoll_wait");
            } 
        }

        for (int i = 0; i < numEvent; ++i)
        {
            int currentFd = events[i].data.fd;
            if (events[i].events == EPOLLERR || events[i].events == EPOLLHUP)
            {
                handleClientDisconnection(currentFd);
                continue ;
            }
            //la ligne fait peur mais elle cree juste un iterateur
            std::map<int, const ServerConfig &, std::less<int>, std::allocator<std::pair<const int, const ServerConfig &> > >::iterator currentServer = _listeningSockets.find(currentFd);
            if (currentServer == _listeningSockets.end())
            {
                if (events[i].events & EPOLLIN)
                    handleClientRead(currentFd);
                else if (events[i].events & EPOLLOUT)
                    handleClientWrite(currentFd);
            }
            else
                handleNewConnection(currentFd);
        }
    }
}