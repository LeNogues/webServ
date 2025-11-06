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

#include "../../includes/server/WebServer.hpp"

void setNonBlocking(int serverFD)
{
    int flags = fcntl(serverFD, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("ERROR: fcntl failed");

    flags |= O_NONBLOCK;
    int s = fcntl(serverFD, F_SETFL, flags);
    if (s == -1)
        throw std::runtime_error("ERROR: fcntl failed");
}