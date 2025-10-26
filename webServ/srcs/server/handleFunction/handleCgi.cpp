#include "../../../includes/server/handleCgi.hpp"

static char **createEnv(Client& currentClient, std::vector<std::string> env)
{
	env.push_back("AUTH_TYPE=" + currentClient.getRequest().getHeader("Authorization"));
	env.push_back("CONTENT_LENGTH=" + currentClient.getRequest().getHeader("Content-Length"));
	env.push_back("CONTENT_TYPE=" + currentClient.getRequest().getHeader("Content-Type"));
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("DOCUMENT_ROOT=" + currentClient.getRoot());
	env.push_back("PATH_INFO=" + currentClient.getRequest().getSecondPath());
	env.push_back("PATH_TRANSLATED=" + currentClient.getRoot() + currentClient.getRequest().getSecondPath());
	env.push_back("QUERY_STRING=" + currentClient.getRequest().getQuery());
	// env.push_back("REMOTE_ADDR=" + currentClient.getRequest().getHeader("Remote-Addr"));
	// env.push_back("REMOTE_HOST=" + currentClient.getRequest().getHeader("Remote-Host"));
	// env.push_back("REMOTE_IDENT=" + currentClient.getRequest().getHeader("Remote-Ident"));
	// env.push_back("REMOTE_USER=" + currentClient.getRequest().getHeader("Remote-User"));
	env.push_back("REQUEST_METHOD=" + currentClient.getRequest().getMethod());
	env.push_back("SCRIPT_NAME=" + currentClient.getRequest().getPath());
	env.push_back("SERVER_NAME=" + currentClient.getConfig()._serverName[0]);
	env.push_back("SERVER_PORT=" + intToString(currentClient.getConfig()._listenOn.second));
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.0");
	env.push_back("REQUEST_URI=" + currentClient.getRequest().getUri());

	char **envp = new char*[env.size() + 1];
	size_t i = 0;
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++)
	{
		envp[i] = new char[it->size() + 1];
		std::strcpy(envp[i], it->c_str());
		i++;
	}
	return (envp);
}

void handleCgiRequest(Client& currentClient, std::vector<std::string> env)
{
	Request request = currentClient.getRequest();
	std::map<std::string, std::string> headers;
	std::string body;

	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw HttpStatus(500);

	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		throw HttpStatus(500);
	}
	if (pid == 0)
	{
		char **envp = createEnv(currentClient, env);

		close(pipefd[1]);
		dup2(pipefd[0], STDOUT_FILENO);
		close(pipefd[0]);
		delete[] envp;
		//execve(request.getPath().c_str(), NULL, envp);
		std::cout << "execve" << std::endl;
		exit(1);
	}
	else
	{
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		waitpid(pid, NULL, 0);
	}
}
