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

void WebServer::switchToRead(int clientFd)
{
    epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLIN;

    if (epoll_ctl(_epollFD, EPOLL_CTL_MOD, clientFd, &event) == -1)
    {
        std::cerr << "ERROR: can't change epoll for write request on fd : " << clientFd << std::endl;
        handleClientDisconnection(clientFd);
    }
}

void WebServer::switchToWrite(int clientFd)
{
    epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLOUT;

    if (epoll_ctl(_epollFD, EPOLL_CTL_MOD, clientFd, &event) == -1)
    {
        std::cerr << "ERROR: can't change epoll for write request on fd : " << clientFd << std::endl;
        handleClientDisconnection(clientFd);
    }
}
