/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*                        webServ                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   this is just                                   +#+  +:+       +#+        */
/*   to fill it up                                +#+#+#+#+#+   +#+           */
/*   so it look                                        #+#    #+#             */
/*   ✨ aestheticaly pleasing ✨                      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <map>
# include <vector>
# include <utility>
# include <iostream>

struct CommonConfig {
    std::pair<int, std::string>             _redirect;
    std::map<int, std::string>              _errorPage;
    std::map<std::string, std::string>      _cgiParams;
    std::vector<std::string>                _allowedMethods;
    std::pair<std::string, std::string>     _alias;
    std::string                             _root;
    std::string                             _defaultFile;
    unsigned long                           _maxSizeBody;
    bool                                    _autoIndex;

    CommonConfig() : _alias(std::make_pair("", "")), _maxSizeBody(1000000), _autoIndex(false) {}
};

struct LocationConfig : public CommonConfig
{
    std::map<std::string, LocationConfig>   _locations;
};

struct ServerConfig : public CommonConfig
{
    std::map<std::string, LocationConfig>       _location;
    std::pair<std::string, int>                 _listenOn;
    std::vector<std::string>                    _serverName;
};

#endif
