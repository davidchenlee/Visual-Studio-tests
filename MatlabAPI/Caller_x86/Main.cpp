#pragma once
#include <iostream>
#include <Windows.h>	//ShellExecute
//using namespace std;

int main()
{
	//Execute the Matlab wrapper
	ShellExecute(NULL, "open", "D:\\OwnCloud\\Codes\\Visual-Studio-tests\\MatlabAPI\\x64\\Release\\MatlabAPI.exe", NULL, NULL, SW_SHOWDEFAULT);
	std::cout << "test\n";
	getchar();

	return 0;
}

