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

void WebServer::handlePostRequest(Client& currentClient)
{
	std::string contentType = currentClient.getRequest().getHeader("Content-Type");
	if(contentType.find("multipart/form-data") != std::string::npos) {
		std::cout << "post Path:" << currentClient.getRequest().getPath() << std::endl;
		currentClient.getRequest().setSecondPath(currentClient.getRequest().getUri());
		currentClient.getRequest().setPath("./cgi/upload.php");
		std::cout << "SecondPath:" << currentClient.getRequest().getSecondPath() << std::endl;
		std::cout << "Path:"  << currentClient.getRequest().getPath() << std::endl;
		handleCgiRequest(currentClient);
		return ;
	}
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
	std::cout << "Fichier créé avec succès: " << filePath << std::endl;
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = "text/plain";
	currentClient.generateResponse(201, headers, "Ressource créée avec succès.");
}
