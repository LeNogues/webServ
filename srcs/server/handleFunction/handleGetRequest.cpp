#include "../../../includes/server/WebServer.hpp"

static void	getFromDir(Client &currentClient, std::string path)
{
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


static void	getFromFile(Client &currentClient, std::string path)
{
	std::map<std::string, std::string>	headers;
	std::string							body;

	if (fileToString(path, body) == false)
		throw HttpStatus(404);
	headers["Content-Type"] = guessContentType(path);
	currentClient.generateResponse(200, headers, body);
}

void	handleGetRequest(Client &currentClient)
{
	std::string	path;
	struct stat	info;

	path = currentClient.getRequest().getPath();
	if (stat(path.c_str(), &info) == -1)
		throw HttpStatus(404);
	if (access(path.c_str(), R_OK) == -1)
		throw HttpStatus(403);
	if (S_ISDIR(info.st_mode)) {
		getFromDir(currentClient, path);
	} else if (S_ISREG(info.st_mode)) {
		getFromFile(currentClient, path);
	} else
		throw HttpStatus(404);
}
