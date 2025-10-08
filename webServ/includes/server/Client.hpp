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

# include <map>
# include <vector>
# include <cerrno>
# include "../config/ServerConfig.hpp"

class Client
{
    private:
        const ServerConfig&     _config;
        int                     _clientFd;
        
    public:
        Client(int clientFd, const ServerConfig& config);
        ~Client();
};


#endif
