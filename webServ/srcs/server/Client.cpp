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
    _response = "";
}

Client::~Client()
{
}

Request& Client::getRequest()
{
    return (_request);
}

static std::string getCurrentDate()
{
	time_t now = std::time(0);
	std::tm *gtm = std::gmtime(&now);
	char buf[40];
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gtm);
	return (std::string(buf));
}

static void buildResponse(std::string& response, const std::map<std::string, std::string>& headers, const std::string& body)
{
	response += "Date: " + getCurrentDate() + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n" + body;
}

void Client::generateResponse(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body)
{
    _response = status + "\r\n";
	buildResponse(_response, headers, body);
}
void Client::generateResponse(const int status, const std::map<std::string, std::string>& headers, const std::string& body)
{
    _response = getStatusMessage(status) + "\r\n";
	buildResponse(_response, headers, body);
}

bool	Client::hasResponse() const
{
    return (!_response.empty());
}

ssize_t	Client::sendPending()
{
    ssize_t	bytesSent;

    if (_response.empty())
        return (0);
    bytesSent = send(_clientFd, _response.data(), _response.size(), 0);
    if (bytesSent > 0)
    {
        _response.erase(0, static_cast<size_t>(bytesSent));
        return (bytesSent);
    }
    if (bytesSent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return (-2);
    return (-1);
}
