#pragma once
#include <string>
#include <iostream>

/**
 * Retrieves a line, ignoring everything after the # symbol.
 * Also removes leading/trailing whitespace.
 */
inline std::istream & getCommentLine(std::istream & is, std::string & s)
{
	std::getline(is, s);
	s = s.substr(0, s.find_first_of('#'));

	// trim leading spaces
	const size_t spaceBegin = s.find_first_not_of(" \t\r\n");
	if (spaceBegin != std::string::npos)
	{
		s = s.substr(spaceBegin);
	}

	// trim trailing spaces
	s = s.substr(0, s.find_last_not_of(" \t\r\n")+1);

	return is;
}

inline int showHelp()
{
	std::cout << "Usage: appname inputFileName outputFileName" << std::endl;
	return 0;
}

inline int error(char const * errString)
{
	std::cerr << "ERROR: " << errString << std::endl;
	showHelp();
	return 1;
}

