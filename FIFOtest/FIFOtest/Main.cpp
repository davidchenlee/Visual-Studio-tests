/*
This code tests the target-to-host (FPGA-to-PC) FIFO communication. A host-vi was created in LV as well as an alternative to this C++ code
*/



#include "NiFpga_FPGA.h"
#include "NiFpga.c"
#include <iostream>

/*Define the full path of the bitfile*/
static const char* Bitfile = "D:\\OwnCloud\\Codes\\Visual-Studio-tests\\FIFOtest\\LabView\\FPGA Bitfiles\\" NiFpga_FPGA_Bitfile;


int main()
{
	/* must be called before any other FPGA calls */
	NiFpga_Status status = NiFpga_Initialize();
	std::cout << "FPGA initialize status: " << status << "\n";

	/* check for any FPGA error */
	if (NiFpga_IsNotError(status))
	{
		NiFpga_Session session;

		/* opens a session, downloads the bitstream, but does not run the FPGA */
		NiFpga_MergeStatus(&status, NiFpga_Open(Bitfile, NiFpga_FPGA_Signature, "RIO0", 1, &session)); //1=no run, 0=run
		std::cout << "FPGA open-session status: " << status << "\n";

		if (NiFpga_IsNotError(status))
		{
			/* run the FPGA application */
			NiFpga_MergeStatus(&status, NiFpga_Run(session, 0));
			std::cout << "Run FPGA status: " << status << "\n";


			//initialize. set the control flags to LOW
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_Start, 0));
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_StartTest, 0));
			



			//FPGA****************************************************************************************************
			
			size_t Npush = 10; //set the number of elements to push into the FIFO on the FPGA side. I have implemented a 'for loop' on the FPGA side
			NiFpga_MergeStatus(&status, NiFpga_WriteI32(session, NiFpga_FPGA_ControlI32_Nmax, Npush));


			//start the sequence and set the control back to LOW
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_Start, 1)); 
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_Start, 0));
			
			//Read how many free spots are available in the FIFO
			uint32_t Nfree;
			NiFpga_MergeStatus(&status, NiFpga_ReadU32(session, NiFpga_FPGA_IndicatorU32_NumberofElementstoWrite, &Nfree));
			std::cout << "Number of spots free in the FPGA FIFO: " << Nfree << "\n";
			
			//NiFpga_MergeStatus(&status, NiFpga_StartFifo(session, NiFpga_FPGA_TargetToHostFifoU16_FIFO));
			//NiFpga_MergeStatus(&status, NiFpga_StopFifo(session, NiFpga_FPGA_TargetToHostFifoU8_FIFO));



			//HOST**********************************************************
			size_t Npop = 6;
			uint32_t r; //elements remaining
			size_t timeout = 100;
			uint16_t* data = new uint16_t[Npop];
			for (int ii = 0 ; ii < Npop; ii++)
				data[ii] = 0;

			//Start up the host FIFO. Not needed for reading the data, but it takes about 3ms to read 'elementsRemaining' once the FIFO starts running.
			NiFpga_MergeStatus(&status, NiFpga_StartFifo(session, NiFpga_FPGA_TargetToHostFifoU16_FIFO));
			Sleep(10);

		    //By reading 0 elements, this function returns the number of elements queued in the host FIFO
			NiFpga_MergeStatus(&status, NiFpga_ReadFifoU16(session, NiFpga_FPGA_TargetToHostFifoU16_FIFO, data, 0, -1, &r));
			std::cout << "Number of elements remaining in the host FIFO: " << r << "\n";

			// read the DMA FIFO data and print. This function alone is able to start up the FIFO, but it would not read 'elementsRemaining' right away because it takes about 3ms to read 'elementsRemaining' once the FIFO starts running
			NiFpga_MergeStatus(&status, NiFpga_ReadFifoU16(session, NiFpga_FPGA_TargetToHostFifoU16_FIFO, data, Npop, timeout, &r));
			for(int ii=0;ii<Npop;ii++)
				std::cout << "Data: " << data[ii]<< "\n";
				
			std::cout << "Number of elements remaining in the host FIFO: " << r << "\n";
				


			//debugging signals
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_StartTest, 1));
			Sleep(1);
			NiFpga_MergeStatus(&status, NiFpga_WriteBool(session, NiFpga_FPGA_ControlBool_StartTest, 0));


			/* close the session */
			NiFpga_MergeStatus(&status, NiFpga_Close(session, 0));
			std::cout << "Close session status: " << status << "\n";
		}

		/* must be called after all other calls */
		NiFpga_MergeStatus(&status, NiFpga_Finalize());
		std::cout << "FPGA finalize status: " << status << "\n";


		getchar();

		return 0;
	}
}