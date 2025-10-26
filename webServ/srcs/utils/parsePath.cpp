#include "../../includes/utils/parsePath.hpp"

static std::string	getFileExtension(const std::string& path)
{
	std::string		ext;
	size_t			dotPos;

	dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos)
		return ("");
	ext = path.substr(dotPos);
	return (ext);
}

bool backPath(std::string& path)
{
	size_t slashPos = path.rfind("/");
	if (slashPos == std::string::npos)
		return (false);
	if (slashPos == 0)
	{
		if (path == "/")
			return (false);
		path = "/";
	}
	else
		path = path.substr(0, slashPos);
	return (true);
}

std::string	guessContentType(const std::string& path)
{
	std::string ext = getFileExtension(path);
	static std::map<std::string, std::string> mimeTypes;

	if (mimeTypes.empty())
	{
		mimeTypes[".aac"] = "audio/aac";
		mimeTypes[".avi"] = "video/x-msvideo";
		mimeTypes[".mid"] = "audio/midi";
		mimeTypes[".midi"] = "audio/midi";
		mimeTypes["*-+.mpeg"] = "video/mpeg";
		mimeTypes[".oga"] = "audio/ogg";
		mimeTypes[".ogv"] = "video/ogg";
		mimeTypes[".ogx"] = "application/ogg";
		mimeTypes[".ts"] = "application/typescript";
		mimeTypes[".wav"] = "audio/x-wav";
		mimeTypes[".weba"] = "audio/webm";
		mimeTypes[".webm"] = "video/webm";
		mimeTypes[".3gp"] = "video/3gpp";
		mimeTypes[".3g2"] = "video/3gpp2";
		mimeTypes[".bmp"] = "image/bmp";
		mimeTypes[".gif"] = "image/gif";
		mimeTypes[".ico"] = "image/x-icon";
		mimeTypes[".jpeg"] = "image/jpeg";
		mimeTypes[".jpg"] = "image/jpeg";
		mimeTypes[".png"] = "image/png";
		mimeTypes[".svg"] = "image/svg+xml";
		mimeTypes[".tif"] = "image/tiff";
		mimeTypes[".tiff"] = "image/tiff";
		mimeTypes[".webp"] = "image/webp";
		mimeTypes[".eot"] = "application/vnd.ms-fontobject";
		mimeTypes[".otf"] = "font/otf";
		mimeTypes[".ttf"] = "font/ttf";
		mimeTypes[".woff"] = "font/woff";
		mimeTypes[".woff2"] = "font/woff2";
		mimeTypes[".css"] = "text/css";
		mimeTypes[".htm"] = "text/html";
		mimeTypes[".html"] = "text/html";
		mimeTypes[".js"] = "application/javascript";
		mimeTypes[".json"] = "application/json";
		mimeTypes[".xhtml"] = "application/xhtml+xml";
		mimeTypes[".xml"] = "application/xml";
		mimeTypes[".abw"] = "application/x-abiword";
		mimeTypes[".csv"] = "text/csv";
		mimeTypes[".doc"] = "application/msword";
		mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		mimeTypes[".epub"] = "application/epub+zip";
		mimeTypes[".ics"] = "text/calendar";
		mimeTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
		mimeTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
		mimeTypes[".odt"] = "application/vnd.oasis.opendocument.text";
		mimeTypes[".pdf"] = "application/pdf";
		mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
		mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		mimeTypes[".rtf"] = "application/rtf";
		mimeTypes[".vsd"] = "application/vnd.visio";
		mimeTypes[".xls"] = "application/vnd.ms-excel";
		mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		mimeTypes[".arc"] = "application/octet-stream";
		mimeTypes[".bz"] = "application/x-bzip";
		mimeTypes[".bz2"] = "application/x-bzip2";
		mimeTypes[".jar"] = "application/java-archive";
		mimeTypes[".rar"] = "application/x-rar-compressed";
		mimeTypes[".tar"] = "application/x-tar";
		mimeTypes[".zip"] = "application/zip";
		mimeTypes[".7z"] = "application/x-7z-compressed";
		mimeTypes[".azw"] = "application/vnd.amazon.ebook";
		mimeTypes[".bin"] = "application/octet-stream";
		mimeTypes[".csh"] = "application/x-csh";
		mimeTypes[".mpkg"] = "application/vnd.apple.installer+xml";
		mimeTypes[".sh"] = "application/x-sh";
		mimeTypes[".swf"] = "application/x-shockwave-flash";
		mimeTypes[".xul"] = "application/vnd.mozilla.xul+xml";
	}
	if (mimeTypes.count(ext))
		return (mimeTypes[ext]);
	return ("text/plain");
}
