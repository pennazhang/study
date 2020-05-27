#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long ULONG;

typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;

typedef float F32;
typedef UINT8 U8;
typedef UINT16 U16;
typedef UINT32 U32;

typedef unsigned char BYTE;

typedef enum
{
	MIC_CHANNEL_CENTER = 0,
	MIC_CHANNEL_SURROUNDS = 1,
} MIC_CHANNEL;

#define LBYTE(a) (a & 0xFF)
#define HBYTE(a) ((a >> 8) & 0xFF)

#define SET_BIT(value, bit)  (value | (1 << bit))
#define CLEAR_BIT(value, bit)	(value & ~(1<<bit))

#define GET_BIT(value, bit) ((value >> bit) & 1)


#define		JSON_ENUM		-5
#define		JSON_STRING		-4
#define		JSON_BOOL		-3
#define		JSON_FLOAT		-2
#define		JSON_INT		-1

typedef enum 
{
	EQ_PEAKING,
	EQ_L_SHELF,
	EQ_H_SHELF,
	EQ_NOTCH, 
	EQ_LPF, 
	EQ_HPF, 
	EQ_BPF,
	EQ_APF,
	EQ_USER_DEFINED_FIR,
	EQ_XOVER,
} EQType;

typedef enum pfType
{
	PF_IIR_BS6 = 0,
	PF_IIR_BS12,
	PF_IIR_BS18,
	PF_IIR_BS24,
	PF_IIR_BS30,
	PF_IIR_BS36,
	PF_IIR_BS42,
	PF_IIR_BS48,
	PF_IIR_BW6,
	PF_IIR_BW12,
	PF_IIR_BW18,
	PF_IIR_BW24,
	PF_IIR_BW30,
	PF_IIR_BW36,
	PF_IIR_BW42,
	PF_IIR_BW48,
	PF_IIR_LR12,
	PF_IIR_LR24,
	PF_IIR_LR36,
	PF_IIR_LR48,
} PF_IIR_Type;


typedef enum 
{
	PF_IIR,
	PF_FIR,
} PF_TYPE;		/* PATH_FILTER only for HPF and LPF */

#endif