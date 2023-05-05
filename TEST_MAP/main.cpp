/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpauline <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 15:27:40 by jpauline          #+#    #+#             */
/*   Updated: 2023/05/02 15:57:23 by jpauline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>

int foisdeux(int x)
{	return (2*x);}
int	foistrois(int x)
{	return (3*x);}
int foisdix(int x)
{	return (10*x);}

int main()
{
	using func = int (*)(int);

	std::map<std::string,func> funcMap;
	funcMap["deux"] = &foisdeux;
	funcMap["trois"] = &foistrois;
	funcMap["dix"] = &foisdix;
	
	std::cout << "TEST:" << std::endl;
	std::cout << "deux fois 4 = " << (*(funcMap["deux"]))(4) << std::endl;
	std::cout << "trois fois 4 = " << (*(funcMap["trois"]))(4) << std::endl;
	std::cout << "dix fois 4 = " << (*(funcMap["dix"]))(4) << std::endl;
	
	return 0;
	
}
