/*-------------------------------------------------------------------------*
 * File:  irUtilities.h - Copyright (C) 2013 All Rights Reserved - SVSi
 *-------------------------------------------------------------------------*/
#ifndef IRUTILITIES_H_
#define IRUTILITIES_H_

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

// enabled legacy mmap cir
#define MMAP_IR

#define MAX_IR_VALUES		4096

#define IR_PRONTO_FORMAT	0x01

// UART offsets
#define IR_UART_THR 	0x00
#define IR_UART_DLL 	0x00
#define IR_UART_DLM 	0x04
#define IR_UART_IER		0x04
#define IR_UART_FCR		0x08
#define IR_UART_LCR		0x0c
#define IR_UART_MCR		0x10
#define IR_UART_MRD1	0x20
#define IR_UART_MDR2	0x24
#define IR_UART_SSR		0x44
#define IR_UART_CFPS 	0x60

#define IR_FIFO_SIZE	10

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
//! Single IR command to send (encoding of Philips Pronto Code)
typedef struct
{
	//! Type of IR command format (e.g. 1=IR_PRONTO_FORMAT)
	unsigned char formatType;

#ifdef MMAP_IR
    // required for memory mapped access
    //! Carrier frequency
    double carryFreq;
    //! Duration of bits (in seconds) in sequence #1
    double Tperiod1;
    //! Duration of bits (in seconds) in sequence #2
    double Tperiod2;
#endif // MMAP_IR

	//! Carrier frequency kHz
	unsigned int carryFreqkHz;
	//! Duration of bits (in useconds)
	unsigned int TperioduS;

	//! Number of burst pairs in sequence #1
	unsigned short seq1Count;
	//! Number of burst pairs in sequence #2
	unsigned short seq2Count;

	//! sequence #1 array of burst pair words
	unsigned int seq1[MAX_IR_VALUES];
	//! sequence #2 array of burst pair words
	unsigned int seq2[MAX_IR_VALUES];

	//! Sequence #1 output byte length
	unsigned short seq1CountOut;
	//! Sequence #2 output byte length
	unsigned short seq2CountOut;

	//! Sequence #1 output bytes
	unsigned char seq1Out[MAX_IR_VALUES*2];
	//! Sequence #2 output bytes
	unsigned char seq2Out[MAX_IR_VALUES*2];

	//! Flag set when ready to send (1=ready, 0=not-ready)
	unsigned char readyToSend;

    //! Flag to repeat this command until aborted
    volatile unsigned char repeat;

    //! Flag to set to abort a currently repeating IR
    volatile unsigned char abortRepeat;
} irParams_t;

//! FIFO of IR commands
typedef struct
{
#ifdef MMAP_IR
    //! Pointer to UART registers
    volatile unsigned char *uartPtr;
#endif // MMAP_IR
	//! FIFO read point (place to take data out of the FIFO)
	unsigned char irFifoRdPtr;

	//! FIFO write point (place to put data into the FIFO)
	unsigned char irFifoWrPtr;

	//! IR commands
	irParams_t ipParams[IR_FIFO_SIZE];

    //! Time to pause between command and repeat (milliseconds)
    unsigned int timeRepeatHoldoff;

    //! Time to pause between the end of one command and the
    //! start of another (milliseconds)
    unsigned int timeCommandHoldoff;

    //! Flag to use either CIR drive or mmap register access
    unsigned char useMMIO;
} irParamsFIFO_t;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void* irThread(void *arg);
int loadIRCommand(char *fileName, irParams_t *irParams, bool useMMIO);
void initIR(irParamsFIFO_t *irCommandFIFO);
void IRResetData(void);
void IRResetFIFO(irParamsFIFO_t *fifo);
int IRLoadDriver(void);
int IRUnloadDriver(void);
int IROpenMmap(volatile unsigned char **uartPtr);
int IRCloseMmap(int fd, void *uartPtr);

#endif /* IRUTILITIES_H_ */

/*-------------------------------------------------------------------------*
 * End of File:  irUtilities.h
 *-------------------------------------------------------------------------*/
