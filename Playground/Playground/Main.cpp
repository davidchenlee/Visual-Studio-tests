/*
Create a process to open a file in ImageJ
CreateProcess example from: https://stackoverflow.com/questions/32926272/beginner-c-createprocess-error-2
ImageJ command line options: https://imagej.nih.gov/ij/docs/install/windows.html

*/

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
//#include <string>
using namespace std;


void main()
{

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);

/*
	if (argc != 2)
	{
		printf("Usage: %s [cmdline]\n", argv[0]);
		return;
	}
	*/

	//Run a macro: the first path is for the ImageJ API, the second path for the ImageJ macro
	//LPSTR command = "java -jar \"C:\\Users\\chen\\Desktop\\ImageJ-win64\\jars\\ij-1.51w.jar\" -macro C:\\Users\\chen\\Desktop\\ImageJ-win64\\macros\\macroTest.ijm";

	//Open a file: the first path is for the ImageJ API, the second path for the file
	LPSTR command = "java -jar \"C:\\Users\\chen\\Desktop\\ImageJ-win64\\jars\\ij-1.51w.jar\" C:\\Users\\chen\\Desktop\\tiffExample.tif";

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		command,        // Command line
		NULL,				// Process handle not inheritable
		NULL,				// Thread handle not inheritable
		FALSE,				// Set handle inheritance to FALSE
		CREATE_NO_WINDOW,	// Creation flags
		NULL,				// Use parent's environment block
		NULL,				// Use parent's starting directory 
		&si,				// Pointer to STARTUPINFO structure
		&pi)				// Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		//getchar();
		return;
	}
	//getchar();

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	cout << "test" << endl;

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);


}
