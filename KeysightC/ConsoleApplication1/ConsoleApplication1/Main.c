/*
*DC: Modified code from Keysight's programmer's guide p1471
*
* Keysight VISA Example in C
* ------------------------------------------------------------------
* This program illustrates a few commonly-used programming
* features of your Keysight oscilloscope.
*/

#include <stdio.h>												/* For printf(). */
#include <string.h>												/* For strcpy(), strcat(). */
#include <time.h>												/* For clock(). */
#include <visa.h>												/* Keysight VISA routines. */
#include <stdlib.h>												/* For exit()?*/

#define VISA_ADDRESS "USB0::0x0957::0x17BC::MY54100539::0::INSTR"
#define IEEEBLOCK_SPACE 5000000

/* Function prototypes */
void initialize(void);											/* Initialize to known state. */
void capture(void);												/* Capture the waveform. */
void analyze(void);												/* Analyze the captured waveform. */
void do_command(char *command);									/* Send command. */
int do_command_ieeeblock(char *command, int num_bytes);			/* Command w/IEEE block. */
void do_query_string(char *query);								/* Query for string. */
void do_query_number(char *query);								/* Query for number. */
void do_query_numbers(char *query);								/* Query for numbers. */
int do_query_ieeeblock(char *query);							/* Query for IEEE block. */
void check_instrument_errors();									/* Check for inst errors. */
void error_handler();											/* VISA error handler. */

																/* Global variables */
ViSession defaultRM, vi;										/* Device session ID. */
ViStatus err;													/* VISA function return value. */
char str_result[256] = { 0 };									/* Result from do_query_string(). */
double num_result;												/* Result from do_query_number(). */
unsigned char ieeeblock_data[IEEEBLOCK_SPACE];					/* Result from do_query_ieeeblock(). */
double dbl_results[10];											/* Result from do_query_numbers(). */


/* Main Program
* --------------------------------------------------------------- */

void main(void)
{
	/* Open the default resource manager session. */
	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) error_handler();
	/* Open the session using the oscilloscope's VISA address. */
	err = viOpen(defaultRM, VISA_ADDRESS, VI_NULL, VI_NULL, &vi);
	if (err != VI_SUCCESS) error_handler();
	/* Set the I/O timeout to fifteen seconds. */
	err = viSetAttribute(vi, VI_ATTR_TMO_VALUE, 15000);
	if (err != VI_SUCCESS) error_handler();

	/* Initialize - start from a known state. */
	initialize();

	/* Capture data. */
	capture();

	/* Analyze the captured waveform. */
	analyze();

	do_command("*CLS");//DC: added

	/* Close the vi session and the resource manager session. */
	viClose(vi);
	viClose(defaultRM);


}


/* Initialize the oscilloscope to a known state.
* --------------------------------------------------------------- */
void initialize(void)
{
	/* Clear the interface. */
	err = viClear(vi);
	if (err != VI_SUCCESS) error_handler();
	/* Get and display the device's *IDN? string. */
	do_query_string("*IDN?");
	printf("Oscilloscope *IDN? string: %s\n", str_result);
	/* Clear status and load the default setup. */
	do_command("*CLS");
	//do_command("*RST"); //DC: This resets the oscilloscope
}


/* Capture the waveform.
* --------------------------------------------------------------- */
void capture(void)
{
	int num_bytes;
	FILE *fp;
	/* Use auto-scale to automatically configure oscilloscope. */
	//do_command(":AUToscale");

	/* Change settings with individual commands:
	/* Set vertical scale and offset. */
	do_command(":CHANnel1:SCALe 5.0");
	do_query_string(":CHANnel1:SCALe?");
	printf("Channel 1 vertical scale: %s\n", str_result);

	do_command(":CHANnel1:OFFSet 0.0");
	do_query_string(":CHANnel1:OFFSet?");
	printf("Channel 1 offset: %s\n", str_result);

	do_command(":TIMebase:MODE MAIN");
	do_query_string(":TIMebase:MODE?");
	printf("Timebase mode: %s\n", str_result);

	/* Set horizontal scale and offset. */
	do_command(":TIMebase:SCALe 0.00005");
	do_query_string(":TIMebase:SCALe?");
	printf("Timebase scale: %s\n", str_result);

	do_command(":TIMebase:POSition 0.0");
	do_query_string(":TIMebase:POSition?");
	printf("Timebase position: %s\n", str_result);

	/* Set the acquisition type (NORMal, PEAK, AVERage, or HRESolution). */
	do_command(":ACQuire:TYPE NORMal");
	do_query_string(":ACQuire:TYPE?");
	printf("Acquire type: %s\n", str_result);

	/* Set trigger mode (EDGE, PULSe, PATTern, etc., and input source. */
	do_command(":TRIGger:MODE EDGE");
	do_query_string(":TRIGger:MODE?");
	printf("Trigger mode: %s\n", str_result);

	/* Set EDGE trigger parameters. */
	do_command(":TRIGger:EDGE:SOURce CHANnel1");
	do_query_string(":TRIGger:EDGE:SOURce?");
	printf("Trigger edge source: %s\n", str_result);

	do_command(":TRIGger:EDGE:LEVel 7.0");
	do_query_string(":TRIGger:EDGE:LEVel?");
	printf("Trigger edge level: %s\n", str_result);

	do_command(":TRIGger:EDGE:SLOPe POSitive");
	do_query_string(":TRIGger:EDGE:SLOPe?");
	printf("Trigger edge slope: %s\n", str_result);




	do_command(":MARKer:MODE WAVeform");
	do_query_string(":MARKer:MODE?");
	printf("Marker mode: %s\n", str_result);


	if (0)
	{
		/* Save oscilloscope configuration. */
		/* Read system setup. */
		num_bytes = do_query_ieeeblock(":SYSTem:SETup?");
		printf("Read setup string query (%d bytes).\n", num_bytes);
		/* Write setup string to file. */
		fp = fopen("c:\\scope\\config\\setup.stp", "wb");
		num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes, fp);
		fclose(fp);
		printf("Wrote setup string (%d bytes) to ", num_bytes);
		printf("c:\\scope\\config\\setup.stp.\n");
	}
	///* Or, configure by loading a previously saved setup. */
	///* Read setup string from file. */
	//fp = fopen("c:\\scope\\config\\setup.stp", "rb");
	//num_bytes = fread(ieeeblock_data, sizeof(unsigned char),IEEEBLOCK_SPACE, fp);
	//fclose(fp);
	//printf("Read setup string (%d bytes) from file ", num_bytes);
	//printf("c:\\scope\\config\\setup.stp.\n");
	///* Restore setup string. */
	//num_bytes = do_command_ieeeblock(":SYSTem:SETup", num_bytes);
	//printf("Restored setup string (%d bytes).\n", num_bytes);

	/* Capture an acquisition using :DIGitize. DC: for some reason DIGitize does not wait for the trigger on the scope.
	I also tried the C# code with the same result*/
	do_command(":DIGitize CHANnel1");
	//do_command(":SINGle");


	/*Check if the scope was triggered*/
	if (0)
	{
		do_query_number(":TER?");
		printf("Trigger?: %.0f \n", num_result);
		while (!(int)num_result)
		{
			do_query_number(":TER?");
			printf("Trigger?: %.0f \n", num_result);
		}
	}


}

/* Analyze the captured waveform.
* --------------------------------------------------------------- */
void analyze(void)
{
	double wav_format;
	double acq_type;
	double wav_points;
	double avg_count;
	double x_increment;
	double x_origin;
	double x_reference;
	double y_increment;
	double y_origin;
	double y_reference;
	FILE *fp;
	int num_bytes; /* Number of bytes returned from instrument. */
	int i;


	/*TEST: measure single values*/
	for (int i = 0; i < 60; i++)
	{
		char str[3];
		sprintf_s(str, 3, "%d", i);

		char str_cmd[80];
		strcpy_s(str_cmd, 80, ":MARKer:X1Position ");
		strcat_s(str_cmd, 80, str);
		strcat_s(str_cmd, 80, " us");

		//printf("command: %s\n", str_cmd);
		do_command(str_cmd);
		do_query_string(":MARKer:Y1Position?");
		printf("Y1 Position %s?: %s\n", str, str_result);

		/*If I want to save the query as a number*/
		//do_query_number(":MARKer:Y1Position?");
		//printf("Y1 Position?: %.5f \n", num_result);
	}





	if (0)
	{
		/* Make a couple of measurements.
		* ------------------------------------------------------------- */
		do_command(":MEASure:SOURce CHANnel1");
		do_query_string(":MEASure:SOURce?");
		printf("Measure source: %s\n", str_result);

		do_command(":MEASure:FREQuency");
		do_query_number(":MEASure:FREQuency?");
		printf("Frequency: %.4f kHz\n", num_result / 1000);

		do_command(":MEASure:VAMPlitude");
		do_query_number(":MEASure:VAMPlitude?");
		printf("Vertical amplitude: %.2f V\n", num_result);
	}

	if (0)
	{
		/* Download the screen image.
		* ------------------------------------------------------------- */
		do_command(":HARDcopy:INKSaver OFF");
		/* Read screen image. */
		num_bytes = do_query_ieeeblock(":DISPlay:DATA? PNG, COLor");
		printf("Screen image bytes: %d\n", num_bytes);
		/* Write screen image bytes to file. */
		fp = fopen("c:\\scope\\data\\screen.png", "wb");
		num_bytes = fwrite(ieeeblock_data, sizeof(unsigned char), num_bytes, fp);
		fclose(fp);
		printf("Wrote screen image (%d bytes) to ", num_bytes);
		printf("c:\\scope\\data\\screen.png.\n");
	}
	if (0)
	{
		/* Download waveform data.
		* ------------------------------------------------------------- */
		/* Set the waveform points mode. */
		do_command(":WAVeform:POINts:MODE RAW");
		do_query_string(":WAVeform:POINts:MODE?");
		printf("Waveform points mode: %s\n", str_result);

		/* Get the number of waveform points available. */
		do_query_string(":WAVeform:POINts?");
		printf("Waveform points available: %s\n", str_result);

		/* Set the waveform source. */
		do_command(":WAVeform:SOURce CHANnel1");
		do_query_string(":WAVeform:SOURce?");
		printf("Waveform source: %s\n", str_result);

		/* Choose the format of the data returned (WORD, BYTE, ASCII): */
		do_command(":WAVeform:FORMat BYTE");
		do_query_string(":WAVeform:FORMat?");
		printf("Waveform format: %s\n", str_result);

		/* Display the waveform settings: */
		do_query_numbers(":WAVeform:PREamble?");
		wav_format = dbl_results[0];
		if (wav_format == 0.0)
		{
			printf("Waveform format: BYTE\n");
		}
		else if (wav_format == 1.0)
		{
			printf("Waveform format: WORD\n");
		}
		else if (wav_format == 2.0)
		{
			printf("Waveform format: ASCii\n");
		}
		acq_type = dbl_results[1];
		if (acq_type == 0.0)
		{
			printf("Acquire type: NORMal\n");
		}
		else if (acq_type == 1.0)
		{
			printf("Acquire type: PEAK\n");
		}
		else if (acq_type == 2.0)
		{
			printf("Acquire type: AVERage\n");
		}
		else if (acq_type == 3.0)
		{
			printf("Acquire type: HRESolution\n");
		}
		wav_points = dbl_results[2];
		printf("Waveform points: %e\n", wav_points);
		avg_count = dbl_results[3];
		printf("Waveform average count: %e\n", avg_count);
		x_increment = dbl_results[4];
		printf("Waveform X increment: %e\n", x_increment);
		x_origin = dbl_results[5];
		printf("Waveform X origin: %e\n", x_origin);
		x_reference = dbl_results[6];
		printf("Waveform X reference: %e\n", x_reference);
		y_increment = dbl_results[7];
		printf("Waveform Y increment: %e\n", y_increment);
		y_origin = dbl_results[8];
		printf("Waveform Y origin: %e\n", y_origin);
		y_reference = dbl_results[9];
		printf("Waveform Y reference: %e\n", y_reference);

		/* Read waveform data. */
		num_bytes = do_query_ieeeblock(":WAVeform:DATA?");
		printf("Number of data values: %d\n", num_bytes);

		/* Open file for output. */
		fp = fopen("c:\\scope\\data\\waveform_data.csv", "wb");

		/* Output waveform data in CSV format. */
		for (i = 0; i < num_bytes - 1; i++)
		{
			/* Write time value, voltage value. */
			fprintf(fp, "%9f, %6f\n", x_origin + ((float)i * x_increment), (((float)ieeeblock_data[i] - y_reference) * y_increment) + y_origin);
		}
		/* Close output file. */
		fclose(fp);
		printf("Waveform format BYTE data written to ");
		printf("c:\\scope\\data\\waveform_data.csv.\n");

	}
}

/* Send a command to the instrument.
* --------------------------------------------------------------- */
void do_command(char *command)
{
	char message[80];
	strcpy_s(message, 80, command);
	strcat_s(message, 80, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();
	check_instrument_errors();
}

/* Command with IEEE definite-length block.
* --------------------------------------------------------------- */
int do_command_ieeeblock(char *command, int num_bytes)
{
	char message[80];
	int data_length;
	strcpy_s(message, 80, command);
	strcat_s(message, 80, " #8%08d");
	err = viPrintf(vi, message, num_bytes);
	if (err != VI_SUCCESS) error_handler();
	err = viBufWrite(vi, ieeeblock_data, num_bytes, &data_length);
	if (err != VI_SUCCESS) error_handler();
	check_instrument_errors();
	return(data_length);
}

/* Query for a string result.
* --------------------------------------------------------------- */
void do_query_string(char *query)
{
	char message[80];
	strcpy_s(message, 80, query);
	strcat_s(message, 80, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();
	err = viScanf(vi, "%t", str_result);
	if (err != VI_SUCCESS) error_handler();
	check_instrument_errors();
}

/* Query for a number result.
* --------------------------------------------------------------- */
void do_query_number(char *query)
{
	char message[80];
	strcpy_s(message, 80, query);
	strcat_s(message, 80, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();
	err = viScanf(vi, "%lf", &num_result);
	if (err != VI_SUCCESS) error_handler();
	check_instrument_errors();
}

/* Query for numbers result.
* --------------------------------------------------------------- */
void do_query_numbers(char *query)
{
	char message[80];
	strcpy_s(message, 80, query);
	strcat_s(message, 80, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();
	err = viScanf(vi, "%,10lf\n", dbl_results);
	if (err != VI_SUCCESS) error_handler();
	check_instrument_errors();
}

/* Query for an IEEE definite-length block result.
* --------------------------------------------------------------- */
int do_query_ieeeblock(char *query)
{
	char message[80];
	int data_length;
	strcpy_s(message, 80, query);
	strcat_s(message, 80, "\n");
	err = viPrintf(vi, message);
	if (err != VI_SUCCESS) error_handler();
	data_length = IEEEBLOCK_SPACE;
	err = viScanf(vi, "%#b\n", &data_length, ieeeblock_data);
	if (err != VI_SUCCESS) error_handler();
	if (data_length == IEEEBLOCK_SPACE)
	{
		printf("IEEE block buffer full: ");
		printf("May not have received all data.\n");
	}
	check_instrument_errors();
	return(data_length);
}

/* Check for instrument errors.
* --------------------------------------------------------------- */
void check_instrument_errors()
{
	char str_err_val[256] = { 0 };
	char str_out[800] = "";
	err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
	if (err != VI_SUCCESS) error_handler();
	while (strncmp(str_err_val, "+0,No error", 3) != 0)
	{
		strcat_s(str_out, 800, ", ");
		strcat_s(str_out, 800, str_err_val);
		err = viQueryf(vi, ":SYSTem:ERRor?\n", "%t", str_err_val);
		if (err != VI_SUCCESS) error_handler();
	}
	if (strcmp(str_out, "") != 0)
	{
		printf("INST Error%s\n", str_out);
		err = viFlush(vi, VI_READ_BUF);
		if (err != VI_SUCCESS) error_handler();
		err = viFlush(vi, VI_WRITE_BUF);
		if (err != VI_SUCCESS) error_handler();
	}
}

/* Handle VISA errors.
* --------------------------------------------------------------- */
void error_handler()
{
	char err_msg[1024] = { 0 };
	viStatusDesc(vi, err, err_msg);
	printf("VISA Error: %s\n", err_msg);
	if (err < VI_SUCCESS)
	{
		exit(1);
	}
}