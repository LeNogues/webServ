#include "../../includes/request/Request.hpp"

void Request::checkRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string			word;
	int					wordCount = 0;

	while (iss >> word && wordCount < 3)
	{
		if (wordCount == 0)
		{
			if (word != "GET" && word != "POST" && word != "DELETE")
			{
				if (word == "PUT" || word == "PATCH" || word == "OPTIONS" || word == "CONNECT" || word == "TRACE")
					throw HttpStatus(501);
				throw HttpStatus(405);
			}
			_method = word;
		}
		else if (wordCount == 1)
		{
			if (word[0] != '/' || word.find("..") != std::string::npos)
				throw HttpStatus(400);
			_path = word;
		}
		else if (wordCount == 2)
		{
			if (word != "HTTP/1.1")
				throw HttpStatus(505);
			_protocol = word;
		}
		wordCount++;
	}
	if (wordCount != 3)
		throw HttpStatus(400);
	if (_request.size() > 8192)
		throw HttpStatus(414);
}

void Request::splitHeader(const size_t end)
{
	size_t	start = 0;
	size_t	pos = 0;

	pos = _request.find("\r\n", start);
	while(pos <= end)
	{
		std::string line = _request.substr(start, pos - start);
		if (line.find('\r') != std::string::npos)
			throw HttpStatus(400);
		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			throw HttpStatus(400);
		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		btrim(key);
		btrim(value);
		if (key.empty() || value.empty() || _headers.find(key) != _headers.end())
			throw HttpStatus(400);
		_headers[key] = value;
		start = pos + 2;
		pos = _request.find("\r\n", start);
	}
}

void Request::checkHeader(void)
{
	std::map<std::string, std::string>::iterator	contentLength = _headers.find("Content-Length");
	std::map<std::string, std::string>::iterator	transferEncoding = _headers.find("Transfer-Encoding");

	if (_headers.find("Host") == _headers.end())
		throw HttpStatus(400);
	if (contentLength != _headers.end() && transferEncoding != _headers.end())
		throw HttpStatus(400);
	if (contentLength == _headers.end() && transferEncoding == _headers.end())
		_bodyNecessary = false;
	if (!_bodyNecessary && _request.size() > 0)
		throw HttpStatus(411);
	if (contentLength != _headers.end())
	{
		if (!strToSizeT(contentLength->second, _contentLength, 10))
			throw HttpStatus(400);
	}
	if (transferEncoding != _headers.end())
	{
		if (transferEncoding->second != "chunked")
			throw HttpStatus(400);
		_isChunked = true;
	}
}

// Constructor
Request::Request()
{
	_request = "";
	_method = "";
	_path = "";
	_protocol = "";
	_headers.clear();
	_body = "";
	_haveRequest = false;
	_haveHeader = false;
	_bodyNecessary = true;
	_haveBody = false;
	_haveTrailers = false;
	_isChunked = false;
	_contentLength = 0;
}

Request::Request(const Request& other)
{
	*this = other;
}

Request& Request::operator=(const Request& other)
{
	if (this == &other)
		return *this;
	this->_request = other._request;
	this->_method = other._method;
	this->_path = other._path;
	this->_protocol = other._protocol;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_haveRequest = other._haveRequest;
	this->_haveHeader = other._haveHeader;
	this->_bodyNecessary = other._bodyNecessary;
	this->_haveBody = other._haveBody;
	this->_haveTrailers = other._haveTrailers;
	this->_isChunked = other._isChunked;
	this->_contentLength = other._contentLength;
	return *this;
}

Request::~Request()
{
}

int Request::parseRequest(const std::string& request)
{
	_request += request;
	if (_haveTrailers || (_haveBody && !_isChunked))
		throw HttpStatus(400);
	else if (!_bodyNecessary)
		throw HttpStatus(411);
	if (!_haveRequest)
	{
		size_t	pos = _request.find("\r\n");
		if (pos == std::string::npos)
			return (0);
		checkRequest(_request.substr(0, pos));
		_haveRequest = true;
		_request = _request.substr(pos + 2);
		if (_request.empty())
			return (0);
		return (1);
	}
	if (!_haveHeader)
	{
		size_t	pos = _request.find("\r\n\r\n");
		if (pos == std::string::npos)
			return (0);
		splitHeader(pos);
		_request = _request.substr(pos + 4);
		checkHeader();
		_haveHeader = true;
		if (_request.empty())
			return (0);
	}
	if (!_haveBody)
	{
		if (!_isChunked)
		{
			// TODO: check if tcp send all data in one time
			if (!_body.empty() || _request.size() != _contentLength)
				throw HttpStatus(400);
			_body = _request;
			_request = "";
			_haveBody = true;
			return (1);
		}
		else
		{
			size_t	pos = _request.find("\r\n");
			if (pos == std::string::npos)
				return (0);
			while (pos != std::string::npos)
			{
				std::string	chunk, hex;
				size_t	chunkSize;
				chunk = _request.substr(0, pos);
				if (chunk.size() == 1 && chunk == "0")
				{
					_request = _request.substr(pos + 2);
					_haveBody = true;
					break;
				}
				if (chunk.find(";") != std::string::npos)
					hex = chunk.substr(0, chunk.find(";"));
				else
					hex = chunk;
				if (!strToSizeT(hex, chunkSize, 16))
					throw HttpStatus(400);
				if (_request.size() - (chunk.size() + 2) < chunkSize + 2)
					return (0);
				if (_request.substr(pos + 2 + chunkSize, 2) != "\r\n")
					throw HttpStatus(400);
				_body += _request.substr(pos + 2, chunkSize);
				_request = _request.substr(pos + 2 + chunkSize + 2);
				pos = _request.find("\r\n");
			}
			if (_request.empty() || !_haveBody)
				return (0);
		}
	}
	if (!_haveTrailers)
	{
		// TODO: check potential not detected end
		size_t	pos = _request.find("\r\n\r\n");
		if (pos == std::string::npos)
			return (0);
		splitHeader(pos);
		_request = _request.substr(pos + 4);
		_haveTrailers = true;
		if (!_request.empty())
			throw HttpStatus(400);
	}
	return (1);
}

// Getters
std::string Request::getMethod() const{ return _method; }
std::string Request::getPath() const{ return _path; }
std::string Request::getProtocol() const{ return _protocol; }
std::map<std::string, std::string> Request::getHeaders() const{ return _headers; }
std::string Request::getBody() const
{
	if (_haveBody)
		return (_body);
	else
		return ("");
}
