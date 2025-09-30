#ifndef PARSREQUEST_HPP
#define PARSREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../utils/httpStatus.hpp"
#include "../utils/trim.hpp"

class Request
{
	private:
		std::string							_request;
		std::string							_method;
		std::string							_path;
		std::string							_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		bool								_haveRequest;
		bool								_haveHeader;
		bool								_haveBody;
		size_t								_contentLength;
		bool								_isChunked;

		void checkRequest(const std::string& request);
		void splitHeader(size_t end);
		void checkHeader(void);

	public:
		// Constructors
		Request();
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request();

		int parseRequest(const std::string& request);


		// Getters
		std::string getMethod() const;
		std::string getPath() const;
		std::string getProtocol() const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
};

#endif
