/*
This code tests the target-to-host (FPGA-to-PC) FIFO communication. A host-vi was created in LV besides the present C++ code
*/
#include "NiFpga_Main.h"
#include "NiFpga.c"
#include <iostream>

/*Define the full path of the bitfile*/
static const char* Bitfile = "D:\\OwnCloud\\Codes\\Visual-Studio-tests\\FIFOtest\\LabView\\FPGA Bitfiles\\" NiFpga_Main_Bitfile;


int main()
{
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
			
			int nPixAllFrames = 120000; //set the number of elements to push into the FIFO on the FPGA side. I have implemented a 'for loop' on the FPGA side
			NiFpga_MergeStatus(&status, NiFpga_WriteI32(session, NiFpga_Main_ControlU32_Nmax, nPixAllFrames));



			for (int ii = 0; ii < 100; ii++)
			{
				std::cout << "Iteration: " << ii + 1 << std::endl;


				//HOST**********************************************************
				uint32_t NremainFIFO = 0; //elements remaining
				size_t timeout = 100;
				uint32_t* BufArray = new uint32_t[nPixAllFrames];
				int NelemReadFIFO = 0;
				int TimeoutCounter_iter = 100;
				uint32_t* dummy = new uint32_t[0];
				const int ReadFifoWaitingTime_ms = 10;


				//Start up the host FIFO. No needed for reading the data, but it takes about 3ms to read the number of elements remaining 'r' once the FIFO starts running
				NiFpga_MergeStatus(&status, NiFpga_StartFifo(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT));
				//Sleep(20);

				//start the sequence with a pulse
				NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 1));
				NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 0));

				//Read the data as it comes
				while (NelemReadFIFO < nPixAllFrames)
				{
					Sleep(ReadFifoWaitingTime_ms);

					//By reading 0 elements, this function returns the number of elements queued in the host FIFO
					NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, dummy, 0, timeout, &NremainFIFO));
					std::cout << "Number of elements remaining in the host FIFO: " << NremainFIFO << "\n";

					if (NremainFIFO > 0)
					{
						NelemReadFIFO += NremainFIFO;
						//Read the DMA FIFO data. This function alone is able to start up the FIFO, but it would not read 'r' right away
						//because it takes about 3ms to read it once the FIFO starts running
						NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, BufArray, NremainFIFO, timeout, &NremainFIFO));
						std::cout << "Number of elements remaining in the host FIFO: " << NremainFIFO << "\n";
					}

					TimeoutCounter_iter--;

					//Transfer timeout
					if (TimeoutCounter_iter == 0)
					{
						getchar();
						break;
					}
				}//While

				delete[] BufArray;
				delete[] dummy;

			}//For loop


			//Close the session
			NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
			std::cout << "Close session status: " << status << "\n";
		}

		//Must be called after all other calls
		NiFpga_MergeStatus(&status, NiFpga_Finalize());
		std::cout << "FPGA finalize status: " << status << "\n";

		getchar();

		return 0;
	}
}

/*
//Read how many free spots are available in the FIFO
uint32_t Nfree;
NiFpga_MergeStatus(&status, NiFpga_ReadU32(session, NiFpga_Main_IndicatorU32_NumberofElementstoWrite, &Nfree));
std::cout << "Number of spots free in the FPGA FIFO: " << Nfree << "\n";
*/