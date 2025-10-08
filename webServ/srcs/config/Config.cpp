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

#include "Config.hpp"

static LocationConfig parseLocationBlock(std::ifstream& configFile, const CommonConfig& parentConfig);

#pragma region Cannonical Class

Config::Config(const std::string& ConfigFilePath)
{
    this->parseConfig(ConfigFilePath);
    for (std::vector<ServerConfig>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it)
    {
        ServerConfig& server = *it;
        if (server._root.empty())
            server._root = "/var/www/html";
        if (server._defaultFile.empty())
            server._defaultFile = "index.html";
        if (server._allowedMethods.empty())
        {
            server._allowedMethods.push_back("GET");
            server._allowedMethods.push_back("HEAD");
        }
        if (server._listenOn.first == "")
            server._listenOn = std::make_pair("0.0.0.0", 80);
    }
}
Config::~Config()
{
}

Config::Config(const Config& other)
{
	this->_servers = other._servers;
}

Config& Config::operator=(const Config& other)
{
	if (this != &other)
	{
		this->_servers = other._servers;
	}
	return (*this);
}


#pragma region Getter/setter

std::vector<ServerConfig>& Config::getServers() { return this->_servers; }
const std::vector<ServerConfig>& Config::getServers() const { return this->_servers; }

#pragma endregion


static std::vector<std::string> tokenize(const std::string& line)
{
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;

	while (ss >> token)
		tokens.push_back(token);
	return tokens;
}

static bool addCommonToken(std::vector<std::string>& tokens, CommonConfig& newConfig);

#pragma region addToken

static void addListen(std::vector<std::string>& tokens, ServerConfig& newServerConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'listen' directive needs one argument.");
    if (newServerConfig._listenOn.first != "")
        throw std::runtime_error("ERROR: multiple listen");

    std::string listenAddress = tokens[1];
    size_t colon_pos = listenAddress.find(':');
    std::string host = "0.0.0.0";
    int port = 80;
    std::stringstream ss;

    if (colon_pos != std::string::npos)
    {
        host = listenAddress.substr(0, colon_pos);
        ss.str(listenAddress.substr(colon_pos + 1));
    }
    else
        ss.str(listenAddress);
    ss >> port;
    if (ss.fail() || !ss.eof())
        throw std::runtime_error("ERROR: invalid port number in listen directive.");

    newServerConfig._listenOn = std::make_pair(host, port);
}

static void addServerName(std::vector<std::string>& tokens, ServerConfig& newServerConfig)
{
    if (tokens.size() < 2)
        throw std::runtime_error("ERROR: 'server_name' needs at least one argument.");
    for (size_t i = 1; i < tokens.size(); ++i)
        newServerConfig._serverName.push_back(tokens[i]);
}

static void addErrorPage(std::vector<std::string>& tokens, ServerConfig& newServerConfig)
{
    if (tokens.size() != 3)
        throw std::runtime_error("ERROR: 'error_page' directive needs two arguments (code and path).");

    int errorCode;
    std::stringstream ss(tokens[1]);
    ss >> errorCode;
    if (ss.fail() || !ss.eof())
        throw std::runtime_error("ERROR: invalid error code in error_page directive.");

    newServerConfig._errorPage.insert(std::make_pair(errorCode, tokens[2]));
}

static void addMaxSize(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'client_body_buffer_size' needs one argument.");

    unsigned long maxSize;
    std::stringstream ss(tokens[1]);
    ss >> maxSize;
    if (ss.fail() || !ss.eof())
        throw std::runtime_error("ERROR: invalid number for client_body_buffer_size.");

    newConfig._maxSizeBody = maxSize;
}

static void addRoot(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'root' directive needs one argument.");
    newConfig._root = tokens[1];
}

static void addAutoIndex(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'autoindex' directive needs one argument ('on' or 'off').");
    if (tokens[1] == "on")
        newConfig._autoIndex = true;
    else if (tokens[1] == "off")
        newConfig._autoIndex = false;
    else
        throw std::runtime_error("ERROR: 'autoindex' argument must be 'on' or 'off'.");
}

static void addDefaultFile(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'index' directive needs one argument.");
    newConfig._defaultFile = tokens[1];
}

static void addAllowedMethods(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() < 2)
        throw std::runtime_error("ERROR: 'allow_methods' directive needs at least one argument.");
    newConfig._allowedMethods.clear();
    for (size_t i = 1; i < tokens.size(); ++i)
        newConfig._allowedMethods.push_back(tokens[i]);
}

static void addCgiParam(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens.size() != 3)
        throw std::runtime_error("ERROR: 'cgi_param' directive needs two arguments (key and value).");
    newConfig._cgiParams.insert(std::make_pair(tokens[1], tokens[2]));
}


static void addCgiPass(std::vector<std::string>& tokens, LocationConfig& newLocationConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'cgi_pass' directive needs one argument (path to executable).");
    newLocationConfig._cgiPass = tokens[1];
}

static void addRedirect(std::vector<std::string>& tokens, LocationConfig& newLocationConfig)
{
    if (tokens.size() != 3)
        throw std::runtime_error("ERROR: 'return' directive needs two arguments (code and URL).");

    int code;
    std::stringstream ss(tokens[1]);
    ss >> code;
    if (ss.fail() || !ss.eof())
        throw std::runtime_error("ERROR: invalid code for return directive.");

    newLocationConfig._redirect = std::make_pair(code, tokens[2]);
}

static void addUploadStore(std::vector<std::string>& tokens, LocationConfig& newLocationConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'upload_store' directive needs one argument.");
    newLocationConfig._uploadStore = tokens[1];
}

static bool addCommonToken(std::vector<std::string>& tokens, CommonConfig& newConfig)
{
    if (tokens[0] == "root")
        addRoot(tokens, newConfig);
    else if (tokens[0] == "autoindex")
        addAutoIndex(tokens, newConfig);
    else if (tokens[0] == "index")
        addDefaultFile(tokens, newConfig);
    else if (tokens[0] == "client_body_buffer_size")
        addMaxSize(tokens, newConfig);
    else if (tokens[0] == "allow_methods")
        addAllowedMethods(tokens, newConfig);
    else if (tokens[0] == "cgi_param")
        addCgiParam(tokens, newConfig);
    else
        return (false);
    return (true);
}

static void addServerToken(std::vector<std::string>& tokens, ServerConfig& newServerConfig)
{
    if (addCommonToken(tokens, newServerConfig))
        return;
    else if (tokens[0] == "listen")
        addListen(tokens, newServerConfig);
    else if (tokens[0] == "server_name")
        addServerName(tokens, newServerConfig);
    else if (tokens[0] == "error_page")
        addErrorPage(tokens, newServerConfig);
    else
        throw std::runtime_error("SYNTAX ERROR: " + tokens[0] + " is not a recognized directive in a server block.");
}

static void addAlias(std::vector<std::string>& tokens, LocationConfig& newLocationConfig)
{
    if (tokens.size() != 2)
        throw std::runtime_error("ERROR: 'alias' directive needs one argument (a path).");
    newLocationConfig._alias = tokens[1];
}

static void addLocationToken(std::vector<std::string>& tokens, LocationConfig& newLocationConfig)
{
    if (addCommonToken(tokens, newLocationConfig))
        return;
    else if (tokens[0] == "return")
        addRedirect(tokens, newLocationConfig);
    else if (tokens[0] == "upload_store")
        addUploadStore(tokens, newLocationConfig);
    else if (tokens[0] == "cgi_pass")
        addCgiPass(tokens, newLocationConfig);
    else if (tokens[0] == "alias")
        addAlias(tokens, newLocationConfig);
    else
        throw std::runtime_error("SYNTAX ERROR: '" + tokens[0] + "' is not a recognized directive in a location block.");
}

#pragma endregion

static LocationConfig parseLocationBlock(std::ifstream& configFile, const CommonConfig& parentConfig)
{
    LocationConfig newLocationConfig;
    static_cast<CommonConfig&>(newLocationConfig) = parentConfig;

    std::string line;
    std::vector<std::string> tokens;
    std::string path;

    while(std::getline(configFile, line))
    {
        btrim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        tokens = tokenize(line);
        if (tokens.empty())
            continue;
        if (tokens[0] == "}")
            return (newLocationConfig);
        else if (tokens[0] == "location")
        {
            if (tokens.size() < 3 || tokens.back() != "{")
                throw std::runtime_error("ERROR: 'location' must be followed by a path and '{'");
            path = tokens[1];
            LocationConfig nestedLocation = parseLocationBlock(configFile, newLocationConfig);
            newLocationConfig._locations.insert(std::make_pair(path, nestedLocation));
        }
        else
            addLocationToken(tokens, newLocationConfig);
    }
    throw std::runtime_error("ERROR: location block needs a '}' at the end");
}

static ServerConfig parseServerBlock(std::ifstream& configFile)
{
	ServerConfig newServerConfig;
	std::string line;
	std::vector<std::string> tokens;
	std::string path;

    while(std::getline(configFile, line))
    {
        btrim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        tokens = tokenize(line);
        if (tokens.empty())
            continue;
        if (tokens[0] == "}")
            return (newServerConfig);
        else if (tokens[0] == "location")
        {
            if (tokens.size() < 3 || tokens.back() != "{")
                throw std::runtime_error("ERROR: 'location' must be followed by a path and '{'");
            path = tokens[1];
            LocationConfig newLocation = parseLocationBlock(configFile, newServerConfig);
            newServerConfig._location.insert(std::make_pair(path, newLocation));
        }
        else
            addServerToken(tokens, newServerConfig);
    }
    throw std::runtime_error("ERROR: server block needs a '}' at the end");
}

void Config::parseConfig(const std::string& configFilePath)
{
    std::ifstream configFile(configFilePath.c_str());
    if (!configFile)
        throw std::runtime_error("ERROR: can't open config file");
    std::string line;
    std::vector<std::string> tokens;

    while(std::getline(configFile, line))
    {
        btrim(line);
        if (line.empty() || line[0] == '#')
            continue ;
        tokens = tokenize(line);
        if (tokens.empty())
            continue;
        if (tokens[0] == "server")
        {
            if (tokens.size() != 2 || tokens[1] != "{")
                throw std::runtime_error("SYNTAX ERROR: server block must start with 'server {'");
            ServerConfig newServer = parseServerBlock(configFile);
            this->_servers.push_back(newServer);
        }
        else
            throw std::runtime_error("SYNTAX ERROR: unexpected token '" + tokens[0] + "', expected 'server'");
    }
    if (this->_servers.empty())
        throw std::runtime_error("ERROR: config file does not contain any server block");
}
