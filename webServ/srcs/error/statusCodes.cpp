#include "../../includes/error/statusCodes.hpp"

void  HttpException::setMessage()
{
	switch (statusCode)
	{
		case 400:
			message = "400 Bad Request";
			break;
		case 401:
			message = "401 Unauthorized";
			break;
		case 403:
			message = "403 Forbidden";
			break;
		case 404:
			message = "404 Not Found";
			break;
		case 405:
			message = "405 Method Not Allowed";
			break;
		case 408:
			message = "408 Request Timeout";
			break;
		case 413:
			message = "413 Payload Too Large";
			break;
		case 500:
			message = "500 Internal Server Error";
			break;
		case 501:
			message = "501 Not Implemented";
			break;
		case 502:
			message = "502 Bad Gateway";
			break;
		case 503:
			message = "503 Service Unavailable";
			break;
		case 505:
			message = "505 HTTP Version Not Supported";
			break;
		default:
			message = "500 Internal Server Error";
			statusCode = 500;
			break;
	}
}

HttpException::HttpException(int code) : statusCode(code)
{
	setMessage();
}

HttpException::HttpException(const HttpException& other)
{
	*this = other;
}

HttpException& HttpException::operator=(const HttpException& other)
{
	if (this == &other)
		return *this;
	statusCode = other.statusCode;
	message = other.message;
	return *this;
}

HttpException::~HttpException() throw()
{
}

const char* HttpException::what() const throw()
{
	return message.c_str();
}

int HttpException::getStatusCode() const
{
	return statusCode;
}
