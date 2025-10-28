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

#include "../../../includes/server/WebServer.hpp"

void WebServer::handleNewConnection(int currentFd, const ServerConfig& config)
{

    std::cout << "\n------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "during NewConnection " << config._serverName[0] << std::endl;
    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int clientFd = accept(currentFd, (struct sockaddr*)&clientAddress, &clientAddrLen);

        if (clientFd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                std::cerr << "Erreur lors de accept()" << std::endl;
                break;
            }
        }

        setNonBlocking(clientFd);

        struct epoll_event event = {};
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        event.data.fd = clientFd;
        if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, clientFd, &event) == -1)
        {
            std::cerr << "Erreur: epoll_ctl ne peut pas ajouter le client fd" << std::endl;
            close(clientFd);
            continue;
        }

        _clients.insert(std::make_pair(clientFd, Client(clientFd, config)));
        std::cout << "Nouvelle connexion acceptée sur le fd: " << clientFd << std::endl;
    }
}

void WebServer::handleClientDisconnection(int currentFd)
{
    if (epoll_ctl(_epollFD, EPOLL_CTL_DEL, currentFd, NULL) == -1)
        std::cerr << "Warning: epoll-ctl(DEL) failed for fd : " << currentFd << std::endl;
    close(currentFd);
    size_t erased_count = _clients.erase(currentFd);
    if (erased_count > 0)
    {
        std::cout << "Client on fd " << currentFd << " disconnected and cleaned up." << std::endl;
        std::cout << "------------------------------------------------------------------------------------------------------------------------------\n" << std::endl;
    }
    else
        std::cerr << "Warning: tried to erase non-existant client for fd :" << currentFd << std::endl;
}
