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

# include "../utils/trim.hpp"

# include <map>
# include <list>
# include <vector>
# include <fstream>
# include <sstream>
# include <iostream>
# include "ServerConfig.hpp"

class Config
{
	private:
			std::vector<ServerConfig>	_servers;
	public:
			void								parseConfig(const std::string& configFilePath);
			std::vector<ServerConfig>& 			getServers();
			const std::vector<ServerConfig>& 	getServers() const;

			Config(const std::string& configFilePath);
			~Config();
			Config(const Config& other);
			Config& operator=(const Config& other);
};

#endif
