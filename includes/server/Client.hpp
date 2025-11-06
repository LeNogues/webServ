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
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <ctime>
# include <sys/time.h>
# include <sys/types.h>

class Client
{
    private:
        const ServerConfig&     _config;
        LocationConfig          _location;
        std::string             _response;
        Request                 _request;
        bool                    _ShouldClose;
        int                     _clientFd;

        void buildResponse(std::string& response, const std::map<std::string, std::string>& headers, const std::string& body);

    public:
        Client(int clientFd, const ServerConfig& config);
        ~Client();
        Request& getRequest();
        const std::string&                getRoot();
        const std::string&                getDefaultFile();
        const ServerConfig&               getConfig();
        bool                              getShouldClose() const;
        void                              setShouldClose(bool state);
        void                              setLocation(const LocationConfig& location);
        bool	                          getAutoIndex();
        const std::map<int, std::string>& getErrorPage();
        void                              generateResponse(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body);
        void                              generateResponse(const int status, const std::map<std::string, std::string>& headers, const std::string& body);
        bool                              hasResponse() const;
        ssize_t                           sendPending();
        std::string                       genAbsPath(const std::string& path);
};

#endif
