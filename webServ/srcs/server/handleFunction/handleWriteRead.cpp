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

    if (sent == -1)
    {
        std::cerr << "Error: send() failed for fd: " << currentFd << std::endl;
        handleClientDisconnection(currentFd);
        return;
    }

    if (currentClient.hasResponse())
        return;

    if (currentClient.getShouldClose())
        handleClientDisconnection(currentFd);
    else
    {
        currentClient.getRequest().clear();
        switchToRead(currentFd);
    }
}

void WebServer::executeRequest(Client& currentClient, int& currentFd)
{
        char buffer[4096];
        ssize_t bytes_read;

        std::cout << "Request received on fd " << currentFd << std::endl;
        std::cout << "\n--- Request Start ---" << std::endl;

        while (true)
        {
            bytes_read = recv(currentFd, buffer, sizeof(buffer), 0);

            if (bytes_read > 0)
            {
                currentClient.getRequest().parseRequest(std::string(buffer, bytes_read));
            }
            else if (bytes_read == 0)
            {
                std::cout << "Client on fd " << currentFd << " closed the connection." << std::endl;
                handleClientDisconnection(currentFd);
                return;
            }
            else
                break;
        }
        currentClient.setLocation(currentClient.getRequest().getLocation());
        currentClient.getRequest().logRequest();

        std::cout << "--- Request End ---" << std::endl;

        std::string method = currentClient.getRequest().getMethod();
        std::string uri = currentClient.getRequest().getPath();

        if (currentClient.getRequest().getIsCGI())
            handleCgiRequest(currentClient, _envp);
        if (method == "DELETE")
            handleDeleteRequest(currentClient, uri);
        else if (method == "POST")
            handlePostRequest(currentClient, uri);
        else if (method == "GET")
            handleGetRequest(currentClient);

        switchToWrite(currentFd);
}

void WebServer::failedRequest(const HttpStatus& status, Client& currentClient, int& currentFd)
{
    std::cerr << status.what() << '\n';
    std::string									body = "";
    std::map<std::string, std::string>			headers;
    std::map<int, std::string>::const_iterator	itPage = currentClient.getErrorPage().find(status.getStatusCode());
    if (itPage != currentClient.getErrorPage().end())
    {
        std::string	customPath = itPage->second;
        std::string	fileContent;
        if (fileToString(customPath, fileContent) == true)
        {
            headers["Content-Type"] = guessContentType(customPath);
            body = fileContent;
        }
        else
        {
            body = generateErrorPage(status.what());
            headers["Content-Type"] = "text/html";
        }
    }
    else
    {
        body = generateErrorPage(status.what());
        headers["Content-Type"] = "text/html";
    }
    currentClient.generateResponse(status.getStatusCode(), headers, body);
    switchToWrite(currentFd);
}

void WebServer::handleClientRead(int currentFd)
{
    std::map<int, Client>::iterator it = _clients.find(currentFd);
    Client& currentClient = it->second;

    if (it == _clients.end())
    {
        std::cerr << "Error: Received data for a non-existent client fd: " << currentFd << std::endl;
        handleClientDisconnection(currentFd);
        return;
    }

    try
    {
        executeRequest(currentClient, currentFd);
    }
    catch(const HttpStatus& status)
    {
        failedRequest(status, currentClient, currentFd);
    }
}

