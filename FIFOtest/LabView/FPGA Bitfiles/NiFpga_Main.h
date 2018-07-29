/*
 * Generated with the FPGA Interface C API Generator 16.0.0
 * for NI-RIO 16.0.0 or later.
 */

#ifndef __NiFpga_Main_h__
#define __NiFpga_Main_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1600
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_Main_Bitfile;
 */
#define NiFpga_Main_Bitfile "NiFpga_Main.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_Main_Signature = "C8F11DDFE567825E1B5511BDDBF5CA26";

typedef enum
{
   NiFpga_Main_IndicatorU32_NumberofElementstoWrite = 0x18,
} NiFpga_Main_IndicatorU32;

typedef enum
{
   NiFpga_Main_ControlBool_Start = 0x16,
} NiFpga_Main_ControlBool;

typedef enum
{
   NiFpga_Main_ControlI32_Nmax = 0x10,
} NiFpga_Main_ControlI32;

typedef enum
{
   NiFpga_Main_TargetToHostFifoU32_FIFOOUT = 0,
} NiFpga_Main_TargetToHostFifoU32;

#endif
