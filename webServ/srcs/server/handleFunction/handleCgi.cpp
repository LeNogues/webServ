#include "../../../includes/server/handleCgi.hpp"
#include "handleCgi.hpp"

static char **createEnv(Client& currentClient, std::vector<std::string> env)
{

	std::cout << "SERVER_PORT" << currentClient.getConfig()._listenOn.second << std::endl;
	std::cout << "SERVER_ADDR" << currentClient.getConfig()._listenOn.first << std::endl;

	env.push_back("AUTH_TYPE=" + currentClient.getRequest().getHeader("Authorization"));
	env.push_back("CONTENT_LENGTH=" + currentClient.getRequest().getHeader("Content-Length"));
	env.push_back("CONTENT_TYPE=" + currentClient.getRequest().getHeader("Content-Type"));
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("DOCUMENT_ROOT=" + currentClient.getRoot());
	// env.push_back("PATH_INFO=" + currentClient.getRequest().getPath());
	// env.push_back("PATH_TRANSLATED=" + currentClient.getRequest().getPath());
	env.push_back("QUERY_STRING=" + currentClient.getRequest().getQuery());
	// env.push_back("REMOTE_ADDR=" + currentClient.getRequest().getHeader("Remote-Addr"));
	// env.push_back("REMOTE_HOST=" + currentClient.getRequest().getHeader("Remote-Host"));
	// env.push_back("REMOTE_IDENT=" + currentClient.getRequest().getHeader("Remote-Ident"));
	// env.push_back("REMOTE_USER=" + currentClient.getRequest().getHeader("Remote-User"));
	env.push_back("REQUEST_METHOD=" + currentClient.getRequest().getMethod());
	// env.push_back("SCRIPT_NAME=" + currentClient.getRequest().getPath());
	env.push_back("SERVER_NAME=" + currentClient.getConfig()._serverName[0]);
	// env.push_back("SERVER_PORT=" + intToString(currentClient.getConfig()._listenOn.second));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");
	env.push_back("REQUEST_URI=" + currentClient.getRequest().getUri());

	char **envp = new char*[env.size() + 1];
	size_t i = 0;
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
	{
		// std::cout << "env: " << it->c_str() << std::endl;
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

void CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env, Request &request, const char *test, int pipeOut[2], int pipeIn[2])
{
    char **envp = createEnv(currentClient, env);
    std::string path_str = request.getPath();
    const char *path = path_str.c_str();
    const char *argv[] = {test, NULL};

    dup2(pipeOut[1], STDOUT_FILENO);
    dup2(pipeIn[0], STDIN_FILENO);
    closeAllFd(pipeOut, pipeIn);

    execve(path, const_cast<char *const *>(argv), envp);

    perror("execve");
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

void closeAndThrow(int pipeOut[2], int pipeIn[2])
{
    closeAllFd(pipeOut, pipeIn);
    throw HttpStatus(500);
}

void handleCgiRequest(Client& currentClient, std::vector<std::string> env)
{
	Request request = currentClient.getRequest();
	std::string cgiResponse;
	std::string test1 = request.getBody();
	const char *test = test1.c_str();

	int pipeOut[2];
	int pipeIn[2];

	if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1)
		throw HttpStatus(500);

	pid_t pid = fork();

	if (pid == -1)
        closeAndThrow(pipeOut, pipeIn);

	if (pid == 0)
        CreateEnvAndExecute(currentClient, env, request, test, pipeOut, pipeIn);
	else
	{
		close(pipeOut[1]);
		close(pipeIn[0]);

		std::string body = request.getBody();
		if (!body.empty())
			write(pipeIn[1], body.c_str(), body.length());
		close(pipeIn[1]);

        createResponse(pipeOut, cgiResponse);

        close(pipeOut[0]);
		waitpid(pid, NULL, 0);

        CreateHeaderAndGenerate(cgiResponse, currentClient);
    }
    return ;
}




// localhost:8080/test.cgi
