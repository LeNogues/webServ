#include <iostream>

int writeError(std::string str, int returnValue)
{
    std::cout << str << std::endl;
    return (returnValue);
}