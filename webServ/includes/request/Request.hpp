#ifndef PARSREQUEST_HPP
# define PARSREQUEST_HPP

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include "../utils/httpStatus.hpp"
# include "../utils/trim.hpp"
# include "../utils/strToSizeT.hpp"

class Request
{
	private:
		std::map<std::string, std::string>	_headers;
		std::string							_request;
		std::string							_method;
		std::string							_path;
		std::string							_protocol;
		std::string							_body;
		size_t								_contentLength;
		bool								_haveRequest;
		bool								_haveHeader;
		bool								_bodyNecessary;
		bool								_haveBody;
		bool								_haveTrailers;
		bool								_isChunked;

		
		void checkRequest(const std::string& request);
		void splitHeader(const size_t end);
		void checkHeader(void);

	public:
		int parseRequest(const std::string& request);

		// Getters
		std::map<std::string, std::string> 	getHeaders()	const;
		std::string 						getMethod() 	const;
		std::string 						getPath() 		const;
		std::string 						getProtocol() 	const;
		std::string 						getBody() 		const;

		// Constructors
		Request();
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request();
};

#endif
