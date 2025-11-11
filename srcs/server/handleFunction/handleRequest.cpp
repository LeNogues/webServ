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

	if (stat(filePath.c_str(), &buffer) != 0) {
		std::cout << "Warning: cannot delete: " << filePath << std::endl;
		throw HttpStatus(404);
	}
	if (std::remove(filePath.c_str()) == -1) {
		std::cout << "Warning: cannot delete: " << filePath << std::endl;
		throw HttpStatus(500);
	}
	std::map<std::string, std::string> headers;
	currentClient.generateResponse(204, headers, "");
	std::cout << filePath << " successfully deleted" << std::endl;
}

void WebServer::handlePostRequest(Client& currentClient, const std::vector<std::string>& envp)
{
	(void)envp;

	std::string contentType = currentClient.getRequest().getHeader("Content-Type");
	if(contentType.find("multipart/form-data") != std::string::npos) {
		std::cout << "post Path:" << currentClient.getRequest().getPath() << std::endl;
		currentClient.getRequest().setSecondPath(currentClient.getRequest().getUri());
		currentClient.getRequest().setPath("./cgi/upload.php");
		std::cerr << "SecondPath:" << currentClient.getRequest().getSecondPath() << std::endl;
		std::cerr << "Path:"  << currentClient.getRequest().getPath() << std::endl;
		handleCgiRequest(currentClient, envp);
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
		std::cerr << "Erreur: impossible de créer le fichier: " << filePath << std::endl;
		throw HttpStatus(500);
	}
	outputFile << body;
	outputFile.close();
	std::cout << "Fichier créé avec succès: " << filePath << std::endl;
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = "text/plain";
	currentClient.generateResponse(201, headers, "Ressource créée avec succès.");
}

void	getFromDir(Client &currentClient, std::string path) {

	std::map<std::string, std::string>	headers;
	std::string							body;
	std::string							contentType;
	std::string							indexPath;
	struct stat							info;

	if (!path.empty() && path[path.size() - 1] != '/') {
		headers["Location"] = currentClient.getRequest().getUri() + "/";
		currentClient.generateResponse(308, headers, "");
		return;
	}
	indexPath = path + currentClient.getDefaultFile();
	if (stat(indexPath.c_str(), &info) == -1 || fileToString(indexPath, body) == false) {
		if (currentClient.getAutoIndex() == false) {
			throw HttpStatus(403);
		} else {
			body = generateAutoIndex(path);
			headers["Content-Type"] = "text/html";
		}
	} else {
		contentType = guessContentType(indexPath);
		headers["Content-Type"] = contentType;
	}
	currentClient.generateResponse(200, headers, body);
}


void	getFromFile(Client &currentClient, std::string path) {

	std::map<std::string, std::string>	headers;
	std::string							contentType;
	std::string							body;

	contentType = guessContentType(path);
	if (fileToString(path, body) == false)
		throw HttpStatus(404);
	headers["Content-Type"] = contentType;
	currentClient.generateResponse(200, headers, body);
}

void	handleGetRequest(Client &currentClient) {

	std::string	path;
	struct stat	info;

	path = currentClient.getRequest().getPath();
	std::cout << "path: " << path << std::endl;
	if (stat(path.c_str(), &info) == -1)
		throw HttpStatus(404);
	if (S_ISDIR(info.st_mode)) {
		getFromDir(currentClient, path);
	} else if (S_ISREG(info.st_mode)) {
		getFromFile(currentClient, path);
	} else
		throw HttpStatus(404);
}
