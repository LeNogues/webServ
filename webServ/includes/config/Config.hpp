#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <map>
# include <list>
# include <vector>
# include <fstream>
# include <sstream>
# include "ServeurConfig.hpp"
# include "../trim/trim.hpp"

class Config
{
	private:
			std::vector<ServerConfig>	_servers;
	public:
			Config(const std::string& configFilePath);
			~Config();
			Config(const Config& other);
			Config& operator=(const Config& other);

			std::vector<ServerConfig>& getServers();
			void	parseConfig(const std::string& configFilePath);
};

#endif
