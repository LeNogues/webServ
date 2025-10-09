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
# include "../request/Request.hpp"

# include <map>
# include <vector>
# include <cerrno>

class Client
{
    private:
        const ServerConfig&     _config;
        std::string             _rawRequest;
        Request                 _request;
        int                     _clientFd;
        
        
    public:
        Client(int clientFd, const ServerConfig& config);
        ~Client();
        void injectIntoRawRequest(std::string partialRequest);
        std::string getRawRequest();
        Request getRequest();
};


#endif
