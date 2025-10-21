#include "../../includes/httpGen/generateAutoIndex.hpp"

static std::string	buildHeader(const std::string& path)
{
	std::string	header = "<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
			"<meta charset=\"UTF-8\">\n"
			"<title>AutoIndex</title>\n"
		"</head>\n"
		"<style>\n"
			"body {\n"
				"display: flex;\n"
				"flex-direction: column;\n"
				"align-items: start;\n"
				"justify-content: start;\n"
				"height: 100%;\n"
				"font-family: Arial, sans-serif;\n"
				"margin: 0;\n"
				"padding: 1rem;\n"
			"}\n"
			"h1 {\n"
				"font-size: 2rem;\n"
				"font-weight: bold;\n"
				"color: #002ed2;\n"
				"line-height: 1.3;\n"
				"margin: 0;\n"
			"}\n"
			".autoindex-container {\n"
				"width: 100%;\n"
				"display: flex;\n"
				"flex-direction: column;\n"
				"align-items: start;\n"
				"justify-content: start;\n"
				"height: 100%;\n"
				"font-family: Arial, sans-serif;\n"
				"margin: 1rem;\n"
				"padding: 1rem;\n"
				"gap: 0.5rem;\n"
			"}\n"
			".item {\n"
				"display: flex;\n"
				"align-items: center;\n"
				"gap: 0.5rem;\n"
			"}\n"
			".item.dir:before { content: \"📁\"; }\n"
			".item.file:before { content: \"📄\"; }\n"
			".autoindex-container a {\n"
				"text-decoration: none;\n"
				"color: #666;\n"
				"padding: 0.25rem 0.5rem;\n"
				"border-radius: 6px;\n"
			"}\n"
			".autoindex-container a:hover {\n"
				"background: #f3f5ff;\n"
				"color: #002ed2;\n"
			"}\n"
		"</style>\n"
		"<body>\n"
			"<h1> Index of " + path + "</h1>\n"
			"<div class=\"autoindex-container\">\n";

	return header;
}

static std::string	buildFooter(void)
{
	std::string	footer;

	footer = "</div>\n"
		"</body>\n"
		"</html>\n";

	return footer;
}

static void			appendEntryLine(std::string& body, const std::string& entryName, int isDir)
{
	std::string	displayName = entryName;
	std::string	itemClass = isDir ? "dir" : "file";

	if (isDir)
		displayName = displayName + "/";
	body += "<div class=\"item " + itemClass + "\"><a href=\"" + entryName + "\">" + displayName + "</a></div>\n";
}

static void			appendDirectoryEntries(std::string& body, const std::string& path, DIR* dir)
{
	struct dirent*	entry = NULL;
	struct stat		entryStat;
	int				isDir;
	std::string		entryName;
	std::string		entryPath;

	while ((entry = readdir(dir)) != NULL)
	{
		entryName = std::string(entry->d_name);
		entryPath = path;
		if (!path.empty() && path[path.size() - 1] != '/')
			entryPath = entryPath + "/";
		entryPath = entryPath + entryName;

		isDir = 0;
		if (stat(entryPath.c_str(), &entryStat) == 0 && S_ISDIR(entryStat.st_mode))
			isDir = 1;

		appendEntryLine(body, entryName, isDir);
	}
}

std::string			generateAutoIndex(const std::string& path)
{
	std::string	body = "";
	DIR*		dir = opendir(path.c_str());

	if (dir == NULL)
		throw HttpStatus(403);
	body += buildHeader(path);
	appendDirectoryEntries(body, path, dir);
	body += buildFooter();
	closedir(dir);

	return body;
}
