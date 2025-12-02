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
#include "../includes/server/WebServer.hpp"

void	signalFunction(int sig)
{
	(void)sig;
	throw WebServer::signalException();
}

int	main(int argc, char **argv, char **envp)
{
	signal(SIGINT, signalFunction);
	signal(SIGQUIT, signalFunction);
	if (argc > 2)
		return (std::cerr << "ERROR: wrong number of arguments\n", 1);
	try
	{
		Config configs;
		if (argc == 2)
		{
			Config tmp(argv[1]);
			configs = tmp;
		}
			
		const std::vector<ServerConfig>& serversConfigs = configs.getServers();
		WebServer webServer(serversConfigs, envp);
		try {
			// 3. Initialize the listening sockets
			webServer.init();
			// 4. Launch the main event loop
			webServer.run();
			// 5. Cleanup resources
			webServer.cleanup();
		} catch(const WebServer::signalException& e) {
			webServer.cleanup();
			std::cerr << e.what() << '\n';
			return 1;
		} catch(const std::exception& e) {
			webServer.cleanup();
			std::cerr << e.what() << '\n';
			return 1;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
	

	return 0;
}
