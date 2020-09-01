#ifndef __LINUX_COMMDEV_H
#define __LINUX_COMMDEV_H

#include <linux/types.h>
#include <linux/version.h>

#ifdef __KERNEL__

#include <linux/poll.h>

struct comm_device {
	struct comm_device *next;
	struct file_operations *f_op;
	int (*open) (struct comm_device *, struct file *);
	int board;		/* Device private index */
	int minor;
};

extern int commdev_init(void);
#define COMM_MAJOR	100
extern int comm_register_device(struct comm_device *p, int unit,int dev_type);
#define COMM_UNIT_ANY	-1
extern void comm_unregister_device(struct comm_device *pfd,int dev_type);

#define COMM_NUM_DEVICES	256

#define AUDIO_DEV		0
#define SYS_DEV 		1
#define VIDEO_DEV		2
#define DVR_DEV		3

#define AUDIO_MIN_MINOR		0
#define AUDIO_MAX_MINOR		4
#define SYS_MIN_MINOR		4
#define SYS_MAX_MINOR		5
#define VIDEO_MIN_MINOR 		8
#define VIDEO_MAX_MINOR		9
#define DVR_MIN_MINOR		9
#define DVR_MAX_MINOR		10


#endif

typedef struct DEVICE_INFO
{
	short swDeviceType; //0--slic ,1--Codec
	short swChannelNum; //SLIC MAX Channel num;
	unsigned char  cwChannelCS1;
	unsigned char  cwChannelReset1;
	unsigned char  cwChannelCS2;
	unsigned char  cwChannelReset2;

}tDEVICE_INFO, *ptDEVICE_INFO;

typedef struct AUDIO_CODEC
{
	int lwSampleMethod;
	int lwSampleBit;
	int lwSampleRate;
	int lwFrameTime;
	int lwUnderLock;
	int lwEcMethod;
	int lwSlienceSupp;
	short swJitterEmpty;
	short swJitterHold;
	int lwTalkerId;
}tAUDIO_CODEC, *ptAUDIO_CODEC;

typedef struct AUDIO_PCM
{
	char cwPcmName[16];
	int *plwPcmAddr;
	int lwPcmLen;
}tAUDIO_PCM, *ptAUDIO_PCM;

typedef struct SLIC_REG
{
	char cwRegFlag;
	unsigned char cwRegIndex;
	unsigned short swRegValue;
}tSLIC_REG, *ptSLIC_REG;

typedef struct KEY_TIME
{
	unsigned int lwSec;
	unsigned int lwUsec;
}tKEY_TIME, *ptKEY_TIME;

typedef struct AUDIO_MODE
{
	short swMode;
	short swEnable;
}tAUDIO_MODE, *ptAUDIO_MODE;

#define AUDIO_SET_CODEC			_IOW ('q', 0x80, struct tAUDIO_CODEC *)
#define AUDIO_GET_CODEC			_IOR ('q', 0x81, struct tAUDIO_CODEC *)
#define AUDIO_VOICE_START		_IO  ('q', 0x82)
#define AUDIO_VOICE_STOP		_IO  ('q', 0x83)
#define AUDIO_PLAY_TONE			_IOW ('q', 0x84, int)
#define AUDIO_STOP_TONE			_IO  ('q', 0x85)
#define AUDIO_GET_KEY			_IOR  ('q', 0x86,struct tKEY_TIME *)
#define AUDIO_PLAY_PCM		_IOW ('q', 0x87, struct tAUDIO_PCM *)
#define AUDIO_SEND_CALLERID _IOW ('q', 0x88, char *)
#define AUDIO_ADD_TALKER		_IOW ('q', 0x89, int)
#define AUDIO_DEL_TALKER		_IOW ('q', 0x8a, int)
#define AUDIO_SET_SLIC_REG		_IOW ('q', 0x8b, struct tSLIC_REG *)
#define AUDIO_GET_SLIC_REG		_IOR ('q', 0x8c, struct tSLIC_REG *)
#define AUDIO_SET_MODE			_IOW ('q', 0x8d, struct tAUDIO_MODE *)
#define AUDIO_LED_ON			_IO  ('q', 0x8e)
#define AUDIO_LED_OFF			_IO  ('q', 0x8f)	
#define AUDIO_CHECK_ENABLE  	_IO  ('q', 0x90)
#define AUDIO_LED_WINK		_IOW ('q', 0x91, int)
#define AUDIO_INIT_DEVICE		_IOW ('q', 0x92, struct tDEVICE_INFO *)
#define AUDIO_SET_LS_MODE		_IOW ('q', 0x93, int)

typedef struct ALARM_PARA
{
	char  cwEnable;//0-Disable,1-Enable
	char  cwGpioNo;
	char  cwGpioLevel;//0-Low, 1-High
	char  cwAttribute; //alarm in:0-level triggered,1-edge triggered,2-pulse count per second
				//alarm out: duration,unit:1 second
}tALARM_PARA,*ptALARM_PARA;

typedef struct ALARM_RES
{
	char  cwGpioNo;
	char  cwAlarmStat;//0-off, 1-level trigger on , 2-edge trigger on 
	short  swPulseCount; 
}tALARM_RES,*ptALARM_RES;


typedef struct DVR4CHIPS_PKT
{
	int lwDestChipId;
	int lwDataAddr;
	int lwDataLen;
}tDVR4CHIPS_PKT, *ptDVR4CHIPS_PKT;

#define SYS_SET_LOG_MODE		_IOW ('c', 0x80, int)
#define SYS_PRINT_VERSION		_IO  ('c',0x81)
#define SYS_CONTROL_LED		_IOW ('c', 0x82, int)
#define SYS_SET_GPIO_DIR		_IOW ('c', 0x83, int)
#define SYS_SET_GPIO_VAL		_IOW ('c', 0x84, int)
#define SYS_GET_GPIO_VAL		_IO ('c', 0x85)
#define SYS_GET_CHIP_ID		_IO ('c', 0x86)
#define SYS_GET_IPADDR		_IOR('c', 0x87, char *)
#define SYS_SEND_PKT		_IOW('c', 0x88, struct tDVR4CHIPS_PKT *)
#define SYS_RECEIVE_PKT		_IOR('c', 0x89, struct tDVR4CHIPS_PKT *)
#define SYS_GET_CAMERA_ID		_IO ('c', 0x8a)	
#define SYS_ENABLE_ALARM_IN	_IOW ('c', 0x8b, struct tALARM_PARA *)
#define SYS_GET_ALARM_RES		_IOW ('c', 0x8c, struct tAlARM_RES *)
#define SYS_ENABLE_ALARM_OUT	_IOW ('c', 0x8e, struct tALARM_PARA *)
#define SYS_SET_CLOCK_ORIGIN	_IOW ('c', 0x8f, int)


//****************OSD TYPE*******************************/

typedef struct BMP_PARA
{
	unsigned short swXpos;
	unsigned short swYpos;
	unsigned int lwAddr;
	unsigned int lwLen;
	unsigned int lwIndexAddr;
	unsigned short swIndexNumber;
	unsigned short swAttribute;
}tBMP_PARA,*ptBMP_PARA;

typedef struct SCREEN_PARA
{
	unsigned short swXpos;
	unsigned short swYpos;
	unsigned short swHeight;
	unsigned short swWidth;
	unsigned int   lwAttribute;
}tSCREEN_PARA,*ptSCREEN_PARA;


typedef struct LINE_PARA
{
	unsigned short swStartXpos;
	unsigned short swStartYpos;
	unsigned short swEndXpos;
	unsigned short swEndYpos;
	int lwAttribute;
}tLINE_PARA,*ptLINE_PARA;

typedef struct STRING_PARA
{
	unsigned int lwAddr;
	int lwLen;
	unsigned short swXpos;
	unsigned short swYpos;
	int lwAttribute;
}tSTRING_PARA,*ptSTRING_PARA;




////////////////////////////////////////////////////////////////////////////
typedef struct VIDEO_SIZE
{
	int lwMode;
	short swWidth;
	short swHeight;
}tVIDEO_SIZE,*ptVIDEO_SIZE;

typedef struct VIDEO_FORMAT
{
	int lwMode;
	short swRowScale;
	short swColumnScale;
	short swRowPosition;
	short swColumnPosition;
}tVIDEO_FORMAT,*ptVIDEO_FORMAT;

typedef struct VIDEO_ROBOT
{
	short swEvent;
	short swWalkData;
	short swTurnData;
	short swLrData;
	int	  swExtern;
}tVIDEO_ROBOT,*ptVIDEO_ROBOT;

typedef struct CAMERA_REG
{
	int lwRegIndex;
	unsigned int lwRegValue;
}tCAMERA_REG, *ptCAMERA_REG;

typedef struct VIDEO_QUALITY
{
	short swQuality;
	char cwMaxQuality;
	char cwMinQuality;
}tVIDEO_QUALITY,*ptVIDEO_QUALITY;

typedef struct CAMERA_ATTRIBUTE
{
	short swAttribute;
	short swLevel;
}tCAMERA_ATTRIBUTE,*ptCAMERA_ATTRIBUTE;

typedef struct CAMERA_FLIPMIRROR
{
	short swFlip;
	short swMirror;
}tCAMERA_FLIPMIRROR,*ptCAMERA_FLIPMIRROR;

typedef struct VIDEO_ATTRIBUTE
{
	short swMode;
	short swAttribute;
}tVIDEO_ATTRIBUTE,*ptVIDEO_ATTRIBUTE;

typedef struct VIDEO_BITRATE
{
	unsigned int lwRxAverageRate;
	unsigned int lwRxMaxRate;
	unsigned int lwRxMinRate;
	unsigned int lwRxCurRate;
	unsigned int lwRxSecond;
	unsigned int lwTxAverageRate;
	unsigned int lwTxMaxRate;
	unsigned int lwTxMinRate;
	unsigned int lwTxCurRate;
	unsigned int lwTxSecond;

}tVIDEO_BITRATE,*ptVIDEO_BITRATE;

typedef struct VIDEO_YUV
{
	unsigned int lwTimeStamp;
	int lwScale;
	int lwAddr;
	int lwLen;
}tVIDEO_YUV,*ptVIDEO_YUV;

typedef struct VIDEO_W_YUV
{
	int 	lwScale;
	short 	swWidth;	
	short	swHeight;
	int 	lwAddr;
	
}tVIDEO_W_YUV,*ptVIDEO_W_YUV;

typedef struct PIP_IMAGE_PARA
{
	short swFormat;
	short swPage;
	short swWidth;
	short swHeight;
	int lwAddr;
	int lwLen;

}tPIP_IMAGE_PARA,*ptPIP_IMAGE_PARA;

typedef struct SCROLL_BAR
{
	unsigned int lwAddr;
	int lwLen;
	int lwAttribute;

}tSCROLL_BAR,*ptSCROLL_BAR;

typedef struct VB_MOTION_PARA
{
	int lwSense;
	unsigned int lwVectorAddr;
}tVB_MOTION_PARA,*ptVB_MOTION_PARA;

#define VIDEO_GET_FRAME_SIZE		_IO  ('v', 0x80)
#define VIDEO_START 			_IOW ('v', 0x81, int)
#define VIDEO_STOP			_IOW ('v', 0x82, int)
#define VIDEO_SET_QUALITY		_IOW ('v', 0x83, struct tVIDEO_QUALITY *) 
#define VIDEO_GET_QUALITY		_IOR ('v', 0x84, struct tVIDEO_QUALITY *)
#define VIDEO_SET_IMAGE_SIZE		_IOW ('v', 0x85, struct tVIDEO_SIZE *)
#define VIDEO_GET_IMAGE_SIZE 		_IOR ('v', 0x86, struct tVIDEO_SIZE *)
#define VIDEO_SET_SENSE			_IOW ('v', 0x87, struct tVB_MOTION_PARA *)
//#define VIDEO_SET_OBJECT_SIZE 		_IOW ('v', 0x89, int)
//#define VIDEO_GET_OBJECT_SIZE  		_IO  ('v', 0x8a)
#define VIDEO_SET_VIN_FRAME_RATE		_IOW ('v', 0x8b, int)
#define VIDEO_GET_VIN_FRAME_RATE		_IO  ('v', 0x8c)
#define VIDEO_SET_INTRA_FRAME_RATE 	_IOW ('v', 0x8d, int)
#define VIDEO_GET_INTRA_FRAME_RATE	_IO ('v', 0x8e)
#define VIDEO_SET_CODEC			_IOW ('v', 0x8f, int)
#define VIDEO_GET_CODEC			_IO  ('v', 0x90)
#define VIDEO_SET_FORMAT		_IOW ('v', 0x91, struct tVIDEO_FORMAT *)
#define VIDEO_GET_FORMAT		_IOR ('v', 0x92, struct tVIDEO_FORMAT *)
//#define VIDEO_SET_ROBOT		_IOW ('v', 0x93, struct tVIDEO_ROBOT *)
//#define VIDEO_GET_TEMPERATURE 	_IO  ('v', 0x94)
#define VIDEO_SET_CAMERA_REG		_IOW ('v', 0x95, struct tCAMERA_REG *)
#define VIDEO_GET_CAMERA_REG		_IOR ('v', 0x96, struct tCAMERA_REG *)
#define VIDEO_SET_CBR				_IOW ('v', 0x97, int)
#define VIDEO_GET_CBR				_IO  ('v', 0x98)
#define VIDEO_SET_VOUT_FRAME_RATE		_IOW ('v', 0x99, int)
#define VIDEO_GET_VOUT_FRAME_RATE		_IO  ('v', 0x9a)
//#define VIDEO_GET_DECODE_STATUS		_IO  ('v', 0x9b)
#define VIDEO_RESET_DECODE		_IO  ('v', 0x9c)
//#define VIDEO_ENABLE_MOTION		_IOW ('v', 0x9d, int) 
//#define VIDEO_CHECK_MOTION		_IO  ('v', 0x9e)
#define VIDEO_GET_ALARM			_IO  ('v', 0x9f)
//#define VIDEO_LED_ON				_IO  ('v', 0xa0) //unused
//#define VIDEO_LED_OFF				_IO  ('v', 0xa1) //unused
#define VIDEO_CHECK_ENABLE  		_IO  ('v', 0xa2)
#define VIDEO_SELECT_NPMODE		_IOW ('v', 0xa3, struct tVIDEO_ATTRIBUTE *)
#define VIDEO_SET_CAM_ATTRIBUTE		_IOW ('v', 0xa4, struct tCAMERA_ATTRIBUTE *)
#define VIDEO_ENABLE_CAM_FLIPMIRROR	_IOW ('v', 0xa5, struct tCAMERA_FLIPMIRROR *)
#define VIDEO_GET_BITRATE			_IOR ('v', 0xa6, struct tVIDEO_BITRATE *)
#define VIDEO_FILL_BMP			_IOW ('v', 0xa7, struct tBMP_PARA *)
#define VIDEO_CLR_SCREEN			_IOW ('v', 0xa8, struct tSCREEN_PARA *)
#define VIDEO_SET_DISPLAY_MODE		_IOW ('v', 0xa9, struct tVIDEO_ATTRIBUTE *)
#define VIDEO_CONTROL_INTERLACED	_IOW ('v', 0xaa, struct tVIDEO_ATTRIBUTE *)
//#define VIDEO_SELECT_CAMERA		_IOW ('v', 0xab, int)
#define VIDEO_DRAW_LINE 			_IOW ('v', 0xac, struct tLINE_PARA *)
#define VIDEO_FILL_COLOR			_IOW ('v', 0xad, struct tSCREEN_PARA *)
#define VIDEO_SET_PALETTE			_IOW ('v', 0xae, void *)
#define VIDEO_CHANGE_PAGE			_IOW ('v', 0xaf, int)
//#define VIDEO_SET_CLOCK_ORIGIN		_IOW ('v', 0xb0, int)
#define VIDEO_ENABLE_CHECKSUM		_IOW ('v', 0xb1, int)
#define VIDEO_GET_YUV			_IOR ('v', 0xb2, struct tVIDEO_YUV *)
#define VIDEO_PIP_FILL_IMAGE		_IOW ('v', 0xb3, struct tPIP_IMAGE_PARA *)
#define VIDEO_PIP_DISPLAY_IMAGE		_IOW ('v', 0xb4, int)
#define VIDEO_SET_SYNC_MODE		_IOW ('v', 0xb5, int)
#define VIDEO_SWITCH_IMAGE		_IOW ('v', 0xb6, int)
#define VIDEO_ANALOG_ENCODE_MODE	_IOW ('v', 0xb7, int)
#define VIDEO_SET_HALF_IMAGE		_IOW ('v', 0xb8, int)
#define VIDEO_WRITE_STRING		_IOW ('v', 0xb9, struct tSTRING_PARA *)
#define VIDEO_SET_AVSYNC_THRESHOLD	_IOW ('v', 0xba, int)
#define VIDEO_DEFAULT_CAMERA		_IO ('v', 0xbb)
//#define VIDEO_GET_CAMERA_ID		_IO ('v', 0xbc)	
//#define VIDEO_ENABLE_ALARM		_IOW ('v', 0xbd, struct tALARM_PARA *) //unused
#define VIDEO_SET_SCROLL_BAR		_IOW ('v', 0xbe, struct tSCROLL_BAR *)
//#define VIDEO_MOTION_ALARM		_IOW ('v', 0xbf, struct tALARM_PARA *) //unused
#define VIDEO_SET_YUV			_IOW ('v', 0xc0, struct tVIDEO_W_YUV *)
#define VIDEO_SET_RVO_DISPLAY_MODE	_IOW ('v', 0xc1, int)
#define VIDEO_SET_MOTION_FRAME_RATE	_IOW ('v', 0xc2, int)
#define VIDEO_ENABLE_DE_BLOCKING	_IOW ('v', 0xc3, int)
#define VIDEO_ENABLE_FIND_CAMERA	_IOW ('v', 0xc4, int)
#define VIDEO_GET_MOTION_STATUS		_IOR ('v', 0xc5, char *)
#define VIDEO_SET_PLAYBACK_SPEED	_IOW ('v', 0xc6, int)
#define VIDEO_PAUSE_VOUT			_IOW ('v', 0xc7, int)


typedef struct DVR2835_PARA
{
	char cwInMode;
	char cwOutMode;
	short swChannal;
	int  lwPath;
}tDVR2835_PARA,*ptDVR2835_PARA;


typedef struct BITMAP_PARA
{
	unsigned int lwAddr;
	unsigned short swXpos;
	unsigned short swYpos;
	unsigned short swHeight;
	unsigned short swWidth;
	int  lwPath;
}tBITMAP_PARA,*ptBITMAP_PARA;


typedef struct OUTMODE_PARA
{
	char cwOutMode;
	char cwChannal;
	short swPath;
}tOUTMODE_PARA,*ptOUTMODE_PARA;




typedef struct MOTION_PARA
{
	short swChannel;
	unsigned char cwLevelSense;
	unsigned char cwSpeedSense;
	unsigned char cwTimeSense;
	unsigned char cwSpaceSense;
	short swPath;
	unsigned int lwVectorAddr;
}tMOTION_PARA,*ptMOTION_PARA;

typedef struct PLAYBACK_PARA
{
	char cwOutMode;
	char cwOutChannel;
	char cwInChannel;
	char cwAttribute;

}tPLAYBACK_PARA,*ptPLAYBACK_PARA;

typedef struct COLOR_INDEX
{
	unsigned int lwIndexAddr;
	int lwPath;
}tCOLOR_INDEX,*ptCOLOR_INDEX;

typedef struct PAGE_PARA
{
	short swPage;
	short swPath;
}tPAGE_PARA,*ptPAGE_PARA;

typedef struct OSD_ATTRIBUTE
{
	char cwChannel;
	char cwAttribute;
	char cwLevel;
	char cwPath;
}tOSD_ATTRIBUTE,*ptOSD_ATTRIBUTE;

typedef struct DBOX_DISPLAY
{
	short swChannel;
	unsigned short swXpos;
	unsigned short swYpos;
	unsigned short swHeight;
	unsigned short swWidth;
	unsigned char  cwBoxNum;
	unsigned char  cwColor;
	unsigned short swAttribute;
	short swPath;

}tDBOX_DISPLAY,*ptDBOX_DISPLAY;

typedef struct DVR_AUDIO_GAIN
{
	short swChannel;
	unsigned short swAttribute;
}tDVR_AUDIO_GAIN,*ptDVR_AUDIO_GAIN;

typedef struct DVR_AUDIO_LIVE
{
	int swChannel;
	unsigned int lwAttribute;
}tDVR_AUDIO_LIVE,*ptDVR_AUDIO_LIVE;

typedef struct DVR_AUDIO_RECORD
{
	short swChannel;
	unsigned short swAttribute;
	unsigned char cwSeq[8];

}tDVR_AUDIO_RECORD,*ptDVR_AUDIO_RECORD;

#define DVR_INIT_2835		_IOW ('d', 0x80, struct tDVR2835_PARA *)
#define DVR_DRAW_LINE 		_IOW ('d', 0x81, struct tLINE_PARA *)
#define DVR_FILL_COLOR		_IOW ('d', 0x82, struct tSCREEN_PARA *)
//#define DVR_FILL_BITMAP		_IOW ('d', 0x83, struct tBITMAP_PARA *) 
#define DVR_WRITE_STRING		_IOW ('d', 0x84, struct tSTRING_PARA *)
#define DVR_SET_OUTMODE		_IOW ('d', 0x85, struct tOUTMODE_PARA *)
#define DVR_GET_REMOTE		_IO  ('d', 0x86)
#define DVR_FILL_BMP		_IOW ('d', 0x87, struct tBMP_PARA *)
#define DVR_CONTROL_LED		_IOW ('d', 0x88, int)
#define DVR_CONTROL_BUZZER	_IOW ('d', 0x89, int)
//#define DVR_ENABLE_ALARM	_IOW ('d', 0x8a, struct tALARM_PARA *) //unused
//#define DVR_GET_ALARM_STAT	_IO  ('d', 0x8b) //unused
//#define DVR_GET_PULSE_COUNT	_IO  ('d', 0x8c) //unused
#define DVR_CLR_SCREEN		_IOW ('d', 0x8d, struct tSCREEN_PARA *)
#define DVR_GET_KEYPAD		_IO  ('d', 0x8e)
#define DVR_MOTION_DETECTION	_IOW ('d', 0x8f, struct tMOTION_PARA *)
#define DVR_GET_MOTION_STATUS	_IOR ('d', 0x90, char *)
#define DVR_PLAYBACK		_IOW ('d', 0x91, struct tPLAYBACK_PARA *)
#define DVR_SET_COLOR		_IOW ('d', 0x92, struct tCOLOR_INDEX *)
#define DVR_CHANGE_PAGE		_IOW ('d', 0x93, struct tPAGE_PARA *)
#define DVR_SET_OSD_ATTRIBUTE	_IOW ('d', 0x94, struct tOSD_ATTRIBUTE *)
#define DVR_2DBOX_DISPLAY		_IOW ('d', 0x95, struct tDBOX_DISPLAY *)
#define DVR_SET_AUDIO_GAIN	_IOW ('d', 0x96, struct tDVR_AUDIO_GAIN *)
#define DVR_SET_AUDIO_LIVE	_IOW ('d', 0x97, struct tDVR_AUDIO_LIVE *)
#define DVR_SET_AUDIO_RECORD	_IOW ('d', 0x98, struct tDVR_AUDIO_RECORD *)
#define DVR_DETECT_LINK		_IOW ('d', 0x99, int)
#define DVR_DETECT_BLIND		_IOW ('d', 0x9a, int)
#define DVR_DETECT_NIGHT		_IOW ('d', 0x9b, int)
#define DVR_GET_DETECT_STATUS	_IO  ('d', 0x9c)
#define DVR_SET_BLEND_MODE	_IOW ('d', 0x9e, int)



#endif
