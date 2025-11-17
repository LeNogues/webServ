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


#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../config/ServerConfig.hpp"
# include "../utils/writeError.hpp"
# include "../utils/trim.hpp"
# include "../request/Request.hpp"

# include <sys/socket.h>
# include <sys/types.h>
# include <sys/types.h>
# include <sys/time.h>
# include <unistd.h>
# include <sstream>
# include <ctime>
# include <map>

class Client
{
	private:
		const ServerConfig&	 _config;
		LocationConfig		  _location;
		std::string			 _response;
		Request				 _request;
		std::string			 _cgiResponseBuffer;
		void buildResponse(std::string& response, const std::map<std::string, std::string>& headers, const std::string& body);

	public:
		Client(int clientFd, const ServerConfig& config);
		~Client();
		int									_clientFd;

		const std::map<int, std::string>&	getErrorPage();
		const ServerConfig&					getConfig();
		const std::string&					getRoot();
		const std::string&					getDefaultFile();
		Request&							getRequest();
		bool								getShouldClose() const;
		bool								getAutoIndex();
		void								setLocation(const LocationConfig& location);
		void								generateResponse(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body);
		void								generateResponse(const int status, const std::map<std::string, std::string>& headers, const std::string& body);
		bool								hasResponse() const;
		ssize_t								sendPending();
		std::string							genAbsPath(const std::string& path);
		void								appendToCgiBuffer(const char* data, size_t len);
		bool								parseCgiResponse();
};

#endif
