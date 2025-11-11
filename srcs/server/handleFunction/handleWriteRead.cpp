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
		std::cerr << "Error: write for unknown client fd: " << currentFd << std::endl;
		handleClientDisconnection(currentFd);
		return;
	}
	Client& currentClient = it->second;
	ssize_t sent = currentClient.sendPending();
	if (sent < 0) {
		std::cerr << "Error: send() failed for fd: " << currentFd << std::endl;
		handleClientDisconnection(currentFd);
		return;
	}
	if (currentClient.hasResponse())
		return;
	if (currentClient.getShouldClose()) {
		handleClientDisconnection(currentFd);
	} else {
		currentClient.getRequest().clear();
		switchToRead(currentFd);
	}
}

void WebServer::executeRequest(Client& currentClient, int& currentFd)
{
	std::cout << "Request complete on fd " << currentFd << ", executing..." << std::endl;
	std::cout << "\n--- Request Start ---" << std::endl;
	currentClient.setLocation(currentClient.getRequest().getLocation());
	currentClient.getRequest().logRequest();
	std::cout << "--- Request End ---" << std::endl;
	std::string method = currentClient.getRequest().getMethod();
	std::string uri = currentClient.getRequest().getPath();
	if (currentClient.getRequest().getIsCGI())
		handleCgiRequest(currentClient, _envp);
	else if (method == "DELETE")
		handleDeleteRequest(currentClient, uri);
	else if (method == "POST")
		handlePostRequest(currentClient, _envp);
	else if (method == "GET")
		handleGetRequest(currentClient);
	switchToWrite(currentFd);
}

void WebServer::failedRequest(const HttpStatus& status, Client& currentClient, int& currentFd)
{
	std::cerr << status.what() << '\n';
	std::string									body = "";
	std::map<std::string, std::string>			headers;
	std::map<int, std::string>					errorPage = currentClient.getRequest().getLocation()._errorPage;
	std::pair<int, std::string>					redirect = currentClient.getRequest().getLocation()._redirect;
	std::map<int, std::string>::const_iterator	itPage = errorPage.find(status.getStatusCode());

	if (redirect.first != 0 && !redirect.second.empty()) {
		if (redirect.first >= 300 && redirect.first <= 308)
			headers["Location"] = redirect.second;
		else
			body = redirect.second;
	} else if (itPage != errorPage.end()) {
		std::string customPath = itPage->second;
		std::string fileContent;
		if (fileToString(customPath, fileContent) == true) {
			headers["Content-Type"] = guessContentType(customPath);
			body = fileContent;
		} else {
			body = generateErrorPage(status.what());
			headers["Content-Type"] = "text/html";
		}
	} else {
		body = generateErrorPage(status.what());
		headers["Content-Type"] = "text/html";
	}
	currentClient.generateResponse(status.getStatusCode(), headers, body);
	switchToWrite(currentFd);
}

void WebServer::handleClientRead(int currentFd)
{
	std::map<int, Client>::iterator it = _clients.find(currentFd);
	if (it == _clients.end()) {
		std::cerr << "Error: Received data for a non-existent client fd: " << currentFd << std::endl;
		handleClientDisconnection(currentFd);
		return;
	}
	Client& currentClient = it->second;
	char buffer[4096];
	ssize_t bytes_read;
	try {
		while (true)
		{
			bytes_read = recv(currentFd, buffer, sizeof(buffer), 0);

			if (bytes_read > 0)
				currentClient.getRequest().parseRequest(std::string(buffer, bytes_read));
			else if (bytes_read == 0)
			{
				std::cout << "Client on fd " << currentFd << " closed the connection." << std::endl;
				handleClientDisconnection(currentFd);
				return;
			}
			else
				break;
		}
		if (currentClient.getRequest().getIsValid())
				executeRequest(currentClient, currentFd);
	} catch(const HttpStatus& status) {
		failedRequest(status, currentClient, currentFd);
	}
}
