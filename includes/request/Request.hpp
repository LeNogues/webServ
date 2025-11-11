#ifndef PARSREQUEST_HPP
# define PARSREQUEST_HPP

# include "../config/ServerConfig.hpp"
# include "../httpGen/httpStatus.hpp"
# include "../utils/converters.hpp"
# include "../utils/parsePath.hpp"
# include "../utils/trim.hpp"

# include <sys/stat.h>
# include <algorithm>
# include <unistd.h>
# include <iostream>
# include <string>
# include <map>

// TODO:
class Request
{
	private:
		const ServerConfig&					_config;
		LocationConfig						_location;
		std::string							_request;
		std::map<std::string, std::string>	_headers;
		std::string							_method;
		std::string							_uri;
		std::string							_path;
		std::string							_secondPath;
		std::string							_query;
		std::string							_protocol;
		std::string							_body;
		bool								_isValid;
		bool								_isCGI;
		size_t								_contentLength;

		// Flags
		bool								_haveRequest;
		bool								_haveHeader;
		bool								_bodyNecessary;
		bool								_haveBody;
		bool								_haveTrailers;
		bool								_isChunked;

		void	setCommonConfig(const CommonConfig& config);
		void	setLocation(const std::string& path);
		void	addPath(const std::string& word);
		void	addMethod(const std::string& word);
		void	addProtocol(const std::string& word);
		void	checkRequest(const std::string& request);
		void	splitHeader(const size_t end);
		void	checkHeader(void);
		int 	processChunkedRequest();
		int		validateAndSetBody();
		int		processRequest();
		int 	ProcessTrailer();
		int 	ProcessHeader();

	public:
		int parseRequest(const std::string& request);
		void logRequest();
		void clear();

		// Getters/Setters
		std::string 						getHeader(const std::string& name) const;
		std::map<std::string, std::string> 	getHeaders()	const;
		const LocationConfig& 				getLocation()	const;
		std::string 						getMethod()		const;
		std::string 						getUri()		const;
		std::string 						getPath()		const;
		bool								getIsCGI()		const;
		std::string 						getSecondPath()	const;
		std::string 						getQuery()		const;
		std::string 						getPrtcl()		const;
		std::string 						getBody()		const;
		bool								getIsValid() const;

		void								setSecondPath(std::string path);
		void								setPath(std::string path);

		// Constructors
		Request(const ServerConfig& config);
		~Request();
		Request(const Request& other);
		Request& operator=(const Request& other);
};

#endif
