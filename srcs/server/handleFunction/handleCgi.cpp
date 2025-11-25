#include "../../../includes/server/WebServer.hpp"

static void	closePair(int pipeFd[2])
{
	if (pipeFd[0] != -1)
		close(pipeFd[0]);
	if (pipeFd[1] != -1)
		close(pipeFd[1]);
}

static void	closePipe(int pipe1[2], int pipe2[2])
{
	closePair(pipe1);
	closePair(pipe2);
}

static std::string	generatePath(Client& currentClient, std::string path)
{
	LocationConfig	location;
	size_t			pos;

	location = currentClient.getRequest().getLocation();
	if (!location._alias.second.empty())
	{
		pos = path.find(location._alias.first);
		if (pos != std::string::npos)
			return (location._alias.second + path.substr(pos + location._alias.first.size()));
		return (location._alias.second + path);
	}
	return (location._root + path);
}

static char	**createEnv(Client& currentClient, std::vector<std::string> env)
{
	char	**envp;
	size_t	envSize;
	size_t	i;

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
	envSize = env.size();
	envp = new char*[envSize + 1];
	i = 0;
	while (i < envSize)
	{
		envp[i] = new char[env[i].size() + 1];
		std::strcpy(envp[i], env[i].c_str());
		i++;
	}
	envp[i] = NULL;
	return (envp);
}

void	WebServer::CreateEnvAndExecute(Client &currentClient, std::vector<std::string> &env)
{
	std::string	scriptPath;
	const char	*pathToScript;
	const char	*scriptArgv[2];
	const char	*phpArgv[3];
	char		**envp;
	size_t		i;

	scriptPath = currentClient.getRequest().getPath();
	pathToScript = scriptPath.c_str();
	envp = createEnv(currentClient, env);
	cleanup();
	if (scriptPath.find(".php") != std::string::npos)
	{
		phpArgv[0] = "/usr/bin/php-cgi";
		phpArgv[1] = pathToScript;
		phpArgv[2] = NULL;
		execve("/usr/bin/php-cgi", const_cast<char *const *>(phpArgv), envp);
	}
	else
	{
		scriptArgv[0] = pathToScript;
		scriptArgv[1] = NULL;
		execve(pathToScript, const_cast<char *const *>(scriptArgv), envp);
	}
	i = 0;
	while (envp[i] != NULL)
	{
		delete[] envp[i];
		i++;
	}
	delete[] envp;
	throw std::exception();
}

static void	setNonBlockingFd(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void	WebServer::handleCgiRequest(Client& currentClient)
{
	std::string	requestBody = currentClient.getRequest().getBody();
	int			pipeOut[2] = {-1, -1};
	int			pipeIn[2] = {-1, -1};
	int			clientFd;
	bool		hasBody;
	CgiHandler	handler;
	pid_t		pid;

	if (pipe(pipeOut) == -1 || pipe(pipeIn) == -1)
	{
		closePipe(pipeOut, pipeIn);
		throw HttpStatus(500);
	}
	pid = fork();
	if (pid == -1)
	{
		closePipe(pipeOut, pipeIn);
		throw HttpStatus(500);
	}
	if (pid == 0)
	{
		if (dup2(pipeIn[0], STDIN_FILENO) == -1 || dup2(pipeOut[1], STDOUT_FILENO) == -1)
		{
			closePipe(pipeOut, pipeIn);
			cleanup();
			exit(EXIT_FAILURE);
		}
		closePipe(pipeOut, pipeIn);
		CreateEnvAndExecute(currentClient, _envp);
		cleanup();
		exit(EXIT_FAILURE);
	}
	close(pipeOut[1]);
	close(pipeIn[0]);
	setNonBlockingFd(pipeOut[0]);
	hasBody = !requestBody.empty();
	if (hasBody)
		setNonBlockingFd(pipeIn[1]);
	else
		close(pipeIn[1]);
	clientFd = currentClient._clientFd;
	handler.pid = pid;
	handler.pipeReadFd = pipeOut[0];
	handler.pipeWriteFd = hasBody ? pipeIn[1] : -1;
	handler.bytesWritten = 0;
	handler.startTime = std::time(NULL);
	_pipeToClient[pipeOut[0]] = clientFd;
	addFdToEpoll(pipeOut[0], EPOLLIN);
	if (hasBody)
	{
		_pipeToClient[pipeIn[1]] = clientFd;
		addFdToEpoll(pipeIn[1], EPOLLOUT);
	}
	_clientToCgi[clientFd] = handler;
}
