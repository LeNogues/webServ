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

static void createFile(Client& currentClient)
{
	std::string filePath = currentClient.getRoot() + currentClient.getRequest().getUri();
	std::cout << "filePath is : " << filePath << std::endl;
	std::string body = currentClient.getRequest().getBody();
	size_t last_slash_idx = filePath.rfind('/');
	if (std::string::npos != last_slash_idx) {
		std::string directory = filePath.substr(0, last_slash_idx);
		if (!directory.empty())
			mkdir(directory.c_str(), 0755);
	}
	std::ofstream outputFile(filePath.c_str());
	if (!outputFile) {
		writeError("Erreur: impossible de créer le fichier: " + filePath);
		throw HttpStatus(500);
	}
	outputFile << body;
	outputFile.close();
	std::cout << "File created successfully: " << filePath << std::endl;
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = "text/plain";
	currentClient.generateResponse(201, headers, "Resource created successfully.");
}

void WebServer::handlePostRequest(Client& currentClient)
{
	if(currentClient.getRequest().getHeader("Content-Type").find("multipart/form-data") != std::string::npos) {
		currentClient.getRequest().setSecondPath(currentClient.getRequest().getUri());
		currentClient.getRequest().setPath("./cgi/upload.php");
		handleCgiRequest(currentClient);
	} else {
		createFile(currentClient);
	}
}
