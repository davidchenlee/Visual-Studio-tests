#include <chrono>
#include <iostream>
#include <stdio.h>
#include <vector>

class A
{
	const int mNpix;
	std::vector<uint32_t> mVectorU32;
	uint32_t* mVectorPointer;
public:
	A(const int nPix);
	~A();
	void test();
	void test2();
};

A::A(const int nPix): mNpix(nPix), mVectorU32(nPix)
{
	mVectorPointer = new uint32_t[nPix]();
}

A::~A()
{
	delete[] mVectorPointer;
}

void A::test()
{
	//std::vector<unsigned char> vectorChar(mNpix);

	for (int pixIndex = 0; pixIndex < mNpix; pixIndex++)
	{
		//mVectorU32[pixIndex];
		mVectorPointer[pixIndex];
		//vectorChar[pixIndex] = upscaled;
	}
}


void A::test2()
{
	//std::vector<unsigned char> vectorChar(mNpix);

	for (int pixIndex = 0; pixIndex < mNpix; pixIndex++)
	{
		mVectorU32[pixIndex];
		//mVectorPointer[pixIndex];
		//vectorChar[pixIndex] = upscaled;
	}
}


void main()
{

	const int nPix = 300 * 400 * 10;

	A a(nPix);

	//Declare and start a stopwatch
	double duration;
	auto t_start = std::chrono::high_resolution_clock::now();
	a.test();
	//Stop the stopwatch
	duration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t_start).count();


	//Declare and start a stopwatch
	double duration2;
	t_start = std::chrono::high_resolution_clock::now();
	a.test2();
	//Stop the stopwatch
	duration2 = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t_start).count();



	std::cout << "Elapsed time: " << duration2/duration << " ms" << std::endl;
	getchar();


}

/*
//When multiplexing later on, each U32 element in bufArray_B must be deMux in 8 segments of 4-bits each
void Image::demux_()
{
	for (int pixIndex = 0; pixIndex < mRTsequence.mNpixAllFrames; pixIndex++)
	{
		unsigned char upscaled = mRTsequence.mUpscaleU8 * mBufArrayB[pixIndex]; //Upscale the buffer from 4-bit to 8-bit

																				//If upscaled overflows
		if (upscaled > _UI8_MAX)
			upscaled = _UI8_MAX;

		mImage[pixIndex] = upscaled;
	}
}
*/