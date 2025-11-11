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

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerConfig.hpp"
# include "../utils/trim.hpp"

# include <vector>
# include <fstream>
# include <sstream>
# include <iostream>

class Config
{
	private:
		std::vector<ServerConfig>	_servers;
	public:
		Config(const std::string& configFilePath);
		~Config();
		Config(const Config& other);
		Config& operator=(const Config& other);
		void								parseConfig(const std::string& configFilePath);
		std::vector<ServerConfig>&			getServers();
		const std::vector<ServerConfig>&	getServers() const;
		std::string							getDefaultFile() const;
};

#endif
