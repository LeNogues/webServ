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

void handleDeleteRequest(Client& currentClient, const std::string& filePath)
{
    struct stat buffer;

    if (stat(filePath.c_str(), &buffer) != 0)
    {
        std::cout << "Warning: cannot delete: " << filePath << std::endl;
        throw HttpStatus(404);
    }

    if (std::remove(filePath.c_str()) == -1)
    {
        std::cout << "Warning: cannot delete: " << filePath << std::endl;
        throw HttpStatus(500);
    }
    std::map<std::string, std::string> headers;
    currentClient.generateResponse(204, headers, "");
    std::cout << filePath << " successfully deleted" << std::endl;
}

void handlePostRequest(Client& currentClient, std::string uri)
{
    (void)currentClient;
    (void)uri;

    std::cout << "POST request received" << std::endl;
}

void handleGetRequest(Client &currentClient) {
	std::string							path;
	std::string							body;
	std::map<std::string, std::string>	headers;
	std::string							contentType;

	path = currentClient.getRequest().getPath();
	contentType = guessContentType(path);
	if (fileToString(path, body) == false)
        throw HttpStatus(404);

	headers["Content-Type"] = contentType;
	headers["Content-Length"] = intToString(static_cast<int>(body.size()));

	std::string connectionHeader = currentClient.getRequest().getHeader("Connection");
    if (connectionHeader == "close") {
        headers["Connection"] = "close";
        currentClient.setShouldClose(true);
    } else {
        headers["Connection"] = "keep-alive";
        currentClient.setShouldClose(false);
    }

	currentClient.generateResponse(200, headers, body);
}