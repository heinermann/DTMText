#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "Movie.h"
#include "common.h"

int main(int argc, char const * argv[])
{
	if (argc != 3)
	{
		return error("Not enough arguments.");
	}

	std::ifstream inFile(argv[1]);
	inFile >> std::boolalpha;

	Movie::DTMHeader head;
	head.load(inFile);

	Movie::ControllerState neutralInput = {0};
	neutralInput.AnalogStickX = 128;
	neutralInput.AnalogStickY = 128;
	neutralInput.CStickX = 128;
	neutralInput.CStickY = 128;

	u64 currentFrame = 0;

	std::string s;
	std::vector<Movie::ControllerState> inputs;
	while (getCommentLine(inFile, s))
	{
		if (s.empty()) continue;
		size_t splitPos = s.find_first_of(':');
		u64 frameNo = std::stoull(s.substr(0, splitPos));

		std::istringstream ss(s.substr(splitPos+1));
		Movie::ControllerState input;
		input.load(ss);
		
		// Ignore non-increasing frame numbers
		if (currentFrame <= frameNo)
		{
			inputs.insert(inputs.end(), frameNo - currentFrame, neutralInput);
			inputs.push_back(input);
			currentFrame = frameNo+1;
		}
	}

	head.inputCount = std::max(head.inputCount, currentFrame);
	inputs.insert(inputs.end(), head.inputCount - currentFrame, neutralInput);

	std::ofstream outFile(argv[2], std::ios::binary);
	outFile.write(reinterpret_cast<char*>(&head), sizeof(head));
	outFile.write(reinterpret_cast<char*>(inputs.data()), inputs.size() * sizeof(decltype(inputs)::value_type));

	return 0;
}

