#include <iostream>

const std::string WHITESPACE = " \n\r\t\f\v";

void ltrim(std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	if (start == std::string::npos)
		s.clear();
	else
		s.erase(0, start);
}

void rtrim(std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	if (end == std::string::npos)
		s.clear();
	else
		s.erase(end + 1);
}

void btrim(std::string& s)
{
	rtrim(s);
	ltrim(s);
}
