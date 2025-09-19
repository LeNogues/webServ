#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <map>
# include <list>
# include <fstream>
# include <sstream>
# include "ServeurConfig.hpp"
# include "../trim/trim.hpp"

class Config
{
    private:
            std::vector<ServerConfig>   _servers;
    public:
            #pragma region Cannonical Class

            Config(const std::string& configFilePath);
            ~Config();
            Config(const Config& other);
            Config& operator=(const Config& other);

            #pragma endregion


            #pragma region Getter/Setter
            
            std::vector<ServerConfig>& getServers();
            
            #pragma endregion

            #pragma region MemberFunction

            void    parseConfig(const std::string& configFilePath);
            
            #pragma endregion

};

#endif