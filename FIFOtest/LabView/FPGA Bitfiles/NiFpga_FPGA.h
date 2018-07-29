/*
 * Generated with the FPGA Interface C API Generator 16.0.0
 * for NI-RIO 16.0.0 or later.
 */

#ifndef __NiFpga_FPGA_h__
#define __NiFpga_FPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1600
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_FPGA_Bitfile;
 */
#define NiFpga_FPGA_Bitfile "NiFpga_FPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_FPGA_Signature = "33C81A111058DBC4669FEE63EA57B083";

typedef enum
{
   NiFpga_FPGA_IndicatorU32_NumberofElementstoWrite = 0x1C,
} NiFpga_FPGA_IndicatorU32;

typedef enum
{
   NiFpga_FPGA_ControlBool_Start = 0x1A,
   NiFpga_FPGA_ControlBool_StartTest = 0x16,
} NiFpga_FPGA_ControlBool;

typedef enum
{
   NiFpga_FPGA_ControlI32_Nmax = 0x10,
} NiFpga_FPGA_ControlI32;

typedef enum
{
   NiFpga_FPGA_TargetToHostFifoU16_FIFO = 0,
} NiFpga_FPGA_TargetToHostFifoU16;

#endif
