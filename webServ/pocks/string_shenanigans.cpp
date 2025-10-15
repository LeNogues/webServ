/* ************************************************************************** */
/*                                                                            */
/*                                                         :::     ::::::::   */
/*   string_shenanigans.cpp                              :+:     :+:    :+:   */
/*                                                     +:+ +:+        +:+     */
/*   By: SeeTwoo <waltibee@gmail.com>                +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created:                                           #+#    #+#            */
/*   Uptated:                                          ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>

/*int	main(void) {
	std::string		body;
	std::string		line;
	std::ifstream	in("string_shenanigans.cpp");

	if (!in)
		return (1);
	while (std::getline(in, line)) {
		body += line;
		body += "\n";
	}
	std::cout << body << std::endl;
	return (0);
}
*/


int	main(int ac, char **av) {
	if (ac != 2)
		return (1);
	std::string	body;
	char		buffer[1024];
	int			fd = open((av[1]), O_RDONLY);
	int			bytes_read;

	if (fd == -1)
		return (1);
	do {
		bytes_read = read(fd, buffer, 1023);
		std::string	append(buffer, &buffer[bytes_read]);
		body += append;
	} while (bytes_read > 0);
	std::cout << body << std::endl;
	return (0);
}
