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

# include <vector>
# include <iostream>
# include <map>
# include <utility>

struct CommonConfig {
    std::string                  _root;
    bool                         _autoIndex;
    std::string                  _defaultFile;
    std::vector<std::string>     _allowedMethods;
    unsigned long                _maxSizeBody;
    std::map<std::string, std::string>      _cgiParams;

    CommonConfig() : _autoIndex(false), _maxSizeBody(1000000) {}
};

struct LocationConfig : public CommonConfig
{
    std::pair<int, std::string>             _redirect;
    std::string                             _uploadStore;
    std::string                             _cgiPass;
    std::string                             _alias;
    std::map<std::string, LocationConfig>   _locations;
};

struct ServerConfig : public CommonConfig
{
    std::vector< std::pair<std::string, int> >  _listenOn;
    std::vector<std::string>                    _serverName;
    std::map<int, std::string>                  _errorPage;
    std::map<std::string, LocationConfig>       _location;
};

#endif