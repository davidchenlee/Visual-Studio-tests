#pragma once
#include <tiffio.h>
#include <iostream>
#include <vector>

const std::string foldername = "D:\\OwnCloud\\Data\\_output_D\\";

//int ReadTiffEx(void);
//int WriteTiffEx(void);

class myTiff
{
	std::vector<unsigned char> mImage;
	int mWidth_pix;
	int mHeight_pix;
	int mStripSize_pix;
public:
	myTiff(std::string filename);
	~myTiff();
	void saveToTiff(std::string filename);
};