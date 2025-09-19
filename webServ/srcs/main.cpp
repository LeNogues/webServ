#include "../includes/config/Config.hpp"
#include "../includes/error/writeError.hpp"


int main(int argc, char **argv)
{
    if (argc != 2)
        return (writeError("ERROR: wrong number of argument", 1));
    try
    {
        Config allServeur(argv[1]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return (1);
    }
    return (0);
}