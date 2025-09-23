#ifndef STATUSCODES_HPP
#define STATUSCODES_HPP

#include <exception>
#include <string>

class HttpException : public std::exception
{
	private:
		int			statusCode;
		std::string	message;
		void	setMessage();

	public:
		HttpException(int code);
		HttpException(const HttpException& other);
		HttpException& operator=(const HttpException& other);
		virtual ~HttpException() throw();

		virtual const char* what() const throw();

		int	getStatusCode() const;
};


#endif
