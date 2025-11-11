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

#include "../includes/config/Config.hpp"
#include "../includes/utils/writeError.hpp"
#include "../includes/server/WebServer.hpp"

#include <iostream>
#include <vector>
#include <exception>
#include <csignal>

void	signalFunction(int sig)
{
	(void)sig;
	throw WebServer::signalException();
}

int	main(int argc, char **argv, char **envp)
{
	signal(SIGINT, signalFunction);
	signal(SIGQUIT, signalFunction);
	try {
		if (argc != 2)
			throw std::runtime_error("ERROR: wrong number of arguments");
		Config configs(argv[1]);
		const std::vector<ServerConfig>& serversConfigs = configs.getServers();
			// 2.Create one WebServer object that manages all the servers
			WebServer webServer(serversConfigs, envp);
			// 3. Initialize the listening sockets
			webServer.init();
			// 4. Launch the main event loop
			webServer.run();
	} catch(const WebServer::signalException& e) {
			std::cerr << e.what() << '\n';
			return 1;
	} catch(const std::exception& e)
	{
			std::cerr << e.what() << '\n';
			return 1;
	}
	return 0;
}
