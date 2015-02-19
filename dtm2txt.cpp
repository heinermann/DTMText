#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "Movie.h"
#include "common.h"

Movie::DTMHeader getDTMHeader(std::ifstream &inputFile)
{
	Movie::DTMHeader header = {};
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));
	return header;
}

Movie::ControllerState getControllerState(std::ifstream &inputFile)
{
	Movie::ControllerState controller = {};
	inputFile.read(reinterpret_cast<char*>(&controller), sizeof(controller));
	return controller;
}

int main(int argc, char const * argv[])
{
	if (argc != 3)
	{
		return error("Not enough arguments.");
	}

	std::ifstream inFile(argv[1], std::ios::binary);

	Movie::DTMHeader head = getDTMHeader(inFile);
	if (!std::equal(std::begin(head.filetype), std::end(head.filetype), "DTM\x1A"))
	{
		return error("Not a DTM file.");
	}

	if (head.bWii)
	{
		return error("Wii not supported.");
	}

	if (head.numControllers != 1)
	{
		return error("Only one controller supported.");
	}

	std::ofstream outFile(argv[2]);
	outFile << std::boolalpha;
	head.save(outFile);
	outFile << "\n";
	for (unsigned i = 0; i < head.inputCount && !inFile.bad(); ++i)
	{
		Movie::ControllerState controllerState = getControllerState(inFile);
		if (!controllerState.isDefault())
		{
			outFile << i << ": ";
			controllerState.save(outFile);
			outFile << "\n";
		}
	}

	return 0;
}

