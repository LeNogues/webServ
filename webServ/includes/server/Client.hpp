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
# include "../httpGen/httpStatus.hpp"
# include "../request/Request.hpp"

# include <map>
# include <vector>
# include <cerrno>
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <errno.h>
# include <ctime>
# include <sys/time.h>
# include <sys/types.h>

class Client
{
    private:
        const ServerConfig&     _config;
        std::string             _response;
        Request                 _request;
        int                     _clientFd;

        void buildResponse(std::string& response, const std::map<std::string, std::string>& headers, const std::string& body);

    public:
        Client(int clientFd, const ServerConfig& config);
        ~Client();
        Request& getRequest();
        const std::string& getRoot();
        const std::string& getDefaultFile();
        const ServerConfig& getConfig();
		bool				getAutoIndex();
        void generateResponse(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body);
        void generateResponse(const int status, const std::map<std::string, std::string>& headers, const std::string& body);
        bool hasResponse() const;
        ssize_t sendPending();
};

#endif
