#ifndef PARSREQUEST_HPP
#define PARSREQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "../utils/httpStatus.hpp"
#include "../utils/trim.hpp"

class ParsRequest
{
	private:
		std::vector<std::string>			_request;
		std::string							_method;
		std::string							_path;
		std::string							_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		bool								_haveBody;

		void splitRequest(const std::string& request);
		void checkRequest(const std::string& request);
		void checkHeaders(const std::vector<std::string>::iterator& begin, const std::vector<std::string>::iterator& end);
		void checkKey(void);

	public:
		ParsRequest(std::string request);
		ParsRequest(const ParsRequest& other);
		ParsRequest& operator=(const ParsRequest& other);
		~ParsRequest();
		std::string getMethod() const;
		std::string getPath() const;
		std::string getProtocol() const;
		std::map<std::string, std::string> getHeaders() const;
		std::string getBody() const;
};

#endif
