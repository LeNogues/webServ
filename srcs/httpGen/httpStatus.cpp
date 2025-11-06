#include "../../includes/httpGen/httpStatus.hpp"

std::map<int, std::string>	STATUS_CODES = initStatusCodes();

std::map<int, std::string>	initStatusCodes(void)
{
	std::map<int, std::string>	codes;

	codes[200] = "HTTP/1.1 200 OK";
	codes[201] = "HTTP/1.1 201 Created";
	codes[204] = "HTTP/1.1 204 No Content";
	codes[206] = "HTTP/1.1 206 Partial Content";
	codes[300] = "HTTP/1.1 300 Multiple Choices";
	codes[301] = "HTTP/1.1 301 Moved Permanently";
	codes[302] = "HTTP/1.1 302 Found";
	codes[303] = "HTTP/1.1 303 See Other";
	codes[304] = "HTTP/1.1 304 Not Modified";
	codes[305] = "HTTP/1.1 305 Use Proxy";
	codes[306] = "HTTP/1.1 306 Switch Proxy";
	codes[307] = "HTTP/1.1 307 Temporary Redirect";
	codes[308] = "HTTP/1.1 308 Permanent Redirect";
	codes[400] = "HTTP/1.1 400 Bad Request";
	codes[401] = "HTTP/1.1 401 Unauthorized";
	codes[403] = "HTTP/1.1 403 Forbidden";
	codes[404] = "HTTP/1.1 404 Not Found";
	codes[405] = "HTTP/1.1 405 Method Not Allowed";
	codes[408] = "HTTP/1.1 408 Request Timeout";
	codes[411] = "HTTP/1.1 411 Length Required";
	codes[413] = "HTTP/1.1 413 Payload Too Large";
	codes[414] = "HTTP/1.1 414 URI Too Long";
	codes[500] = "HTTP/1.1 500 Internal Server Error";
	codes[501] = "HTTP/1.1 501 Not Implemented";
	codes[502] = "HTTP/1.1 502 Bad Gateway";
	codes[503] = "HTTP/1.1 503 Service Unavailable";
	codes[505] = "HTTP/1.1 505 HTTP Version Not Supported";
	return (codes);
}

std::string	getStatusMessage(int statusCode)
{
	std::map<int, std::string>::const_iterator	it;

	it = STATUS_CODES.find(statusCode);
	if (it != STATUS_CODES.end())
		return (it->second);
	return ("HTTP/1.1 500 Internal Server Error");
}

HttpStatus::HttpStatus(int code) : _statusCode(code)
{
	_message = getStatusMessage(code);
}

HttpStatus::HttpStatus(const HttpStatus& other)
{
	*this = other;
}

HttpStatus&	HttpStatus::operator=(const HttpStatus& other)
{
	if (this == &other)
		return (*this);
	_statusCode = other._statusCode;
	_message = other._message;
	return (*this);
}

HttpStatus::~HttpStatus() throw()
{
}

const char*	HttpStatus::what() const throw()
{
	return (_message.c_str());
}

int	HttpStatus::getStatusCode() const
{
	return (_statusCode);
}
