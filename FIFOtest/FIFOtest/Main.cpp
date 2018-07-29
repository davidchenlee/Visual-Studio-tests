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
			
			size_t Npush = 10; //set the number of elements to push into the FIFO on the FPGA side. I have implemented a 'for loop' on the FPGA side
			NiFpga_MergeStatus(&status, NiFpga_WriteI32(session, NiFpga_Main_ControlI32_Nmax, Npush));

			//start the sequence and set the control back to LOW
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 1)); 
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_Main_ControlBool_Start, 0));
			
			//Read how many free spots are available in the FIFO
			uint32_t Nfree;
			NiFpga_MergeStatus(&status, NiFpga_ReadU32(session, NiFpga_Main_IndicatorU32_NumberofElementstoWrite, &Nfree));
			std::cout << "Number of spots free in the FPGA FIFO: " << Nfree << "\n";
			

			//HOST**********************************************************
			int Npop = 6;
			uint32_t r; //elements remaining
			size_t timeout = 100;
			uint32_t* data = new uint32_t[Npop];
			for (int ii = 0 ; ii < Npop; ii++)
				data[ii] = 0;

			//Start up the host FIFO. No needed for reading the data, but it takes about 3ms to read the number of elements remaining 'r' once the FIFO starts running
			NiFpga_MergeStatus(&status, NiFpga_StartFifo(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT));
			Sleep(10);

		    //By reading 0 elements, this function returns the number of elements queued in the host FIFO
			NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, data, 0, -1, &r));
			std::cout << "Number of elements remaining in the host FIFO: " << r << "\n";

			//Read the DMA FIFO data and print. This function alone is able to start up the FIFO, but it would not read 'r' right away
			//because it takes about 3ms to read it once the FIFO starts running
			NiFpga_MergeStatus(&status, NiFpga_ReadFifoU32(session, NiFpga_Main_TargetToHostFifoU32_FIFOOUT, data, Npop, timeout, &r));
			for (int ii = 0; ii<Npop; ii++)
				std::cout << "Data: " << data[ii]<< "\n";
				
			std::cout << "Number of elements remaining in the host FIFO: " << r << "\n";
				

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