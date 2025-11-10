#include "../../../includes/server/handleCgi.hpp"
#include "../../../includes/server/WebServer.hpp"

static std::string generatePath(Client& currentClient, std::string path)
{
    LocationConfig location = currentClient.getRequest().getLocation();
    if (!location._alias.second.empty())
    {
        size_t pos = path.find(location._alias.first);
        if (pos != std::string::npos)
            return (location._alias.second + path.substr(pos + location._alias.first.size()));
        return (location._alias.second + path);
    }
    return (location._root + path);
}

static char **createEnv(Client& currentClient, std::vector<std::string> env)
{
	env.push_back("AUTH_TYPE=" + currentClient.getRequest().getHeader("Authorization"));
	env.push_back("CONTENT_LENGTH=" + currentClient.getRequest().getHeader("Content-Length"));
	env.push_back("CONTENT_TYPE=" + currentClient.getRequest().getHeader("Content-Type"));
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("DOCUMENT_ROOT=" + currentClient.getRoot());
	env.push_back("PATH_INFO=" + currentClient.getRequest().getSecondPath());
	env.push_back("PATH_TRANSLATED=" + generatePath(currentClient, currentClient.getRequest().getSecondPath()));
	env.push_back("QUERY_STRING=" + currentClient.getRequest().getQuery());
	env.push_back("REQUEST_METHOD=" + currentClient.getRequest().getMethod());
	env.push_back("SCRIPT_NAME=" + currentClient.getRequest().getPath());
	env.push_back("SERVER_NAME=" + currentClient.getConfig()._serverName[0]);
	env.push_back("SERVER_PORT=" + intToString(currentClient.getConfig()._listenOn.second));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");
	env.push_back("REQUEST_URI=" + currentClient.getRequest().getUri());
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("SCRIPT_FILENAME=" + currentClient.getRequest().getPath());

	char **envp = new char*[env.size() + 1];
	size_t i = 0;
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
	{
		envp[i] = new char[it->size() + 1];
		std::strcpy(envp[i], it->c_str());
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void closeAllFd(int pipeOut[2], int pipeIn[2])
{
	close(pipeOut[0]);
	close(pipeOut[1]);
	close(pipeIn[0]);
	close(pipeIn[1]);
}

void createHeaders(std::string line, std::map<std::string, std::string>& headers)
{
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
	{
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        if (!key.empty() && !value.empty())
            headers[key] = value;
    }
}

void CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env, Request &request, const char *php_interpreter_path, int pipeOut[2], int pipeIn[2])
{
    (void)currentClient;
    (void)env;

    const char *path_to_php = php_interpreter_path;

    std::string script_path_str = request.getPath();
    const char *path_to_script = script_path_str.c_str();

    const char *argv[] = {path_to_php, path_to_script, NULL};

    char **envp = createEnv(currentClient, env);

    dup2(pipeOut[1], STDOUT_FILENO);
    dup2(pipeIn[0], STDIN_FILENO);
    closeAllFd(pipeOut, pipeIn);

    std::cerr << "Avant execve. Commande: " << path_to_php << " " << path_to_script << std::endl;

    execve(path_to_php, const_cast<char *const *>(argv), envp);

    std::cerr << "APRES EXECVE - CELA SIGNIFIE UNE ERREUR !" << std::endl;
    perror("Erreur de execve");

    for (size_t i = 0; envp[i] != NULL; ++i)
        delete[] envp[i];
    delete[] envp;

    throw std::exception();
}

void createResponse(int pipeOut[2], std::string &cgiResponse)
{
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(pipeOut[0], buffer, sizeof(buffer))) > 0)
        cgiResponse.append(buffer, bytes_read);
}

void CreateHeaderAndGenerate(std::string &cgiResponse, Client &currentClient)
{
    std::map<std::string, std::string> headers;
    std::string responseBody;
    size_t separatorPos = cgiResponse.find("\r\n\r\n");
    if (separatorPos == std::string::npos)
        separatorPos = cgiResponse.find("\n\n");
    if (separatorPos != std::string::npos)
    {
        std::string headerBlock = cgiResponse.substr(0, separatorPos);
        responseBody = cgiResponse.substr(separatorPos + (cgiResponse.find("\r\n\r\n") != std::string::npos ? 4 : 2));

        std::stringstream ss(headerBlock);
        std::string line;
        while (std::getline(ss, line))
            createHeaders(line, headers);
    }
    else
        responseBody = cgiResponse;

    currentClient.generateResponse(200, headers, responseBody);
}

void WebServer::addFdToEpoll(int fd, uint32_t events) {
    epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, fd, &event) == -1) {
        throw std::runtime_error("Erreur: epoll_ctl a échoué pour ajouter un fd");
    }
}

void closeAndThrow(int pipe1[2], int pipe2[2]) {
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    throw HttpStatus(500);
}


void WebServer::handleCgiRequest(Client& currentClient, std::vector<std::string> env)
{
    Request request = currentClient.getRequest();
    std::string requestBody = request.getBody();
    const char* bodyCStr = requestBody.c_str();

    int pipeOut[2];
    int pipeIn[2];

    if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1)
        throw HttpStatus(500);

    pid_t pid = fork();

    if (pid == -1)
        closeAndThrow(pipeOut, pipeIn);

    if (pid == 0)
    {
        close(pipeOut[0]);
        close(pipeIn[1]);
        dup2(pipeIn[0], STDIN_FILENO);
        dup2(pipeOut[1], STDOUT_FILENO);
        close(pipeIn[0]);
        close(pipeOut[1]);

        (void)bodyCStr;
        CreateEnvAndExecute(currentClient, env, request, "/usr/bin/php-cgi", pipeOut, pipeIn);
        exit(EXIT_FAILURE);
    }
    else
    {
        close(pipeOut[1]);
        close(pipeIn[0]);

        fcntl(pipeOut[0], F_SETFL, O_NONBLOCK);
        fcntl(pipeIn[1], F_SETFL, O_NONBLOCK);

        _pipeToClient[pipeOut[0]] = currentClient._clientFd;
        _pipeToClient[pipeIn[1]] = currentClient._clientFd;

        addFdToEpoll(pipeOut[0], EPOLLIN);

        if (!requestBody.empty()) {
            addFdToEpoll(pipeIn[1], EPOLLOUT);
        } else {
            close(pipeIn[1]);
        }
    }
}
