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
	std::cout << "New connection on " << config._serverName[0] << " server" << std::endl;
	while (true) {
		struct sockaddr_in clientAddress;
		socklen_t clientAddrLen = sizeof(clientAddress);
		int clientFd = accept(currentFd, (struct sockaddr*)&clientAddress, &clientAddrLen);

		if (clientFd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			} else {
				std::cerr << "Error: accept() failed" << std::endl;
				break;
			}
		}
		setNonBlocking(clientFd);
		struct epoll_event event = {};
		event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
		event.data.fd = clientFd;
		if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, clientFd, &event) == -1) {
			std::cerr << "Error: epoll_ctl failed to add client fd" << std::endl;
			close(clientFd);
			continue;
		}
		_clients.insert(std::make_pair(clientFd, Client(clientFd, config)));
		std::cout << "New connection accepted on fd: " << clientFd << std::endl;
	}
}

void WebServer::handleClientDisconnection(int clientFd)
{
	if (_clients.find(clientFd) == _clients.end())
		return;
	if (_clientToCgi.count(clientFd)) {
		CgiHandler& cgi = _clientToCgi[clientFd];
		//kill the cgi process
		if (cgi.pid > 0) {
			kill(cgi.pid, SIGKILL);
			waitpid(cgi.pid, NULL, 0);
		}
		//clean the read pipe
		if (cgi.pipeReadFd != -1) {
			epoll_ctl(_epollFD, EPOLL_CTL_DEL, cgi.pipeReadFd, NULL);
			close(cgi.pipeReadFd);
			_pipeToClient.erase(cgi.pipeReadFd);
		}
		//clean the write pipe
		if (cgi.pipeWriteFd != -1) {
			epoll_ctl(_epollFD, EPOLL_CTL_DEL, cgi.pipeWriteFd, NULL);
			close(cgi.pipeWriteFd);
			_pipeToClient.erase(cgi.pipeWriteFd);
		}
		_clientToCgi.erase(clientFd);
	}
	std::cout << "\033[31m" << "Client " << clientFd << " disconnected." << "\033[0m" << std::endl;
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, clientFd, NULL);
	close(clientFd);
	_clients.erase(clientFd);
}
