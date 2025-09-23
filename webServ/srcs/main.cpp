#include "../includes/config/Config.hpp"
#include "../includes/error/writeError.hpp"
#include "../includes/request/parsRequest.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
		return (writeError("ERROR: wrong number of argument", 1));
	try
	{
		//Config allServeur(argv[1]);
		std::string request(argv[1]);
		ParsRequest response(request);
		std::cout << "Method: " << response.getMethod() << std::endl;
		std::cout << "Path: " << response.getPath() << std::endl;
		std::cout << "Protocol: " << response.getProtocol() << std::endl;
		std::map<std::string, std::string> headers = response.getHeaders();
		std::cout << "Headers: " << headers.size() << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
			std::cout << "Header: " << it->first << " = " << it->second << std::endl;
		std::cout << "Body: " << response.getBody() << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}
