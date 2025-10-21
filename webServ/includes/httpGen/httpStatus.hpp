#ifndef STATUSCODES_HPP
#define STATUSCODES_HPP

#include <exception>
#include <string>
#include <algorithm>
#include <map>

extern std::map<int, std::string>	STATUS_CODES;

std::map<int, std::string>	initStatusCodes(void);
std::string	getStatusMessage(int statusCode);

class HttpStatus : public std::exception
{
	private:
		int			_statusCode;
		std::string	_message;

	public:
		HttpStatus(int code);
		HttpStatus(const HttpStatus& other);
		HttpStatus&	operator=(const HttpStatus& other);
		virtual			~HttpStatus() throw();

		virtual const char*	what() const throw();
		int					getStatusCode() const;
};

#endif
