#pragma once
#include <iostream>
#include "TiffEx.h"
#include "ConcurrencyEx.h"
#include <Windows.h>	//ShellExecute
using namespace std;

int main()
{
	//ReadTiffEx();
	//WriteTiffEx();
	ThreadEx();
	getchar();

	//"double click" on the specified file
	ShellExecute(NULL, "open", "D:\\OwnCloud\\Codes\\Visual-Studio-tests\\MatlabAPI\\x64\\Release\\MatlabAPI.exe", NULL, NULL, SW_SHOWDEFAULT);

	return 0;
}

