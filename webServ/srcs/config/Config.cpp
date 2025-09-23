#include "../../includes/config/Config.hpp"

Config::Config(const std::string& ConfigFilePath)
{
	this->parseConfig(ConfigFilePath);
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


std::vector<ServerConfig>& Config::getServers()
{
	return this->_servers;
}


static std::vector<std::string> tokenize(const std::string& line)
{
	std::vector<std::string> tokens;
	std::stringstream ss(line);
	std::string token;

	while (ss >> token)
		tokens.push_back(token);
	return tokens;
}

static LocationConfig parseLocationBlock(std::ifstream& configFile)
{
	LocationConfig			locationConfig;

	// TODO: Implement location block parsing
	(void)configFile; // Suppress unused parameter warning
	return (locationConfig);
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
		if (line == "" || line[0] == '#')
			continue ;
		tokens = tokenize(line);
		if (tokens[0] == "}")
			return (newServerConfig);
		else if (tokens[0] == "location")
		{
			path = tokens[1];
			if (tokens[2] != "{")
				throw std::runtime_error("ERROR: 'location must be follow by a path and {'");
			LocationConfig newLocation = parseLocationBlock(configFile);
			newServerConfig._location.insert(std::make_pair(path, newLocation));
		}
		else
		{
		}
	}
	return (newServerConfig);
}

void Config::parseConfig(const std::string& configFilePath)
{
	std::ifstream			configFile(configFilePath.c_str());
	if (!configFile)
		throw std::runtime_error("ERROR: can't open config file");
	std::string line;
	std::vector<std::string> tokens;

	while(std::getline(configFile, line))
	{
		btrim(line);
		if (line == "" || line[0] == '#')
			continue ;
		tokens = tokenize(line);
		if (tokens[0] == "server")
		{
			if (tokens[1] != "{")
				throw std::runtime_error("SYNTAX ERROR: " + tokens[0] + " need a {");
			ServerConfig newServer = parseServerBlock(configFile);
			this->_servers.push_back(newServer);
		}
		else
			throw std::runtime_error("SYNTAX ERROR: " + tokens[0] + ", need: server {");
	}
	if (this->_servers.empty())
		throw std::runtime_error("ERROR: config file does not contain any server block");
}
