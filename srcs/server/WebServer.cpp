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

WebServer::WebServer(const std::vector<ServerConfig>& configs, char **envp)
	: _servers(configs)
{
	for (size_t i = 0; envp[i]; ++i)
		_envp.push_back(envp[i]);
}

WebServer::~WebServer()
{
}

static void errorInit(const std::string& Error, const std::string& value, const int& serverFd)
{
	close(serverFd);
	throw std::runtime_error(Error + value);
}

void WebServer::setServerAdress(const int& serverFd, sockaddr_in& serverAdress, size_t i)
{
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(_servers[i]._listenOn.second);
	serverAdress.sin_addr.s_addr = inet_addr(_servers[i]._listenOn.first.c_str());
	if (serverAdress.sin_addr.s_addr == INADDR_NONE)
		errorInit("ERROR: Invalid IP address: ", _servers[i]._listenOn.first, serverFd);
}


void WebServer::run()
{
	epoll_event	events[MAX_EVENTS];
	int			numEvent;
	int			index;
	int			currentFd;
	uint32_t	eventMask;

	while (true) {
		numEvent = epoll_wait(_epollFD, events, MAX_EVENTS, 10000);
		if (numEvent == -1) {
			if (errno == EINTR)
				continue;
			else
				throw std::runtime_error("ERROR: critical error in epoll_wait");
		}
		index = 0;
		while (index < numEvent) {
			currentFd = events[index].data.fd;
			eventMask = events[index].events;
			if (_pipeToClient.count(currentFd)) {
				handleCgiEvent(currentFd, eventMask);
				index++;
				continue;
			}
			if (eventMask & (EPOLLERR | EPOLLHUP)) {
				handleClientDisconnection(currentFd);
				index++;
				continue;
			}
			if (_listeningSockets.count(currentFd)) {
				handleNewConnection(currentFd, _listeningSockets[currentFd]);
			} else {
				if (eventMask & EPOLLIN)
					handleClientRead(currentFd);
				else if (eventMask & EPOLLOUT)
					handleClientWrite(currentFd);
			}
			index++;
		}
		checkCgiTimeouts();
	}
}

void	WebServer::checkCgiTimeouts()
{
	std::map<int, CgiHandler>::iterator	it;
	std::map<int, CgiHandler>::iterator	next;
	std::map<int, Client>::iterator		clientIt;
	std::time_t							currentTime;
	double								elapsedTime;
	HttpStatus							statusGatewayTimeout(504);

	currentTime = std::time(NULL);
	it = _clientToCgi.begin();
	while (it != _clientToCgi.end())
	{
		next = it;
		next++;
		elapsedTime = std::difftime(currentTime, it->second.startTime);
		if (elapsedTime >= CGI_TIMEOUT_SECONDS)
		{
			int	clientFd;

			clientFd = it->first;
			clientIt = _clients.find(clientFd);
			abortCgi(clientFd, it->second);
			if (clientIt != _clients.end())
			{
				failedRequest(statusGatewayTimeout, clientIt->second);
				switchToWrite(clientFd);
			}
		}
		it = next;
	}
}

void	WebServer::addFdToEpoll(int fd, uint32_t events)
{
	epoll_event	event;

	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, fd, &event) == -1)
		throw std::runtime_error("Error: epoll_ctl failed to add fd");
}

void	WebServer::detachPipeFd(int fd)
{
	if (fd == -1)
		return;
	epoll_ctl(_epollFD, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	_pipeToClient.erase(fd);
}

void	WebServer::abortCgi(int clientFd, CgiHandler& cgi)
{
	if (cgi.pipeReadFd != -1)
	{
		detachPipeFd(cgi.pipeReadFd);
		cgi.pipeReadFd = -1;
	}
	if (cgi.pipeWriteFd != -1)
	{
		detachPipeFd(cgi.pipeWriteFd);
		cgi.pipeWriteFd = -1;
	}
	if (cgi.pid > 0)
	{
		kill(cgi.pid, SIGKILL);
		waitpid(cgi.pid, NULL, WNOHANG);
	}
	_clientToCgi.erase(clientFd);
}

void	WebServer::writeCgiBody(int clientFd, Client& currentClient, CgiHandler& cgi)
{
	std::string	body;
	size_t		bodySize;
	const char	*data;
	size_t		remaining;
	ssize_t		written;

	if (cgi.pipeWriteFd == -1)
		return;
	body = currentClient.getRequest().getBody();
	bodySize = body.size();
	if (bodySize == 0 || cgi.bytesWritten >= bodySize)
	{
		detachPipeFd(cgi.pipeWriteFd);
		cgi.pipeWriteFd = -1;
		return;
	}
	data = body.data() + cgi.bytesWritten;
	remaining = bodySize - cgi.bytesWritten;
	written = write(cgi.pipeWriteFd, data, remaining);
	if (written == -1)
	{
		abortCgi(clientFd, cgi);
		HttpStatus	status(500);
		failedRequest(status, currentClient);
		switchToWrite(clientFd);
		return;
	}
	cgi.bytesWritten += static_cast<size_t>(written);
	if (cgi.bytesWritten >= bodySize)
	{
		detachPipeFd(cgi.pipeWriteFd);
		cgi.pipeWriteFd = -1;
	}
}

void	WebServer::finalizeCgiResponse(int clientFd, Client& currentClient, CgiHandler& cgi)
{
	bool	parsed;
	HttpStatus	statusBadGateway(502);

	if (cgi.pipeWriteFd != -1)
	{
		detachPipeFd(cgi.pipeWriteFd);
		cgi.pipeWriteFd = -1;
	}
	if (cgi.pid > 0)
		waitpid(cgi.pid, NULL, WNOHANG);
	parsed = currentClient.parseCgiResponse();
	_clientToCgi.erase(clientFd);
	if (!parsed)
		failedRequest(statusBadGateway, currentClient);
	switchToWrite(clientFd);
}

void	WebServer::readCgiOutput(int clientFd, Client& currentClient, CgiHandler& cgi)
{
	char	buffer[4096];
	ssize_t	bytesRead;

	if (cgi.pipeReadFd == -1)
		return;
	while (true)
	{
		memset(buffer, 0, 4096);
		bytesRead = read(cgi.pipeReadFd, buffer, sizeof(buffer));
		if (bytesRead > 0)
		{
			currentClient.appendToCgiBuffer(buffer, static_cast<size_t>(bytesRead));
			continue;
		}
		if (bytesRead == 0)
		{
			detachPipeFd(cgi.pipeReadFd);
			cgi.pipeReadFd = -1;
			finalizeCgiResponse(clientFd, currentClient, cgi);
			break;
		}
		if (buffer[0])
		{
			abortCgi(clientFd, cgi);
			HttpStatus	status(500);
			failedRequest(status, currentClient);
			switchToWrite(clientFd);
			break;
		}
	}
}

void	WebServer::handleCgiEvent(int pipeFd, uint32_t events)
{
	std::map<int, int>::iterator				itPipe;
	std::map<int, Client>::iterator				itClient;
	std::map<int, CgiHandler>::iterator			itHandler;
	int											clientFd;
	Client										*currentClient;
	HttpStatus									status(500);
	CgiHandler									*cgi;

	itPipe = _pipeToClient.find(pipeFd);
	if (itPipe == _pipeToClient.end())
		return;
	clientFd = itPipe->second;
	itClient = _clients.find(clientFd);
	if (itClient == _clients.end())
	{
		detachPipeFd(pipeFd);
		return;
	}
	itHandler = _clientToCgi.find(clientFd);
	if (itHandler == _clientToCgi.end())
	{
		detachPipeFd(pipeFd);
		return;
	}
	currentClient = &itClient->second;
	cgi = &itHandler->second;
	if (events & EPOLLERR)
	{
		abortCgi(clientFd, *cgi);
		failedRequest(status, *currentClient);
		switchToWrite(clientFd);
		return;
	}
	if (pipeFd == cgi->pipeWriteFd && (events & EPOLLOUT))
		writeCgiBody(clientFd, *currentClient, *cgi);
	if (pipeFd == cgi->pipeReadFd && (events & (EPOLLIN | EPOLLHUP)))
		readCgiOutput(clientFd, *currentClient, *cgi);
}

void WebServer::init()
{
	_epollFD = epoll_create(MAX_EVENTS);
	if (_epollFD == -1)
		throw std::runtime_error("ERROR:can't create epoll instance");

	for (size_t i = 0; i < _servers.size(); ++i) {
		int serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd == -1)
			throw std::runtime_error("ERROR: could not create socket for " + _servers[i]._serverName[0]);

		int opt = 1;
		if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			errorInit("ERROR: setsockopt failed for", _servers[i]._serverName[0], serverFd);

		struct sockaddr_in serverAdress;
		setServerAdress(serverFd, serverAdress, i);

		if (bind(serverFd, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0)
			errorInit("ERROR: failed to bind to port ", intToString(_servers[i]._listenOn.second), serverFd);

		setNonBlocking(serverFd);

		if (listen(serverFd, SOMAXCONN) < 0)
			errorInit("ERROR: listen failed for ", _servers[i]._serverName[0], serverFd);

		struct epoll_event event = {};
		event.events = EPOLLIN;
		event.data.fd = serverFd;
		if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, serverFd, &event) == -1) {
			throw std::runtime_error("ERROR: epoll_ctl failed to add listening socket");
		}
		_listeningSockets.insert(std::make_pair(serverFd, _servers[i]));
	}
}

void	WebServer::cleanup()
{
	std::map<int, Client>::iterator	it_cli = _clients.begin();
	std::vector<ServerConfig>::iterator it_serv = _servers.begin();

	for (; it_cli != _clients.end(); it_cli++)
		close(it_cli->second._clientFd);
	for (; it_serv != _server.end(); it_serv++)
		close(it_serv->
	close(_epollFD);
}

const char* WebServer::signalException::what() const throw()
{
	return ("\nServer interrupted by signal");
}

