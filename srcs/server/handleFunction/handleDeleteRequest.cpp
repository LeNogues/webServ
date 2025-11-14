#include "../../../includes/server/WebServer.hpp"

static bool	hasPermissionForMode(const struct stat &buffer, mode_t userMask, mode_t groupMask, mode_t otherMask)
{
	uid_t	userId;
	gid_t	groupId;

	userId = geteuid();
	groupId = getegid();
	if (userId == buffer.st_uid)
		return ((buffer.st_mode & userMask) == userMask);
	if (groupId == buffer.st_gid)
		return ((buffer.st_mode & groupMask) == groupMask);
	return ((buffer.st_mode & otherMask) == otherMask);
}

static void	ensureCanDeleteTarget(const struct stat &buffer, const std::string &filePath, bool isDirectory)
{
	bool	hasRights;

	if (isDirectory == true)
		hasRights = hasPermissionForMode(buffer, S_IWUSR | S_IXUSR, S_IWGRP | S_IXGRP, S_IWOTH | S_IXOTH);
	else
		hasRights = hasPermissionForMode(buffer, S_IWUSR, S_IWGRP, S_IWOTH);
	if (hasRights == false) {
		std::cout << "Warning: insufficient rights to delete: " << filePath << std::endl;
		throw HttpStatus(403);
	}
}

static void	deleteDirectoryTarget(const std::string &filePath)
{
	if (std::remove(filePath.c_str()) == -1) {
		std::cout << "Warning: cannot delete directory: " << filePath << " (" << std::strerror(errno) << ")" << std::endl;
		if (errno == EACCES || errno == EPERM)
			throw HttpStatus(403);
		if (errno == ENOTEMPTY)
			throw HttpStatus(409);
		throw HttpStatus(500);
	}
}

static void	deleteFileTarget(const std::string &filePath)
{
	if (std::remove(filePath.c_str()) == -1) {
		std::cout << "Warning: cannot delete: " << filePath << " (" << std::strerror(errno) << ")" << std::endl;
		if (errno == EACCES || errno == EPERM)
			throw HttpStatus(403);
		throw HttpStatus(500);
	}
}

void handleDeleteRequest(Client& currentClient, const std::string& filePath)
{
	std::map<std::string, std::string>	headers;
	struct stat							buffer;

	if (stat(filePath.c_str(), &buffer) != 0) {
		std::cout << "\033[33m" << "Warning: cannot delete: " << filePath << "\033[0m" << std::endl;
		throw HttpStatus(404);
	}
	if (S_ISDIR(buffer.st_mode)) {
		ensureCanDeleteTarget(buffer, filePath, true);
		deleteDirectoryTarget(filePath);
	} else if (S_ISREG(buffer.st_mode) != 0) {
		ensureCanDeleteTarget(buffer, filePath, false);
		deleteFileTarget(filePath);
	} else {
		std::cout << "\033[33m" << "Warning: unsupported target type for delete: " << filePath << "\033[0m" << std::endl;
		throw HttpStatus(403);
	}
	std::cout << "DELETE: " << filePath << std::endl;
	currentClient.generateResponse(204, headers, "");
}
