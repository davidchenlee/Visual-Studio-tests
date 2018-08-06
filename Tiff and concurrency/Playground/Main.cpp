#pragma once
#include "TiffEx.h"
#include "ConcurrencyEx.h"

int main()
{
	std::string inputFilename = "Beads_4um_750nm_50mW_x=35.120_y=19.808_z=18.4285";
	std::string outputFilename = "test";

	try
	{
		myTiff image(inputFilename);
		image.verticalFlip(1);
		image.saveToTiff(2, outputFilename);
	}
	catch (...)
	{
		std::cout << "An error has occurred" << std::endl;
	}

	getchar();
	return 0;
}

