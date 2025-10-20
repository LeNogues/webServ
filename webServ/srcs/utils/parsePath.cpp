#include "../../includes/utils/parsePath.hpp"

static std::string	getFileExtension(const std::string& path)
{
	std::string		ext;
	size_t			dotPos;

	dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return ("");
	ext = path.substr(dotPos + 1);
	return (ext);
}

std::string	guessContentType(const std::string& path)
{
	std::string ext = getFileExtension(path);

	if (ext == "html" || ext == "htm")
		return ("text/html");
	if (ext == "css")
		return ("text/css");
	if (ext == "js")
		return ("application/javascript");
	if (ext == "json")
		return ("application/json");
	if (ext == "png")
		return ("image/png");
	if (ext == "jpg" || ext == "jpeg")
		return ("image/jpeg");
	if (ext == "gif")
		return ("image/gif");
	if (ext == "svg")
		return ("image/svg+xml");
	if (ext == "txt")
		return ("text/plain");
	return ("application/octet-stream");
}
