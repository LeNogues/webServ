#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <vector>
# include <iostream>
# include <map>

struct LocationConfig
{
    std::vector<std::string>                    _allowedMethods;
    std::pair<int, std::string>                 _redirect; //<code, URL>
    std::string                                 _root;
    bool                                        _autoIndex;
    std::string                                 _defaultFile; //surement index.html
    std::string                                 _uploadStore;
};

struct ServerConfig
{
    std::vector< std::pair<std::string, int> >  _listenOn; //<host, port>
    std::vector<std::string>                    _serverName;
    std::map<int, std::string>                  _errorPage;  //<code, chemin d acces>
    unsigned long                               _maxSizeBody;
    std::map<std::string, LocationConfig>       _location; // <chemin d acces, location>
};

#endif