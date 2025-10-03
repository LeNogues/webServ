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

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include "../config/Config.hpp"
# include  <unistd.h>


class Server
{
    private:
        std::vector<int>    _listeningSockets;
        const Config&       _config;
            
 
    public:
        Server(const Config& config);
        ~Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
};

#endif