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
	: _config(config), _response(""), _request(_config), _clientFd(clientFd)
{
	_ShouldClose = 1;
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
	std::tm *ltm = std::localtime(&now);
	char buf[40];
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", ltm);
	return (std::string(buf));
}

void Client::buildResponse(std::string& response, const std::map<std::string, std::string>& headers, const std::string& body)
{
	response += "Date: " + getCurrentDate() + "\r\n";
	response += "Server: " + _config._serverName[0] + "\r\n";
	response += "Content-Length: " + intToString(body.size()) + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n" + body;
}

const std::string& Client::getRoot()
{
	return (this->_location._root);
}

const std::string& Client::getDefaultFile()
{
	return (this->_location._defaultFile);
}

bool	Client::getAutoIndex() {
	return (this->_location._autoIndex);
}

bool Client::getShouldClose() const
{
	return (this->_ShouldClose);
}

void Client::setShouldClose(bool state)
{
	_ShouldClose = state;
}

void Client::setLocation(const LocationConfig& location)
{
	_location = location;
}

const ServerConfig& Client::getConfig()
{
	return (_config);
}

const std::map<int, std::string>& Client::getErrorPage()
{
	return (_location._errorPage);
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
