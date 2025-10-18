/* ************************************************************************** */
/*                                                                            */
/*                                                         :::     ::::::::   */
/*   extensions.cpp                                      :+:     :+:    :+:   */
/*                                                     +:+ +:+        +:+     */
/*   By: SeeTwoo <waltibee@gmail.com>                +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created:                                           #+#    #+#            */
/*   Uptated:                                          ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

int	main() {
	std::string	path = "text.txt";

	size_t	pos = path.find_last_of('.');
	std::string extension = (pos == std::string::npos) ? "" : path.substr(pos + 1);
	std::cout << extension << '\n';
}
