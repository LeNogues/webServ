#include "../../includes/utils/strToSizeT.hpp"

bool strToSizeT(const std::string& str, size_t& size, int base)
{
	char*	end;
	errno = 0;
	unsigned long	val;

	if (str.empty())
		return false;
	val = std::strtoull(str.c_str(), &end, base);
	if (end == str.c_str() || *end != '\0'
		|| (val == ULONG_MAX && errno == ERANGE))
		return false;
	if (val > static_cast<unsigned long>(static_cast<size_t>(-1)))
		return false;
	size = static_cast<size_t>(val);
	return true;
}
