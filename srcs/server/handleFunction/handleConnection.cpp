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

void WebServer::handleClientDisconnection(int clientFd)
{
    // Étape 1: Vérifier si le client existe encore. Si non, on a déjà tout nettoyé.
    if (_clients.find(clientFd) == _clients.end()) {
        // Affiche un warning si vous voulez, mais ne faites rien de plus.
        // std::cerr << "Warning: tentative de déconnexion d'un client déjà nettoyé: " << clientFd << std::endl;
        return;
    }

    // Étape 2: Nettoyer le CGI associé s'il y en a un
    if (_clientToCgi.count(clientFd))
    {
        CgiHandler& cgi = _clientToCgi[clientFd];

        // Tuer le processus CGI pour être sûr qu'il ne devienne pas un zombie
        if (cgi.pid > 0) {
            kill(cgi.pid, SIGKILL);
            waitpid(cgi.pid, NULL, 0); // Nettoyer le processus zombie
        }

        // Nettoyer le pipe de lecture
        if (cgi.pipeReadFd != -1) {
            epoll_ctl(_epollFD, EPOLL_CTL_DEL, cgi.pipeReadFd, NULL); // Ignorer l'erreur si déjà retiré
            close(cgi.pipeReadFd);
            _pipeToClient.erase(cgi.pipeReadFd);
        }

        // Nettoyer le pipe d'écriture
        if (cgi.pipeWriteFd != -1) {
            epoll_ctl(_epollFD, EPOLL_CTL_DEL, cgi.pipeWriteFd, NULL); // Ignorer l'erreur si déjà retiré
            close(cgi.pipeWriteFd);
            _pipeToClient.erase(cgi.pipeWriteFd);
        }

        _clientToCgi.erase(clientFd);
    }
    
    // Étape 3: Nettoyer le client lui-même
    std::cout << "Client " << clientFd << " déconnecté." << std::endl;
    epoll_ctl(_epollFD, EPOLL_CTL_DEL, clientFd, NULL); // Ignorer l'erreur
    close(clientFd); // <-- L'APPEL LE PLUS IMPORTANT !
    _clients.erase(clientFd);
}
