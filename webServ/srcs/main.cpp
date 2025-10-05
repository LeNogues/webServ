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
#include "../includes/request/Request.hpp"
#include "../includes/utils/writeError.hpp"
#include "../includes/server/Server.hpp"

#include <iostream>
#include <vector>
#include <exception>
#include <csignal>

// volatile sig_atomic_t g_running = 1;

// void signalHandler(int signum) {
//   (void)signum;
//   std::cout << "\nSignal d'arrêt reçu. Fermeture des serveurs..." << std::endl;
//   g_running = 0;
// }

// int main(int argc, char **argv) {
//   if (argc != 2) {
//     std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
//     return 1;
//   }

//   signal(SIGINT, signalHandler);

//   std::vector<Server *> serverInstances;
//   try {
//     Config config(argv[1]);
//     std::vector<ServerConfig> &servers = config.getServers();
//     if (servers.empty()) {
//       std::cerr << "Aucune configuration de serveur trouvée dans " << argv[1] << std::endl;
//       return 1;
//     }

//     for (std::vector<ServerConfig>::iterator it = servers.begin();
//          it != servers.end(); ++it) {
//       try {
//         serverInstances.push_back(new Server(*it));
//         if (!it->_serverName.empty()) {
//           std::cout << "Serveur " << it->_serverName[0] << " initialisé et à l'écoute sur le port " << it->_listenOn.second << std::endl;
//         } else {
//           std::cout << "Un serveur a été initialisé et est à l'écoute sur le port " << it->_listenOn.second << std::endl;
//         }
//       } catch (const std::runtime_error &e) {
//         std::cerr << "Erreur lors de l'initialisation du serveur : " << e.what() << std::endl;
//       }
//     }

//     std::cout << "\nTous les serveurs sont en cours d'exécution. Appuyez sur Ctrl+C pour les arrêter." << std::endl;

//     while (g_running) {
//     }

//   } catch (const std::runtime_error &e) {
//     std::cerr << "Une erreur est survenue : " << e.what() << std::endl;
//   }

//   for (size_t i = 0; i < serverInstances.size(); ++i) {
//     delete serverInstances[i];
//   }
//   std::cout << "Toutes les instances de serveur ont été nettoyées. Arrêt." << std::endl;

//   return 0;
// }
// static void printCommonConfig(const CommonConfig& config, const std::string& indent);
// static void printLocations(const std::map<std::string, LocationConfig>& locations, const std::string& indent);
// int main(int argc, char **argv)
// {
//     if (argc != 2)
//         return (writeError("ERROR: wrong number of arguments. Usage: ./webServ <config_file>", 1));

//     try
//     {
//         Config serverConfigs(argv[1]);
//         std::vector<ServerConfig>& servers = serverConfigs.getServers();

//         std::cout << "Configuration loaded successfully. Found " << servers.size() << " server block(s)." << std::endl;
//         std::cout << "=================================================" << std::endl;

//         for (size_t i = 0; i < servers.size(); ++i)
//         {
//             std::cout << "\n--- Server #" << i + 1 << " ---" << std::endl;

//             // Affichage des directives spécifiques au serveur
//             if (servers[i]._listenOn.first == "") {
//                 std::cout << "  Listen: (none specified, will use default)" << std::endl;
//             }
// 			else
// 			{
//                     std::cout << "  Listen: " << servers[i]._listenOn.first << ":" << servers[i]._listenOn.second << std::endl;
//             }

//             if (!servers[i]._serverName.empty()) {
//                 std::cout << "  Server Names: ";
//                 for (size_t j = 0; j < servers[i]._serverName.size(); ++j) {
//                     std::cout << servers[i]._serverName[j] << " ";
//                 }
//                 std::cout << std::endl;
//             }

//             if (!servers[i]._errorPage.empty()) {
//                 std::cout << "  Error Pages:" << std::endl;
//                 for (std::map<int, std::string>::const_iterator it = servers[i]._errorPage.begin(); it != servers[i]._errorPage.end(); ++it) {
//                     std::cout << "    " << it->first << " -> " << it->second << std::endl;
//                 }
//             }

//             // Affichage des directives communes au serveur
//             printCommonConfig(servers[i], "  ");

//             // Affichage des locations du premier niveau
//             if (!servers[i]._location.empty()) {
//                 std::cout << "  Locations:" << std::endl;
//                 printLocations(servers[i]._location, "    ");
//             }
//         }
//         std::cout << "=================================================" << std::endl;

//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << "Error: " << e.what() << '\n';
//         return (1);
//     }
//     return (0);
// }

// Définition des fonctions d'aide

// Affiche les directives de CommonConfig (CORRIGÉ : sans alias)
// static void printCommonConfig(const CommonConfig& config, const std::string& indent)
// {
//     if (!config._root.empty())
//         std::cout << indent << "Root: " << config._root << std::endl;

//     std::cout << indent << "Autoindex: " << (config._autoIndex ? "on" : "off") << std::endl;

//     if (!config._defaultFile.empty())
//         std::cout << indent << "Index: " << config._defaultFile << std::endl;

//     if (config._maxSizeBody > 0)
//         std::cout << indent << "Max Body Size: " << config._maxSizeBody << std::endl;

//     if (!config._allowedMethods.empty()) {
//         std::cout << indent << "Allowed Methods: ";
//         for (size_t i = 0; i < config._allowedMethods.size(); ++i) {
//             std::cout << config._allowedMethods[i] << " ";
//         }
//         std::cout << std::endl;
//     }

//     if (!config._cgiParams.empty()) {
//         std::cout << indent << "CGI Params:" << std::endl;
//         for (std::map<std::string, std::string>::const_iterator it = config._cgiParams.begin(); it != config._cgiParams.end(); ++it) {
//             std::cout << indent << "  " << it->first << " -> " << it->second << std::endl;
//         }
//     }
// }

// // Fonction récursive pour afficher les locations (CORRIGÉ : gère alias ici)
// static void printLocations(const std::map<std::string, LocationConfig>& locations, const std::string& indent)
// {
//     for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
//     {
//         std::cout << indent << "Path: " << it->first << std::endl;

//         // Affiche les directives spécifiques à la location
//         if (it->second._redirect.first != 0)
//             std::cout << indent << "  Redirect: " << it->second._redirect.first << " " << it->second._redirect.second << std::endl;
//         if (!it->second._uploadStore.empty())
//             std::cout << indent << "  Upload Store: " << it->second._uploadStore << std::endl;
//         if (!it->second._cgiPass.empty())
//             std::cout << indent << "  CGI Pass: " << it->second._cgiPass << std::endl;
//         if (!it->second._alias.empty())
//             std::cout << indent << "  Alias: " << it->second._alias << std::endl;

//         // Affiche les directives communes héritées ou surchargées
//         printCommonConfig(it->second, indent + "  ");

//         // Appel récursif pour les locations imbriquées
//         if (!it->second._locations.empty())
//         {
//             std::cout << indent << "  Nested Locations:" << std::endl;
//             printLocations(it->second._locations, indent + "    ");
//         }
//         std::cout << std::endl;
//     }
// }

// test request
int main(int argc, char **argv)
{
	if (argc != 2)
		return (writeError("ERROR: wrong number of argument", 1));
	try
	{
		std::string request(argv[1]);
		Request response;
		response.parseRequest(request);
		std::cout << "Method: " << response.getMethod() << std::endl;
		std::cout << "Path: " << response.getPath() << std::endl;
		std::cout << "Protocol: " << response.getPrtcl() << std::endl;
		std::map<std::string, std::string> headers = response.getHeaders();
		std::cout << "Headers: " << headers.size() << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
			std::cout << "Header: " << it->first << " = " << it->second << std::endl;
		std::cout << "Body: " << response.getBody() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}
