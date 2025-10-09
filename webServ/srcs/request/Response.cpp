#include "../../includes/request/Response.hpp"

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

Response::Response(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body)
{
	_response = status + "\r\n";
	buildResponse(_response, headers, body);
}

Response::Response(const int status, const std::map<std::string, std::string>& headers, const std::string& body)
{
	_response = getStatusMessage(status) + "\r\n";
	buildResponse(_response, headers, body);
}

Response::~Response() {}

std::string Response::sendResponse()
{
	return (_response);
}
