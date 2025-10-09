#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "../utils/httpStatus.hpp"
# include <sstream>
# include <iostream>
# include <iomanip>
# include <string>
# include <ctime>
# include <map>

class Response
{
	private:
		std::string _response;
	public:
		Response(const std::string& status, const std::map<std::string, std::string>& headers, const std::string& body);
		Response(const int status, const std::map<std::string, std::string>& headers, const std::string& body);
		Response(const Response& other);
		~Response();
		Response& operator=(const Response& other);
		std::string sendResponse();
};

#endif
