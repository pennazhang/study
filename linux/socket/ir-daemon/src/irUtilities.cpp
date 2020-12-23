/*-------------------------------------------------------------------------*
 * File:  irUtilities.cpp - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup IR
 *  @{
 *  @brief Infra-Red System
 *  @date Feb 6, 2013
 *  @author Wirth
 *
 * Utilities for controlling the UART IR.
 */
/*-------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "irUtilities.h"
#include "videoEP.h"
#include "config.h"

#define CIR_IOCTL_SET_PULSE_WIDTH   _IOW('c', 1, int)
#define CIR_IOCTL_SET_CARRIER       _IOW('c', 2, int)


#define CIR_DEVICE  "/dev/cir"
#define CIR_DRIVER  "/lib/modules/3.2.0-svsi/kernel/drivers/media/rc/ti-cir.ko"

enum {
    IR_SEQUENCE_1 = 0,
    IR_SEQUENCE_2
};

static unsigned int bitTime_g;
static unsigned int carrierFreq_g;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void setPulseWidth(int fd, unsigned int pulseWidth);
static void setCarrier(int fd, unsigned int carrierFreq);
int IROpenMmap(volatile unsigned char **uartPtr);
int IRCloseMmap(int fd, void *ptr);
/*---------------------------------------------------------------------------*
 * Routine: irMakeOutput
 *---------------------------------------------------------------------------*/
/**
 *  Generate a bit stream for the burst pair stream of 16-bit values.
 *  A burst pair is a count of bits HIGH followed by a number of bits
 *  LOW.  Effectively, this routine creates a number of 1's in inBuffer[0],
 *  then 0's in inBuffer[1], then a number of 1's in inBuffer[2], etc.
 *
 *  @param [in] inBuffer -- Incoming burst pair words
 *  @param [in] inLength -- Number of burst pair words
 *  @param [out] outBuffer -- Output bytes for sending
 *
 *  @return Length of number of bytes generated
 */
/*---------------------------------------------------------------------------*/
unsigned int irMakeOutput(unsigned int inBuffer[], unsigned int inLength, unsigned char outBuffer[])
{
	unsigned int i, j;
	unsigned int byteVal = 0;
	unsigned int bitCount = 0;
	unsigned int outputLen = 0;
	unsigned int onoffToggle = 1;
	unsigned int *inputBufPtr = inBuffer;
	unsigned char *outputBuf = outBuffer;

	// Walk through all the burst pair lengths (each itteration is half a pair)
	for (i=0; i<inLength; i++, inputBufPtr++) {
		// How many bits to output in this word?
		for (j=0; j<*inputBufPtr; j++) {
			// Output that many bits (high or low)
			byteVal >>= 1;
			if (onoffToggle)
				byteVal |= 0x80;
			bitCount++;
			if (bitCount == 8) {
				*outputBuf++ = byteVal;
				outputLen++;
				byteVal = 0;
				bitCount = 0;
			}
		}

		// Toggle the bit for next word
		if (onoffToggle)
			onoffToggle = 0;
		else
			onoffToggle = 1;
	}

	// flush partial bits, if any (one last byte)
	if (bitCount) {
		byteVal = byteVal >> (8 - bitCount);
		*outputBuf++ = byteVal;
		outputLen++;
	}

#if 0
	for (i=0; i<outputLen; i++) {
		printf("%02x, ", outBuffer[i]);
	}
	printf("\n");
#endif

	//printf("outputLen=%d\n", outputLen);
	return outputLen;
}

/*---------------------------------------------------------------------------*
 * Routine: importPronto
 *---------------------------------------------------------------------------*/
/**
 *  Load an IR command stored in Philips Pronto format from a file.
 *
 *  @param [in] fileName -- Name of file to load
 *  @param [out] irParams -- Output of imported file
 *  @param [in] MMIO -- flag for memory mapped io
 *
 *  @return Error code, 0=none
 */
/*---------------------------------------------------------------------------*/
int importPronto(char *fileName, irParams_t *irParams, bool useMMIO)
{
	int i, ftype, freq, n1, n2;
	FILE *filePtr = fopen(fileName, "r");
	unsigned int min1, min2;

	// Reset length of sequence and check if file opened
	irParams->seq1Count = 0;
	irParams->seq2Count = 0;
	if (filePtr == NULL) {
		SVSI_SYSLOG("Failed to open ir file: %s!", fileName);
		return -1;
	}

	// Read the first code and check if a learned code (0x0000)
	ftype = -1;
	fscanf(filePtr, "%x", &ftype);
	if (ftype != 0) {
		SVSI_SYSLOG("Not Pronto learned code: %s!", fileName);
		fclose(filePtr);
		return -1;
	}

	// Read the frequency and lengths of sequence 1 and sequence 2
	fscanf(filePtr, "%x", &freq);
	fscanf(filePtr, "%x", &n1);
	fscanf(filePtr, "%x", &n2);

	// Verify we have enough room to read and parse this file
	if(n1*2 > MAX_IR_VALUES || n2*2 > MAX_IR_VALUES)
	{
		SVSI_SYSLOG("More values than can hold in array! len1: %d len2: %d max Allowed: %d!", n1, n2, MAX_IR_VALUES);
		fclose(filePtr);
		return -1;
	}
	//printf("%d, %d, %d, %d\n", ftype, freq, n1, n2);

	// Read sequence #1
	for (i=0; i<n1*2; i++) {
		int x;
		unsigned int v;
		x = fscanf(filePtr, "%x", &v);
		if (x != 1) {
			SVSI_SYSLOG("Failed to read file: %s!", fileName);
			fclose(filePtr);
			return -1;
		}
		if (i == 0) {
			min1 = v;
		}
		else if (min1 > v) {
			min1 = v;
		}
		irParams->seq1[i] = v;
		irParams->seq1Count++;
		//printf("%d, ", buf1[i]);
	}
	//printf("\n");

	// Read sequence #2
	for (i=0; i<n2*2; i++) {
		int x;
		unsigned int v;
		x = fscanf(filePtr, "%x", &v);
		if (x != 1) {
			SVSI_SYSLOG("Failed to read file second sequence: %s!", fileName);
			fclose(filePtr);
			return -1;
		}
		if (i == 0) {
			min2 = v;
		}
		else if (min2 > v) {
			min2 = v;
		}
		irParams->seq2[i] = v;
		irParams->seq2Count++;
		//printf("%d, ", buf2[i]);
	}
	//printf("\n");
	fclose(filePtr);

	if (useMMIO) {
		// Verify the sequence lengths are valid
		if((min1 == 0 && n1 > 0) || (min2 == 0 && n2 > 0))
		{
			SVSI_SYSLOG("Min of a valid sequence is 0 len1: %d min1: %d len2: %d min2: %d !", n1, min1, n2, min2);
			return -1;
		}

		// now normalize and calculate CFPS and DLL values
		for(i=0;i<irParams->seq1Count;i++)
		{
			irParams->seq1[i] = (unsigned int)( (double)irParams->seq1[i] / min1  + 0.5 );
		}
		for(i=0;i<irParams->seq2Count;i++)
		{
			irParams->seq2[i] = (unsigned int)( (double)irParams->seq2[i] / min2  + 0.5 );
		}

		// Turn the carrier frequencey into a standard freqeuncy (not the ratio to 40k)
		// and, based on the minimum frequency widths, determine the periods for pulse
		// width 1 and 2.
		irParams->carryFreq = 1000000. / ( (double)freq * .241246 );
		irParams->Tperiod1 = (double)min1 / irParams->carryFreq;
		irParams->Tperiod2 = (double)min2 / irParams->carryFreq;
	}

	irParams->TperioduS = ((double)freq * .241246) + 0.5;
	irParams->carryFreqkHz = (1 / (double)irParams->TperioduS) * 1000000;
	SVSI_SYSLOG("IR pulse period: %d", irParams->TperioduS);
	SVSI_SYSLOG("IR carrier: %d", irParams->carryFreqkHz);

	// Remember the format type
	irParams->formatType = IR_PRONTO_FORMAT;

	// If there is data for sequence 1, generate the actual bits for the UART
	if(irParams->seq1Count == 0)
		irParams->seq1CountOut = 0;
	else
		irParams->seq1CountOut = irMakeOutput(irParams->seq1, irParams->seq1Count, irParams->seq1Out);

	// If there is data for sequence 2, generate the actual bits for the UART
	if(irParams->seq2Count == 0)
		irParams->seq2CountOut = 0;
	else
		irParams->seq2CountOut = irMakeOutput(irParams->seq2, irParams->seq2Count, irParams->seq2Out);

	return 0;
}

/*---------------------------------------------------------------------------*
 * Routine: loadIRCommand
 *---------------------------------------------------------------------------*/
/**
 *  Load an IR command (currently only Philips Pronto format) from a file.
 *
 *  @param [in] fileName -- Name of file to load
 *  @param [out] irParams -- Output of imported file
 *
 *  @return Error code, 0=none
 */
/*---------------------------------------------------------------------------*/
int loadIRCommand(char *fileName, irParams_t *irParams, bool useMMIO)
{
	int retVal;

	// try pronto import first
	retVal = importPronto(fileName, irParams, useMMIO);
	if(retVal == 0)
	{
		SVSI_SYSLOG("Format is Pronto and loaded successfully!");
		return retVal;
	}

	// TODO Add additional formats here

	return -1;
}

/*---------------------------------------------------------------------------*
 * Routine: sendIRCommand
 *---------------------------------------------------------------------------*/
/**
 *  Send a string of IR bytes that already have the IR command encoded in
 *  it.  The carrier frequency passed in is used to set the rate of the
 *  bits.
 *
 *  @param [in] irParams -- IR Sequence to send (already processed bytes)
 *  @param [in] sendSecondSeq -- 0=send sequence #1, 1=send sequence #2
 *
 *  @return Error code, 0=none
 */
/*---------------------------------------------------------------------------*/
int sendIRCommand(irParamsFIFO_t *irParamsFIFO, unsigned char sendSecondSeq)
{
    int fd;
    int bytesOut = 0;
    irParams_t *irParams;
    volatile unsigned char *uartPtr;
    int tpdiv;
    int loop;

    irParams = &irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr];

	// If aborted, this command does nothing!
    if (irParams->abortRepeat)
        return 0;

    if (!irParamsFIFO->useMMIO) {
        fd = open(CIR_DEVICE, O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            SVSI_SYSLOG("Error: Could not get handle to cir device!");
            return -1;
        }
        setCarrier(fd, irParams->carryFreqkHz);
        setPulseWidth(fd, irParams->TperioduS);

        if(sendSecondSeq == 0)
        {
            // send sequence 1 bitstream
            while (bytesOut < irParams->seq1CountOut) {
                bytesOut += write(fd, irParams->seq1Out + bytesOut, irParams->seq1CountOut);
            }
        }
        else
        {
            // send sequence 2 bitstream
            while (bytesOut < irParams->seq2CountOut) {
                bytesOut += write(fd, irParams->seq2Out + bytesOut, irParams->seq2CountOut);
            }
        }
        close(fd);
    } else {
        //int fd = openIRMmap(&uartPtr);
        uartPtr = irParamsFIFO->uartPtr;

        // legacy mmap CIR implementation
        int cfval = (int)((48000000. / 12. / irParams->carryFreq) + 0.5);

	// Calculate the baud rate based on first or second sequence
	if(sendSecondSeq == 0)
		tpdiv = (int)((48000000. / 16. * irParams->Tperiod1) + 0.5);
	else
		tpdiv = (int)((48000000. / 16. * irParams->Tperiod2) + 0.5);

	// Configure the UART for the baud rate needed
	uartPtr[IR_UART_IER] = 0;
	uartPtr[IR_UART_MRD1] = 7; // reset mode

	uartPtr[IR_UART_LCR] = 0x83;
	uartPtr[IR_UART_DLL] = 0;
	uartPtr[IR_UART_DLM] = 0;
	uartPtr[IR_UART_LCR] = 3;

	uartPtr[IR_UART_MCR] = 3;
	uartPtr[IR_UART_FCR] = 7;

	uartPtr[IR_UART_LCR] = 0x83;
	uartPtr[IR_UART_DLL] = tpdiv & 0xff;
	uartPtr[IR_UART_DLM] = tpdiv >> 8;
	uartPtr[IR_UART_LCR] = 3;

	uartPtr[IR_UART_MDR2] = 0;
	uartPtr[IR_UART_CFPS] = cfval;

	uartPtr[IR_UART_MRD1] = 6; // CIR mode

	// FIXME: tune this ???  Settling time?  Waiting for previous FIFO to finish?
	usleep(10000);

	if(sendSecondSeq == 0)
	{
		// send sequence 1
		for (loop=0; loop<irParams->seq1CountOut; loop++) {
			// If abort flag is set, stop here before anything more is done
			if (irParams->abortRepeat)
				break;
			while (uartPtr[IR_UART_SSR] & 1) // fifo full
				usleep(10); // FIXME: tune this ???
			uartPtr[IR_UART_THR] = irParams->seq1Out[loop];
			//printf("seq: %d index: %d val: %d\n", sendSecondSeq + 1, loop, irParams->seq1Out[loop]);
		}
	}
	else
	{
		// send sequence 2
		for (loop=0; loop<irParams->seq2CountOut; loop++) {
			// If abort flag is set, stop here before anything more is done
			if (irParams->abortRepeat)
				break;
			while (uartPtr[IR_UART_SSR] & 1) // fifo full
				usleep(10); // FIXME: tune this ???
			uartPtr[IR_UART_THR] = irParams->seq2Out[loop];
			//printf("seq: %d index: %d val: %d\n", sendSecondSeq + 1, loop, irParams->seq2Out[loop]);
		}
	}
	//closeIRMmap((void*)uartPtr);
	//close(fd);
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine: irThread
 *---------------------------------------------------------------------------*/
/**
 *  The irThread is created to process IR commands.  The code waits
 *  sits watching the FIFO and waits for the next entry in the list to
 *  be ready with a command.  When ready, the IR command is sent and the
 *  next FIFO location is checked.
 *
 *  @param [in] arg -- void pointer to irParamsFIFO_t structure
 *
 *  @return End of thread error code (always NULL)
 */
/*---------------------------------------------------------------------------*/
void* irThread(void *arg)
{
	irParamsFIFO_t *irParamsFIFO = (irParamsFIFO_t*)arg;
	prctl(PR_SET_NAME,"videoEP-IR",0,0,0);

	while(1)
	{
		if(irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].readyToSend)
		{
			SVSI_SYSLOG("before send!");
			sendIRCommand(irParamsFIFO, IR_SEQUENCE_1);
			usleep(irParamsFIFO->timeRepeatHoldoff * 1000);
			sendIRCommand(irParamsFIFO, IR_SEQUENCE_2);
			SVSI_SYSLOG("after send!");

			if (irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].abortRepeat) {
				// Note the abort has taken place (and clear)
				irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].abortRepeat = 0;
				// Stop repeating too
				irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].repeat = 0;
			}
			// Only advance if we are NOT repeating
			if (irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].repeat == 0) {
				irParamsFIFO->ipParams[irParamsFIFO->irFifoRdPtr].readyToSend = 0;
				irParamsFIFO->irFifoRdPtr++;
				if(irParamsFIFO->irFifoRdPtr >= IR_FIFO_SIZE)
					irParamsFIFO->irFifoRdPtr = 0;
			}
		}
		usleep(irParamsFIFO->timeCommandHoldoff * 1000);
	}
	/* This should never be reached */
	pthread_exit(NULL);

}

void initIR(irParamsFIFO_t *irCommandFIFO)
{
	// start irThread
	pthread_t irThreadStruct;
	pthread_attr_t attr_irThread;
	pthread_attr_init(&attr_irThread);
	pthread_attr_setdetachstate(&attr_irThread, PTHREAD_CREATE_DETACHED);

	int rc = pthread_create(&irThreadStruct, &attr_irThread, &irThread, (void *)irCommandFIFO);
	if (rc) {
		SVSI_SYSLOG("%d: pthread_create error for ir thread\n", rc);
	}
}

/*---------------------------------------------------------------------------*
 * Routine: IRResetData
 *---------------------------------------------------------------------------*/
/**
 *  Reset the stored files for doing IR data.
 * <!------------------------------------------------------------------------*/
void IRResetData(void)
{
    SVSI_SYSLOG("Resetting IR data");

    // Just zap all the .ir files
    system("rm " IRCODE_PATH "/*.ir");

    // Put in or truncate the index.php file
    system("truncate -s 0 " IRCODE_PATH "/index.php");
}

/*---------------------------------------------------------------------------*
 * Routine: IRResetFIFO
 *---------------------------------------------------------------------------*/
/**
 *  Reset all IR commands in the FIFO.
 *
 *  @params [in] irParamsFIFO -- Pointer to the FIFO to reset.
 *
 * <!------------------------------------------------------------------------*/
void IRResetFIFO(irParamsFIFO_t *irParamsFIFO)
{
    int i;

    // Reset all the waiting IR commands
    for (i=0; i<IR_FIFO_SIZE; i++) {
        if (irParamsFIFO->ipParams[i].readyToSend) {
            // Reset all flags
            irParamsFIFO->ipParams[i].readyToSend = 0;
            irParamsFIFO->ipParams[i].repeat = 0;
            irParamsFIFO->ipParams[i].abortRepeat = 0;
        }
    }
    // Reset the read/write pointers
    irParamsFIFO->irFifoRdPtr = 0;
    irParamsFIFO->irFifoWrPtr = 0;
}

/*---------------------------------------------------------------------------*
 * Routine: IRLoadDriver
 *---------------------------------------------------------------------------*/
/**
 *  Load ti_cir driver
 *
 * <!------------------------------------------------------------------------*/
int IRLoadDriver(void) {
    char command[512];
    SVSI_SYSLOG("Loading CIR module");
    snprintf(command, sizeof(command) - 1, "insmod %s ", CIR_DRIVER);
    system(command);
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine: IRUnloadDriver
 *---------------------------------------------------------------------------*/
/**
 *  Unload ti_cir driver
 *
 * <!------------------------------------------------------------------------*/
int IRUnloadDriver(void) {
    SVSI_SYSLOG("Unloading CIR Module");
    system("rmmod ti_cir");
    return 0;
}

 /*---------------------------------------------------------------------------*
 * Routine: IROpenMmap
 *---------------------------------------------------------------------------*/
/**
 *  Open memory mapped I/O to UART peripheral
 *
 *  @param [out] uartPtr -- Pointer pointer to memory mapped I/O
 */
/*---------------------------------------------------------------------------*/
int IROpenMmap(volatile unsigned char **uartPtr)
{
    int fd;

    ///fd = memory_map_open((volatile unsigned char **)uartPtr, 0x48024000);
	fd = open("/dev/mem", O_RDWR);
	if (fd < 0) {
		perror("Failed to open /dev/mem");
        SVSI_SYSLOG("Error mapping CIR UART!");
		return fd;
	}

	*uartPtr = (unsigned char*)mmap(0, 1024*1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x48024000);

    return fd;
}

 /*---------------------------------------------------------------------------*
 * Routine: IRCloseMmap
 *---------------------------------------------------------------------------*/
/**
 *  Close memory mapped I/O to UART peripheral
 *
 *  @param [in] uartPtr -- Pointer to memory mapped I/O
 */
/*---------------------------------------------------------------------------*/
int IRCloseMmap(int fd, void *uartPtr) {

	int retVal = -1;

    if (uartPtr != NULL) {
        retVal = munmap(uartPtr, 1024*1024);
        close(fd);
    }

    if (retVal < 0) {
        SVSI_SYSLOG("Error unmapping CIR UART!");
        return -1;
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Private functions:
 *---------------------------------------------------------------------------*/

 /*---------------------------------------------------------------------------*
 * Routine: setPulseWidth
 *---------------------------------------------------------------------------*/
/**
 *  Sets the IR baud rate
 */
/*---------------------------------------------------------------------------*/
static void setPulseWidth(int fd, unsigned int pulseWidth) {
    ioctl(fd, CIR_IOCTL_SET_PULSE_WIDTH, pulseWidth);
    bitTime_g = pulseWidth;
}

 /*---------------------------------------------------------------------------*
 * Routine: setCarrier
 *---------------------------------------------------------------------------*/
/**
 *  Sets the IR carrier frequency
 */
/*---------------------------------------------------------------------------*/
static void setCarrier(int fd, unsigned int carrierFreq) {
    ioctl(fd, CIR_IOCTL_SET_CARRIER, carrierFreq);
    carrierFreq_g = carrierFreq;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  irUtilities.cpp
 *-------------------------------------------------------------------------*/
