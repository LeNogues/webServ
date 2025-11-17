/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                        webServ                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   this is just                                   +#+  +:+       +#+        */
/*   to fill it up                                fibrous-root   +#+           */
/*   so it look                                        #+#    #+#             */
/*   ✨ aestheticaly pleasing ✨                      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/server/WebServer.hpp"

void WebServer::handleClientWrite(int currentFd)
{
	std::map<int, Client>::iterator it = _clients.find(currentFd);
	if (it == _clients.end())
	{
		writeError("Error: write for unknown client fd: " + intToString(currentFd));
		handleClientDisconnection(currentFd);
		return;
	}
	Client&	currentClient = it->second;

	if (!currentClient.hasResponse()) {
		if (_clientToCgi.count(currentFd))
			return;
		writeError("Error: no response ready for fd: " + intToString(currentFd));
		handleClientDisconnection(currentFd);
		return;
	}
	if (currentClient.sendPending() <= 0) {
		writeError("Error: send() failed for fd: " + intToString(currentFd));
		handleClientDisconnection(currentFd);
		return;
	}
	if (currentClient.hasResponse()) {
		return;
	}
	if (currentClient.getShouldClose()) {
		handleClientDisconnection(currentFd);
	} else {
		currentClient.getRequest().clear();
		switchToRead(currentFd);
	}
}

void WebServer::executeRequest(Client& currentClient)
{
	std::string	method = currentClient.getRequest().getMethod();
	std::string	uri = currentClient.getRequest().getPath();

	if (currentClient.getRequest().getIsCGI())
		handleCgiRequest(currentClient);
	else if (method == "DELETE")
		handleDeleteRequest(currentClient, uri);
	else if (method == "POST")
		handlePostRequest(currentClient);
	else if (method == "GET")
		handleGetRequest(currentClient);
}

void WebServer::failedRequest(const HttpStatus& status, Client& currentClient)
{
	std::string									body = "";
	std::map<std::string, std::string>			headers;
	std::map<int, std::string>					errorPage = currentClient.getRequest().getLocation()._errorPage;
	std::map<int, std::string>::const_iterator	itPage = errorPage.find(status.getStatusCode());
	std::pair<int, std::string>					redirect = currentClient.getRequest().getLocation()._redirect;

	if (redirect.first != 0 && !redirect.second.empty()) {
		if (redirect.first >= 300 && redirect.first <= 308)
			headers["Location"] = redirect.second;
		else
			body = redirect.second;
	} else if (itPage != errorPage.end() && fileToString(itPage->second, body) == true) {
		headers["Content-Type"] = guessContentType(itPage->second);
	} else {
		body = generateErrorPage(status.what());
		headers["Content-Type"] = "text/html";
	}
	currentClient.generateResponse(status.getStatusCode(), headers, body);
}

void WebServer::handleClientRead(int currentFd)
{
	std::map<int, Client>::iterator it = _clients.find(currentFd);
	if (it == _clients.end()) {
		writeError("Error: Received data for a non-existent client fd: " + intToString(currentFd));
		handleClientDisconnection(currentFd);
		return;
	}
	Client&	currentClient = it->second;
	char	buffer[4096];
	ssize_t	bytes_read;

	try {
		while (!currentClient.getRequest().getIsValid()) {
			bytes_read = recv(currentFd, buffer, sizeof(buffer), 0);
			if (bytes_read <= 0) {
				if (bytes_read == 0)
					std::cout << "Client on fd " << currentFd << " closed the connection." << std::endl;
				else
					writeError("Error: recv() failed for fd: " + intToString(currentFd));
				handleClientDisconnection(currentFd);
				return;
			}
			currentClient.getRequest().parseRequest(std::string(buffer, bytes_read));
		}
		currentClient.setLocation(currentClient.getRequest().getLocation());
		currentClient.getRequest().logRequest( currentFd );
		executeRequest(currentClient);
	} catch(const HttpStatus& status) {
		if (!currentClient.getRequest().getIsValid()) {
			std::cout << "\033[31m" << "Unvalid request on fd " << currentFd << " -----------------------------------------------" << "\033[0m" << std::endl;
			std::cout << std::string(buffer, bytes_read) << std::endl;
		}
		failedRequest(status, currentClient);
	}
	if (_clientToCgi.count(currentFd) == 0 || currentClient.hasResponse())
		switchToWrite(currentFd);
}
