#include "../../includes/request/parsRequest.hpp"

void ParsRequest::splitRequest(const std::string& request)
{
	bool	amptylin = false;
	size_t	start = 0;
	size_t	pos = 0;

	if (request.empty() || request.size() < 25 || request.compare(0, 2, "\r\n") == 0)
		throw HttpException(400);
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
	if (!amptylin || _request.empty() || _request.size() < 2)
		throw HttpException(400);
	else if (start < request.size())
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
				throw HttpException(405);
			_method = word;
		}
		else if (wordCount == 1)
		{
			if (word[0] != '/')
				throw HttpException(400);
			_path = word;
		}
		else if (wordCount == 2)
		{
			if (word != "HTTP/1.1")
				throw HttpException(505);
			_protocol = word;
		}
		wordCount++;
	}
	if (wordCount != 3)
		throw HttpException(400);
}

void ParsRequest::checkHeaders(const std::vector<std::string>::iterator& begin, const std::vector<std::string>::iterator& end)
{
	for (std::vector<std::string>::iterator it = begin; it != end; ++it)
	{
		size_t	colonPos;
		std::string	key;
		std::string	value;

		if (it->find('\r') != std::string::npos)
			throw HttpException(400);
		colonPos = it->find(':');
		if (colonPos == std::string::npos)
			throw HttpException(400);

		key = it->substr(0, colonPos);
		value = it->substr(colonPos + 1);
		btrim(key);
		btrim(value);

		if (key.empty() || value.empty() || _headers.find(key) != _headers.end())
			throw HttpException(400);
		_headers[key] = value;
	}
	if (_headers.find("Host") == _headers.end())
		throw HttpException(400);
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
		checkHeaders(_request.begin() + 1, _request.end() - 1);
		_body = _request.back();
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
std::string ParsRequest::getMethod() const{ return this->_method; }
std::string ParsRequest::getPath() const{ return this->_path; }
std::string ParsRequest::getProtocol() const{ return this->_protocol; }
std::map<std::string, std::string> ParsRequest::getHeaders() const{ return this->_headers; }
std::string ParsRequest::getBody() const{ return this->_body; }
