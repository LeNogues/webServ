#ifndef PARSREQUEST_HPP
# define PARSREQUEST_HPP


# include "../utils/httpStatus.hpp"
# include "../utils/strToSizeT.hpp"
# include "../utils/trim.hpp"

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>

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

		
		void	checkHeader(void);
		void	splitHeader(const size_t end);
		void	addPath(const std::string& word);
		void	addMethod(const std::string& word);
		void	addProtocol(const std::string& word);
		void	checkRequest(const std::string& request);
		int 	processChunkedRequest();
		int		validateAndSetBody();
		int		processRequest();
		int 	ProcessTrailer();
		int 	ProcessHeader();
		

	public:
		int parseRequest(const std::string& request);

		// Getters
		std::map<std::string, std::string> 	getHeaders()	const;
		std::string 						getMethod() 	const;
		std::string 						getPrtcl() 		const;
		std::string 						getPath() 		const;
		std::string 						getBody() 		const;

		// Constructors
		Request();
		Request(const Request& other);
		Request& operator=(const Request& other);
		~Request();
};

#endif
