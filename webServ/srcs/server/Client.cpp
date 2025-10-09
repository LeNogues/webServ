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

#include "../../includes/server/Client.hpp"


Client::Client(int clientFd, const ServerConfig& config)
    : _config(config), _clientFd(clientFd)
{
    (void)_clientFd;
    (void)_config;
}

Client::~Client()
{
}


std::string Client::getRawRequest()
{
    return (_rawRequest);
}

Request Client::getRequest()
{
    return (_request);
}

void Client::injectIntoRawRequest(std::string partialRequest)
{
    _rawRequest += partialRequest;
}