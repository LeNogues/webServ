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

#include "../../includes/server/WebServer.hpp"

WebServer::WebServer(const std::vector<ServerConfig>& configs)
    : _servers(configs)
{
}

WebServer::~WebServer()
{
}

static void errorInit(const std::string& Error, const std::string& value, const int& serverFd)
{
    close(serverFd);
    throw std::runtime_error(Error + value);
}


static void setNonBlocking(int serverFD)
{
    int flags = fcntl(serverFD, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("ERROR: fcntl failed");

    flags |= O_NONBLOCK;
    int s = fcntl(serverFD, F_SETFL, flags);
    if (s == -1)
        throw std::runtime_error("ERROR: fcntl failed");
}

void WebServer::setServerAdress(const int& serverFd, sockaddr_in& serverAdress, size_t i)
{
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(_servers[i]._listenOn.second);
    serverAdress.sin_addr.s_addr = inet_addr(_servers[i]._listenOn.first.c_str());
    if (serverAdress.sin_addr.s_addr == INADDR_NONE)
        errorInit("ERROR: Adresse IP invalide: ", _servers[i]._listenOn.first, serverFd);
}


void WebServer::handleNewConnection(int currentFd, const ServerConfig& config)
{

    std::cout << "\n------------------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "during NewConnection " << config._serverName[0] << std::endl;
    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int clientFd = accept(currentFd, (struct sockaddr*)&clientAddress, &clientAddrLen);

        if (clientFd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                std::cerr << "Erreur lors de accept()" << std::endl;
                break;
            }
        }

        setNonBlocking(clientFd);

        struct epoll_event event = {};
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
        event.data.fd = clientFd;
        if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, clientFd, &event) == -1)
        {
            std::cerr << "Erreur: epoll_ctl ne peut pas ajouter le client fd" << std::endl;
            close(clientFd);
            continue;
        }

        _clients.insert(std::make_pair(clientFd, Client(clientFd, config)));
        std::cout << "Nouvelle connexion acceptée sur le fd: " << clientFd << std::endl;
    }
}

void WebServer::handleClientDisconnection(int currentFd)
{
    if (epoll_ctl(_epollFD, EPOLL_CTL_DEL, currentFd, NULL) == -1)
        std::cerr << "Warning: epoll-ctl(DEL) failed for fd : " << currentFd << std::endl;
    close(currentFd);
    size_t erased_count = _clients.erase(currentFd);
    if (erased_count > 0)
    {
        std::cout << "Client on fd " << currentFd << " disconnected and cleaned up." << std::endl;
        std::cout << "------------------------------------------------------------------------------------------------------------------------------\n" << std::endl;
    }
    else
        std::cerr << "Warning: tried to erase non-existant client for fd :" << currentFd << std::endl;
}

void WebServer::handleClientWrite(int currentFd)
{
    std::map<int, Client>::iterator it = _clients.find(currentFd);
    if (it == _clients.end())
    {
        std::cerr << "Error: write for unknown client fd: " << currentFd << std::endl;
        handleClientDisconnection(currentFd);
        return;
    }

    ssize_t sent = it->second.sendPending();
    if (sent == -1)
    {
        std::cerr << "Error: send() failed for fd: " << currentFd << std::endl;
        handleClientDisconnection(currentFd);
        return;
    }
    if (it->second.hasResponse())
        return;

    handleClientDisconnection(currentFd);
}

void WebServer::switchToRead(int clientFd)
{
    epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLIN;

    if (epoll_ctl(_epollFD, EPOLL_CTL_MOD, clientFd, &event) == -1)
    {
        std::cerr << "ERROR: can't change epoll for write request on fd : " << clientFd << std::endl;
        handleClientDisconnection(clientFd);
    }
}

void WebServer::switchToWrite(int clientFd)
{
    epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLOUT;

    if (epoll_ctl(_epollFD, EPOLL_CTL_MOD, clientFd, &event) == -1)
    {
        std::cerr << "ERROR: can't change epoll for write request on fd : " << clientFd << std::endl;
        handleClientDisconnection(clientFd);
    }
}

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

static std::string getMimeType(const std::string& filePath) {
    // Crée une map statique pour ne l'initialiser qu'une seule fois.
    static std::map<std::string, std::string> mimeTypes;
    if (mimeTypes.empty()) {
        mimeTypes[".html"] = "text/html";
        mimeTypes[".css"] = "text/css";
        mimeTypes[".js"] = "application/javascript";
        mimeTypes[".jpg"] = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".png"] = "image/png";
        mimeTypes[".gif"] = "image/gif";
        // Ajoutez d'autres types MIME si nécessaire
    }

    size_t dotPos = filePath.rfind('.');
    if (dotPos != std::string::npos) {
        std::string ext = filePath.substr(dotPos);
        if (mimeTypes.count(ext)) {
            return mimeTypes[ext];
        }
    }
    // Type par défaut si l'extension n'est pas reconnue
    return "application/octet-stream";
}

void handleGetRequest(Client& currentClient, std::string uri)
{
    if (uri == "/") {
        uri = "/index.html";
    }

    std::string filePath = currentClient.getRoot() + uri;

    struct stat buffer;
    if (stat(filePath.c_str(), &buffer) != 0) {
        std::cerr << "File not found: " << filePath << std::endl;
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "text/html";
        currentClient.generateResponse(404, headers, "<html><body><h1>404 Not Found</h1></body></html>");
        return;
    }

    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filePath << std::endl;
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "text/html";
        currentClient.generateResponse(500, headers, "<html><body><h1>500 Internal Server Error</h1></body></html>");
        return;
    }

    std::stringstream contentStream;
    contentStream << file.rdbuf();
    std::string fileContent = contentStream.str();
    file.close();

    std::map<std::string, std::string> headers;
     headers["Content-Type"] = getMimeType(filePath);
    std::stringstream ss;
    ss << fileContent.length();
    headers["Content-Length"] = ss.str();

    currentClient.generateResponse(200, headers, fileContent);
    
    std::cout << "Successfully served: " << filePath << std::endl;
}

void handlePostRequest(Client& currentClient, std::string uri)
{
    (void)currentClient;
    (void)uri;

    std::cout << "POST request received" << std::endl;
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
        currentClient.getRequest().logRequest();
        std::cout << "--- Request End ---" << std::endl;

        std::string method = currentClient.getRequest().getMethod();
        std::string uri = currentClient.getRequest().getPath();

        if (method == "DELETE")
            handleDeleteRequest(currentClient, uri);
        else if (method == "POST")
            handlePostRequest(currentClient, uri);
        else if (method == "GET")
            handleGetRequest(currentClient, uri);

        switchToWrite(currentFd);
    }
    catch(const HttpStatus& status)
    {
        std::cerr << status.what() << '\n';
        std::string									body = "";
        std::map<std::string, std::string>			headers;
        std::map<int, std::string>::const_iterator	itPage = currentClient.getConfig()._errorPage.find(status.getStatusCode());

        if (itPage != currentClient.getConfig()._errorPage.end())
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
}

void WebServer::run()
{
    epoll_event events[MAX_EVENTS];

    while(1)
    {
        int numEvent = epoll_wait(_epollFD, events, MAX_EVENTS, -1);
        if (numEvent == -1)
        {
            if (errno == EINTR)
                continue ;
            else
            {
                //pensez a fermer les fd ou faire une classe de catch special
                throw std::runtime_error("ERROR: erreur critique de epoll_wait");
            }
        }

        for (int i = 0; i < numEvent; ++i)
        {
            int currentFd = events[i].data.fd;
            if ( events[i].events & (EPOLLERR | EPOLLHUP))
            {
                handleClientDisconnection(currentFd);
                continue ;
            }
            //la ligne fait peur mais elle cree juste un iterateur, panique pas bebou
            std::map<int, const ServerConfig, std::less<int>, std::allocator<std::pair<const int, const ServerConfig> > >::iterator currentServer = _listeningSockets.find(currentFd);
            if (currentServer == _listeningSockets.end())
            {
                if (events[i].events & EPOLLIN)
                    handleClientRead(currentFd);
                else if (events[i].events & EPOLLOUT)
                    handleClientWrite(currentFd);
            }
            else
            {
                handleNewConnection(currentFd, currentServer->second);
            }
        }
    }
}


void WebServer::init()
{
    _epollFD = epoll_create(MAX_EVENTS);
    if (_epollFD == -1)
        throw std::runtime_error("ERROR:can't create epoll instance");

    for (size_t i = 0; i < _servers.size(); ++i)
    {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd == -1)
            throw std::runtime_error("ERROR: could not create socket for " + _servers[i]._serverName[0]);

        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            errorInit("ERROR: setsockopt failed for", _servers[i]._serverName[0], serverFd);

        struct sockaddr_in serverAdress;
        setServerAdress(serverFd, serverAdress, i);

        if (bind(serverFd, (struct sockaddr*)&serverAdress, sizeof(serverAdress)) < 0)
            errorInit("ERROR: failed to bind to port ", intToString(_servers[i]._listenOn.second), serverFd);

        setNonBlocking(serverFd);

        if (listen(serverFd, SOMAXCONN) < 0)
            errorInit("ERROR: listen failed for ", _servers[i]._serverName[0], serverFd);

        struct epoll_event event = {};
        event.events = EPOLLIN;
        event.data.fd = serverFd;
        if (epoll_ctl(_epollFD, EPOLL_CTL_ADD, serverFd, &event) == -1) {
            throw std::runtime_error("ERROR: epoll_ctl failed to add listening socket");
        }
        _listeningSockets.insert(std::make_pair(serverFd, _servers[i]));
    }
}

const char* WebServer::signalException::what() const throw()
{
    return ("\nServer interrupted by signal");
}
