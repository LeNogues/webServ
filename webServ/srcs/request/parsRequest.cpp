#include "../../includes/request/parsRequest.hpp"

void ParsRequest::splitRequest(const std::string& request)
{
	bool	amptylin = false;
	size_t	start = 0;
	size_t	pos = 0;

	if (request.empty() || request.size() < 25 || request.compare(0, 2, "\r\n") == 0)
		throw HttpStatus(400);
	pos = request.find("\r\n", start);
	while(pos != std::string::npos)
	{
		if (request.compare(start, 2, "\r\n") == 0)
		{
			amptylin = true;
			start += 2;
			break;
		}
		_request.push_back(request.substr(start, pos - start));
		start = pos + 2;
		pos = request.find("\r\n", start);
	}
	if (!amptylin || _request.empty() || _request.size() < 2) // check if the request is empty or too short
		throw HttpStatus(400);
	else if (_request[0].size() > 8192) // check if the request line is too long
		throw HttpStatus(414);
	else if (start < request.size()) // create the body if it exists
	{
		_haveBody = true;
		_request.push_back(request.substr(start));
	}
}

void ParsRequest::checkRequest(const std::string& request)
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
}

void ParsRequest::checkKey(void)
{
	std::map<std::string, std::string>::iterator	contentLength = _headers.find("Content-Length");
	std::map<std::string, std::string>::iterator	transferEncoding = _headers.find("Transfer-Encoding");
	size_t	contentLengthValue = 0;

	if (_headers.find("Host") == _headers.end())
		throw HttpStatus(400);
	if ((!_haveBody && (contentLength != _headers.end() || transferEncoding != _headers.end()))
		|| (contentLength != _headers.end() && transferEncoding != _headers.end()))
		throw HttpStatus(400);
	if (_haveBody && contentLength == _headers.end() && transferEncoding == _headers.end())
		throw HttpStatus(411);
	if (contentLength != _headers.end())
	{
		if (contentLength->second.find_first_not_of("0123456789") != std::string::npos)
			throw HttpStatus(400);
		contentLengthValue = std::atoi(contentLength->second.c_str());
		if (contentLengthValue != _body.size())
			throw HttpStatus(400);
	}
	if (transferEncoding != _headers.end())
	{
		if (transferEncoding->second != "chunked")
			throw HttpStatus(400);
	}
}
void ParsRequest::checkHeaders(const std::vector<std::string>::iterator& begin, const std::vector<std::string>::iterator& end)
{
	for (std::vector<std::string>::iterator it = begin; it != end; ++it)
	{
		size_t	colonPos;
		std::string	key;
		std::string	value;

		if (it->find('\r') != std::string::npos) // check if the header contains a carriage return
			throw HttpStatus(400);
		colonPos = it->find(':');
		if (colonPos == std::string::npos) // check if the header contains a colon
			throw HttpStatus(400);

		key = it->substr(0, colonPos);
		value = it->substr(colonPos + 1);
		btrim(key);
		btrim(value);

		if (key.empty() || value.empty() || _headers.find(key) != _headers.end())
			throw HttpStatus(400);
		_headers[key] = value;
	}
	checkKey(); // check they minimum headers values
}

// Constructor
ParsRequest::ParsRequest(std::string request)
{
	_haveBody = false;

	splitRequest(request);
	checkRequest(_request[0]);

	if (!_haveBody)
		checkHeaders(_request.begin() + 1, _request.end());
	else
	{
		_body = _request.back();
		checkHeaders(_request.begin() + 1, _request.end() - 1);
	}
}

ParsRequest::ParsRequest(const ParsRequest& other)
{
	*this = other;
}

ParsRequest& ParsRequest::operator=(const ParsRequest& other)
{
	if (this == &other)
		return *this;
	this->_request = other._request;
	this->_method = other._method;
	this->_path = other._path;
	this->_protocol = other._protocol;
	this->_headers = other._headers;
	this->_body = other._body;
	this->_haveBody = other._haveBody;
	return *this;
}

ParsRequest::~ParsRequest()
{
}

// Getters
std::string ParsRequest::getMethod() const{ return _method; }
std::string ParsRequest::getPath() const{ return _path; }
std::string ParsRequest::getProtocol() const{ return _protocol; }
std::map<std::string, std::string> ParsRequest::getHeaders() const{ return _headers; }
std::string ParsRequest::getBody() const
{
	if (_haveBody)
		return (_body);
	else
		return ("");
}
