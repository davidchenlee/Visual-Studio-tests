/* The following examples are from http://research.cs.wisc.edu/graphics/Courses/638-f1999/libtiff_tutorial.htm */

#include "TiffEx.h"

myTiff::myTiff(std::string filename): mImage(300 * 800)
{
	TIFF *tiffHandle = TIFFOpen((foldername + filename + ".tif").c_str(), "r");

	if (tiffHandle == nullptr)
		throw std::runtime_error("Opening Tiff failed");

	TIFFGetField(tiffHandle, TIFFTAG_IMAGEWIDTH, &mWidth_pix);
	TIFFGetField(tiffHandle, TIFFTAG_IMAGELENGTH, &mHeight_pix);
	TIFFGetField(tiffHandle, TIFFTAG_ROWSPERSTRIP, &mStripSize_pix);

	std::cout << "Width = " << mWidth_pix << "\n";
	std::cout << "Height = " << mHeight_pix << "\n";
	std::cout << "Strip size = " << mStripSize_pix << "\n";

	int bytesPerLine = mWidth_pix;			//Length in memory of one row of pixel in the image.
	unsigned char* buffer = (unsigned char *)_TIFFmalloc(bytesPerLine);
	

	if (buffer == NULL) // check the raster's memory was allocaed
	{
		TIFFClose(tiffHandle);
		std::runtime_error("Could not allocate memory for raster of TIFF image");
	}

	//Now writing image to the file one strip at a time
	for (int rowIndex = 0; rowIndex < mHeight_pix; rowIndex++)
	{
		if (TIFFReadScanline(tiffHandle, buffer, rowIndex, 0) < 0)
			break;
		memcpy(&mImage[(mHeight_pix - rowIndex - 1)*bytesPerLine], buffer, bytesPerLine);    // check the index here, and figure why not using h*bytesPerLine

	}

	//release the memory
	_TIFFfree(buffer);

	//close the tif file
	TIFFClose(tiffHandle);
}

myTiff::~myTiff()
{
}


void myTiff::saveToTiff(std::string filename)
{
	TIFF *tiffHandle = TIFFOpen((foldername + filename + ".tif").c_str(), "w");

	if (tiffHandle == nullptr)
		throw std::runtime_error((std::string)__FUNCTION__ + ": Saving Tiff failed");

	//TAGS
	TIFFSetField(tiffHandle, TIFFTAG_IMAGEWIDTH, mWidth_pix);										//Set the width of the image
	TIFFSetField(tiffHandle, TIFFTAG_IMAGELENGTH, mHeight_pix);										//Set the height of the image
	TIFFSetField(tiffHandle, TIFFTAG_SAMPLESPERPIXEL, 1);											//Set number of channels per pixel
	TIFFSetField(tiffHandle, TIFFTAG_BITSPERSAMPLE, 8);												//Set the size of the channels
	TIFFSetField(tiffHandle, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);								//Set the origin of the image. Many readers ignore this tag (ImageJ, Windows preview, etc...)
	//TIFFSetField(tiffHandle, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);							//PLANARCONFIG_CONTIG (for example, RGBRGBRGB) or PLANARCONFIG_SEPARATE (R, G, and B separate)
	TIFFSetField(tiffHandle, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);							//Single channel with min as black				
	TIFFSetField(tiffHandle, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiffHandle, mWidth_pix));	//Set the strip size of the file to be size of one row of pixels

	int bytesPerLine = mWidth_pix;			//Length in memory of one row of pixel in the image.
	unsigned char *buffer = nullptr;		//Buffer used to store the row of pixel information for writing to file

	//Allocating memory to store pixels of current row
	if (TIFFScanlineSize(tiffHandle))
		buffer = (unsigned char *)_TIFFmalloc(bytesPerLine);
	else
		buffer = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tiffHandle));

	//Now writing image to the file one strip at a time
	for (int row = 0; row < mHeight_pix; row++)
	{
		memcpy(buffer, &mImage[(mHeight_pix - row - 1)*bytesPerLine], bytesPerLine);    // check the index here, and figure out why not using h*bytesPerLine
		if (TIFFWriteScanline(tiffHandle, buffer, row, 0) < 0)
			break;
	}

	//Close the output file
	(void)TIFFClose(tiffHandle);

	//Destroy the buffer
	if (buffer)
		_TIFFfree(buffer);

	std::cout << "File successfully saved" << std::endl;
}


/*
int WriteTiffEx(void)
{
uint32 width = 640;
uint32 height = 640;

TIFF *out = TIFFOpen("new.tif", "w");

//4 channels: RGBA
int sampleperpixel = 4;

//create an matrix representing the image
unsigned char *image = new unsigned char[width*height*sampleperpixel];

//initialize the matrix
for (int ii = 0; ii < width*height*sampleperpixel; ii++)
image[ii] = 0;

//set alpha to the max
for (int ii = 3; ii < width*height*sampleperpixel; ii += 4)
image[ii] = 255;

for (int ii = 0; ii < width*height*sampleperpixel; ii += 4)
image[ii] = 255. / (width*height)*ii / 4;


//TAGS
TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);					// set the width of the image
TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);					// set the height of the image
TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);		// set number of channels per pixel
TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);					// set the size of the channels
TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
//   Some other essential fields to set that you do not have to understand for now.
TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);


tsize_t linebytes = sampleperpixel * width;						// length in memory of one row of pixel in the image.
unsigned char *buf = NULL;										// buffer used to store the row of pixel information for writing to file

// Allocating memory to store the pixels of current row
if (TIFFScanlineSize(out))
buf = (unsigned char *)_TIFFmalloc(linebytes);
else
buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(out));

// set the strip size of the file to be size of one row of pixels
TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(out, width*sampleperpixel));

//Now writing image to the file one strip at a time
for (uint32 row = 0; row < height; row++)
{
memcpy(buf, &image[(height - row - 1)*linebytes], linebytes);    // check the index here, and figure out why not using h*linebytes
if (TIFFWriteScanline(out, buf, row, 0) < 0)
break;
}

//close the output file
(void)TIFFClose(out);

//destroy the buffer
if (buf)
_TIFFfree(buf);

return 0;
}


//READ FILE EXAMPLE
int ReadTiffEx(void)
{
	TIFF *tif = TIFFOpen("D:\\OwnCloud\\Codes\\Cpp playground\\Playground\\marbles.tif", "r");

	//define uint32 unsigned long
	uint32 width, height;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);        // uint32 height;

	std::cout << "Width = " << width << "\n";
	std::cout << "Height = " << height << "\n";

	uint32 nPixels = width*height;
	uint32* raster = (uint32 *)_TIFFmalloc(nPixels * sizeof(uint32));

	if (raster == NULL) // check the raster's memory was allocaed
	{
		TIFFClose(tif);
		cerr << "Could not allocate memory for raster of TIFF image" << endl;
		return -1;
	}

	// Check the tif read to the raster correctly
	if (!TIFFReadRGBAImage(tif, width, height, raster, 0))
	{
		TIFFClose(tif);
		cerr << "Could not read raster of TIFF image" << endl;
		return -1;
	}

	//read a pixel. The channels are RGBA
	unsigned char X = (unsigned char)TIFFGetA(raster[0]);

	std::cout << +X << "\n";

	//release the memory
	_TIFFfree(raster);

	//close the tif file
	TIFFClose(tif);

	return 0;
}



//WRITE FILE EXAMPLE

*/