#include "../../includes/request/Request.hpp"

void Request::setLocation(const std::string& path)
{
	const std::map<std::string, LocationConfig>* current = &_config._location;
	std::map<std::string, LocationConfig>::const_iterator it;
	static_cast<CommonConfig&>(_location) = _config;
	const LocationConfig*	bestMatch;
	std::string				candidate;
	bool					valideBack;

	while (true)
	{
		candidate = path;
		valideBack = true;
		bestMatch = NULL;
		while (valideBack) {
			it = current->find(candidate);
			if (it != current->end()) {
				bestMatch = &it->second;
				break;
			}
			valideBack = backPath(candidate);
		}
		if (bestMatch == NULL)
			break;
		_location = *bestMatch;
		if (bestMatch->_locations.empty())
			break;
		current = &bestMatch->_locations;
	}
	if (_location._redirect.first != 0)
		throw HttpStatus(_location._redirect.first);
}

static std::string findExecutable(const std::string& path)
{
	std::string	executable = path;
	struct stat	info;

	while (executable.size() > 9) {
		if (stat(executable.c_str(), &info) == 0 && S_ISREG(info.st_mode)) {
			if (access(executable.c_str(), X_OK) == 0)
				return (executable);
			else
				throw HttpStatus(403);
		}
		backPath(executable);
	}
	throw HttpStatus(404);
}

void Request::addPath(const std::string&  word)
{
	std::string path = word;
	std::string truePath;
	std::string query = "";

	if (path.find("?") != std::string::npos) {
		query = path.substr(path.find("?") + 1);
		path = path.substr(0, path.find("?"));
	}
	if (path[0] != '/' || path.find("..") != std::string::npos)
		throw HttpStatus(400);
	setLocation(path);
	_uri = path;

	if (!_location._alias.second.empty())
		truePath = _location._alias.second + path.substr(path.find(_location._alias.first) + _location._alias.first.size());
	else
		truePath = _location._root + path;
	if (path.substr(0, 9) == "/cgi-bin/") {
		_isCGI = true;
		_path = findExecutable(truePath);
		if (_path.size() < truePath.size())
			_secondPath = truePath.substr(_path.size());
	} else {
		_isCGI = false;
		_path = truePath;
	}
	_query = query;
}

void Request::addMethod(const std::string&  word)
{
	if (word != "GET" && word != "POST" && word != "DELETE")
		throw HttpStatus(501);
	if (std::find(_location._allowedMethods.begin(), _location._allowedMethods.end(), word) == _location._allowedMethods.end())
		throw HttpStatus(405);
	_method = word;
}

void Request::addProtocol(const std::string&  word)
{
	if (word != "HTTP/1.1")
		throw HttpStatus(505);
	_protocol = word;
}


void Request::checkRequest(const std::string& request)
{
	std::istringstream	iss(request);
	std::string			word;
	std::string			method;
	int					wordCount = 0;

	while (iss >> word && wordCount < 3) {
		if (wordCount == 0)
			method = word;
		else if (wordCount == 1)
		{
			addPath(word);
			addMethod(method);
		}
		else if (wordCount == 2)
			addProtocol(word);
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
	while(pos <= end) {
		std::string	line = _request.substr(start, pos - start);
		size_t		colonPos = line.find(':');
		if (line.find("\r") != std::string::npos || colonPos == std::string::npos)
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
	std::map<std::string, std::string>::iterator contentLength = _headers.find("Content-Length");
	std::map<std::string, std::string>::iterator transferEncoding = _headers.find("Transfer-Encoding");

	if (_headers.find("Host") == _headers.end())
		throw HttpStatus(400);
	if (contentLength != _headers.end() && transferEncoding != _headers.end())
		throw HttpStatus(400);
	if (_method == "POST") {
		_bodyNecessary = true;
		if (contentLength == _headers.end() && transferEncoding == _headers.end())
			throw HttpStatus(411);
	} else
		_bodyNecessary = false;
	if (contentLength != _headers.end()) {
		if (!strToSizeT(contentLength->second, _contentLength, 10))
			throw HttpStatus(400);
		if (_contentLength > _location._maxSizeBody)
			throw HttpStatus(413);
	}
	if (transferEncoding != _headers.end()) {
		if (transferEncoding->second != "chunked")
			throw HttpStatus(501);
		_isChunked = true;
	}
	if (!_bodyNecessary)
		_isValid = true;
}

int Request::processRequest()
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

int Request::ProcessHeader()
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
	return (1);
}

int Request::validateAndSetBody()
{
	if (_request.size() < _contentLength)
		return (0);
	if (_request.size() != _contentLength)
		throw HttpStatus(400);
	_body = _request;
	_request = "";
	_haveBody = true;
	_isValid = true;
	return (1);
}

int Request::processChunkedRequest()
{
	size_t	pos = _request.find("\r\n");
	if (pos == std::string::npos)
		return (0);
	while (pos != std::string::npos) {
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
		if (_body.size() > _location._maxSizeBody)
			throw HttpStatus(413);
		_request = _request.substr(pos + 2 + chunkSize + 2);
		pos = _request.find("\r\n");
	}
	if (_request.empty() || !_haveBody)
		return (0);
	return (1);
}

int 	Request::ProcessTrailer()
{
	size_t	pos = _request.find("\r\n\r\n");
	if (pos == std::string::npos)
		return (0);
	splitHeader(pos);
	_request = _request.substr(pos + 4);
	_haveTrailers = true;
	if (!_request.empty())
		throw HttpStatus(400);
	_isValid = true;
	return (1);
}

int Request::parseRequest(const std::string& request)
{
	_request += request;

	if (!_haveRequest && !processRequest())
		return (0);
	if (!_haveHeader && !ProcessHeader())
		return (0);
	if (!_haveBody) {
		if (!_isChunked)
			return (validateAndSetBody());
		else if (!processChunkedRequest())
			return (0);
	}
	if (!_haveTrailers && !ProcessTrailer())
		return (0);
	return (1);
}

void Request::logRequest(int fd)
{
	std::cout << std::endl << "\033[34m" << "Request on fd " << fd << " -----------------------------------------------" << "\033[0m" << std::endl;
	std::cout << _method << " " << _uri;
	if (!_query.empty())
		std::cout << "?" << _query;
	std::cout << " " << _protocol << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << std::endl << _body << std::endl;
}

void Request::clear()
{
	_uri.clear();
	_headers.clear();
	_request.clear();
	_method.clear();
	_path.clear();
	_query.clear();
	_protocol.clear();
	_body.clear();

	_contentLength = 0;

	_haveRequest = false;
	_haveHeader = false;
	_haveBody = false;
	_haveTrailers = false;
	_isChunked = false;
	_isValid = false;
}

// Getters
const LocationConfig&				Request::getLocation()		const{ return _location; }
bool 								Request::getIsValid()		const{ return _isValid; }
std::string							Request::getMethod()		const{ return _method; }
std::string							Request::getUri()			const{ return _uri; }
std::string							Request::getPath()			const{ return _path; }
bool								Request::getIsCGI()			const{ return _isCGI; }
std::string							Request::getSecondPath()	const{ return _secondPath; }
std::string							Request::getQuery()			const{ return _query; }
std::string							Request::getPrtcl()			const{ return _protocol; }
std::string							Request::getBody()			const{ return _body; }
std::map<std::string, std::string>	Request::getHeaders()		const{ return _headers; }
std::string							Request::getHeader(const std::string& headerName) const
{
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		if (it->first.length() == headerName.length()) {
			bool match = true;
			for (size_t i = 0; i < it->first.length(); ++i) {
				if (std::tolower(it->first[i]) != std::tolower(headerName[i])) {
					match = false;
					break;
				}
			}
			if (match)
				return it->second;
		}
	}
	return "";
}

void	Request::setPath(std::string newPath)		{ _path = newPath; }
void	Request::setSecondPath(std::string newPath)	{ _secondPath = newPath; }

// Constructor
Request::Request(const ServerConfig& config) : _config(config)
{
	_request = "";
	_method = "";
	_path = "";
	_uri = "";
	_query = "";
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
	_isValid = false;
}

Request::Request(const Request& other) : _config(other._config)
{
	*this = other;
}

Request& Request::operator=(const Request& other)
{
	if (this == &other)
		return *this;
	this->_location = other._location;
	this->_request = other._request;
	this->_uri = other._uri;
	this->_method = other._method;
	this->_path = other._path;
	this->_query = other._query;
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
	this->_isValid = other._isValid;
	return *this;
}

Request::~Request()
{
}
