/*
This code tests the target-to-host (FPGA-to-PC) FIFO communication. A host-vi was created in LV besides the present C++ code
*/
#include "NiFpga_Main.h"
#include "NiFpga.c"
#include <iostream>
#include <fstream>					//For std::ofstream

/*Define the full path of the bitfile*/
static const char* Bitfile = "D:\\OwnCloud\\Codes\\Visual-Studio-tests\\FIFOtest\\LabView\\FPGA Bitfiles\\" NiFpga_Main_Bitfile;

const int nPix = 100;
const std::string foldername = ".\\";

int main()
{
	//FOR SAVING TEXT FILE
	std::ofstream fileHandle;						//Create output file
	fileHandle.open(foldername + "output.txt");		//Open the file


	//Must be called before any other FPGA calls
	NiFpga_Status status = NiFpga_Initialize();
	std::cout << "FPGA initialize status: " << status << "\n";

	//Check for any FPGA error
	if (NiFpga_IsNotError(status))
	{
		NiFpga_Session session;

		//Opens a session, downloads the bitstream, but does not run the FPGA
		NiFpga_MergeStatus(&status, NiFpga_Open(Bitfile, NiFpga_Main_Signature, "RIO0", 1, &session)); //1=no run, 0=run
		std::cout << "FPGA open-session status: " << status << "\n";

		if (NiFpga_IsNotError(status))
		{
			//Run the FPGA application
			NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));
			std::cout << "Run FPGA status: " << status << "\n";

			//Initialize. set the control flags to LOW
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 0));
			

			//FPGA****************************************************************************************************
			
			//set the number of elements to push into the FIFO on the FPGA side. I have implemented a 'for loop' on the FPGA side
			NiFpga_MergeStatus(&status, NiFpga_WriteI32(session, NiFpga_Main_ControlU32_Nmax, nPix));



			//HOST**********************************************************
			uint32_t nRemaining = 0; //elements remaining
			size_t timeout = 100;
			uint32_t* bufArray = new uint32_t[nPix]();
			int nElemRead = 0;
			int timeoutCounter_iter = 10;
			uint32_t* dummyArray = new uint32_t[0];
			const int readWaitingTime_ms = 20;


			//Start up the host FIFO. No needed for reading the data, but it takes about 3ms to read the number of elements remaining 'r' once the FIFO starts running
			NiFpga_MergeStatus(&status, NiFpga_StartFifo(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT));
			//Sleep(20);


			uint32_t ptr_index = 0;
			uint32_t dummy = 0;

			//start the sequence with a pulse
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 1));
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 0));

			//Read the data as it comes
			while (nElemRead < nPix)
			{
				Sleep(readWaitingTime_ms);

				//By reading 0 elements, this function returns the number of elements queued in the host FIFO
				NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, dummyArray, 0, timeout, &nRemaining));
				std::cout << "Number of elements remaining in the host FIFO: " << nRemaining << "\n";

				if (nRemaining > 0)
				{
					nElemRead += nRemaining;
					NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, bufArray+ptr_index, nRemaining, timeout, &dummy));

					ptr_index += nRemaining;
				}

				timeoutCounter_iter--;

				//Transfer timeout
				if (timeoutCounter_iter == 0)
				{
					std::cout << "Timeout" << std::endl;
					getchar();
					break;
				}
			}//While

			std::cout << "Total of elements read: " << nElemRead << std::endl; //Print out the total number of elements read


			for (int ii = 0; ii < nPix; ii++)
				fileHandle << (int)bufArray[ii] << std::endl;		//Write each element

			delete[] bufArray;
			delete[] dummyArray;

		}//if


		 //Close the session
		NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
		std::cout << "Close session status: " << status << "\n";
	}

	//Must be called after all other calls
	NiFpga_MergeStatus(&status, NiFpga_Finalize());
	std::cout << "FPGA finalize status: " << status << "\n";

	fileHandle.close();		//Close the txt file
	getchar();
	return 0;
}


/*
//Read how many free spots are available in the FIFO
uint32_t Nfree;
NiFpga_MergeStatus(&status, NiFpga_ReadU32(session, NiFpga_Main_IndicatorU32_NumberofElementstoWrite, &Nfree));
std::cout << "Number of spots free in the FPGA FIFO: " << Nfree << "\n";
*/