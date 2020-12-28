//####
//# Legal Notice :
//#    Copyright, AMX LLC, 2009
//#    Private, proprietary information, the sole property of AMX LLC.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this AMX Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    AMX Software.
//#
//#    This AMX Software is owned by AMX and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this AMX Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. AMX is
//#    not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This AMX Software is provided with restricted rights. Use, duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "commonLib.h"
/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "MIMX8MM6_cm4.h"
#include "userCommand.h"
#include "shellFrame.h"
#include "evkApp.h"
#include "fsl_gpt.h"
#include "ir.h"
#include "fsl_uart.h"
#include "rsc_table.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RPMSG_LITE_SHMEM_BASE         (VDEV0_VRING_BASE)
#define RPMSG_LITE_LINK_ID            (RL_PLATFORM_IMX8MM_M4_USER_LINK_ID)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-virtual-tty-channel-1"
#define APP_TASK_STACK_SIZE (256)
#ifndef LOCAL_EPT_ADDR
#define LOCAL_EPT_ADDR (30)
#endif

#if defined RPMSG_DEBUG
void rpmsg_sendTest(UINT8 frameIndex, int lengthInByte);
#endif

/* Globals */
static char app_buf[512]; /* Each RPMSG buffer can carry less than 512 payload */

/* The following code is for IR issue */
GPIOConfigType s_GPIOConfig[] = 
{	
	// Config GPIO5-11 as output.
	{GPIO5, 11U, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode } },

	// Config GPIO1-13 as input, with no interrupt.
	{GPIO1, 13U, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode } },

	// Config GPIO5-12 as input, with no interrupt
	{GPIO5, 12U, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode } },

};

/* Task priorities. */
#define user_command_task_PRIORITY (configMAX_PRIORITIES - 1)
#define TICK_PER_SECOND	configTICK_RATE_HZ

static int s_addCounter = 0;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void userCommandTask(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
 
void dumpGPT(int i)
{
	/* There are 6 Group of GPTs:  */
	GPT_Type * gpt[] = GPT_BASE_PTRS;

	if ((i < 1) || ( i > 6))
	{
		PRINTF("Invalid GPT index: %d\r\n", i);
		return;
	}
	
	GPT_Type *pGPT = gpt[i];
	PRINTF("\r\nGPT%d = 0x%x\r\n", i, pGPT);
	PRINTF("    CR = 0x%x\r\n", pGPT->CR);
	if (pGPT->CR & GPT_CR_EN_MASK)
	{
		PRINTF("    CLOCK Source = %d\r\n", GPT_GetClockSource(pGPT));
		PRINTF("    CLOCK Divider = %d\r\n", GPT_GetClockDivider(pGPT));
		PRINTF("    OSC CLOCK Divider = %d\r\n", GPT_GetOscClockDivider(pGPT));
		PRINTF("    Current Time Counter = %d\r\n", GPT_GetCurrentTimerCount(pGPT));
	}
}

void initGPIO()
{
	int index;
	int gpioPinCount = sizeof(s_GPIOConfig) / sizeof(GPIOConfigType);
	for (index = 0; index < gpioPinCount; index++)
	{
		GPIO_PinInit(s_GPIOConfig[index].base, s_GPIOConfig[index].pin, &s_GPIOConfig[index].pinConfig);
	}
}

UINT32 getGPIO(int group, int pin)
{
	GPIO_Type *gpioBase[] = {(GPIO_Type *)0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5}; 
	UINT32 value;
	
	if ((group < 1) || (group > 5))
	{
		PRINTF("Invalid group: %d\r\n", group);
		return (UINT32)0xFFFFFFFF;
	}
	if ((pin < 0) || (pin > 31))
	{
		PRINTF("Invalid pin: %d\r\n", pin);
		return (UINT32)0xFFFFFFFF;
	}

	value = GPIO_PinRead(gpioBase[group], pin);
	return (value);
}

void setGPIO(int group, int pin, int value)
{
	GPIO_Type *gpioBase[] = {(GPIO_Type *)0, GPIO1, GPIO2, GPIO3, GPIO4, GPIO5}; 
	if ((group < 1) || (group > 5))
	{
		PRINTF("Invalid group: %d\r\n", group);
		return;
	}
	if ((pin < 0) || (pin > 31))
	{
		PRINTF("Invalid pin: %d\r\n", pin);
		return;
	}
	if ((value != 0) && (value != 1))
	{
		PRINTF("Invalid value: %d\r\n", value);
		return;
	}
	GPIO_PinWrite(gpioBase[group], (UINT32)pin, (UINT8)value);
}


/*   
	For iMX8MMEVK, index = 2, driver = 24. Which means the GPT2 is 24MHz.
	For index, we need to run dumpGPT to find out an idle GPT.
	For divider, we need to run tickTask to find out the exact frequency.
*/
GPT_Type * s_gpt[] = GPT_BASE_PTRS;
int s_gptIndex = 2;

STATUS initGPT(int index, int divider)
{
	gpt_config_t gptConfig;
	GPT_Type *pGPT;

	if ((index < 1) || ( index > 6))
	{
		PRINTF("Invalid GPT index: %d\r\n", index);
		return (STATUS_ERROR);
	}
	pGPT = s_gpt[index];

    GPT_GetDefaultConfig(&gptConfig);
	gptConfig.divider = divider;
    /* Initialize GPT module */
    GPT_Init(pGPT, &gptConfig);

    /* Divide GPT clock source frequency by 24 inside GPT module */
//    GPT_SetClockDivider(pGPT, divider);

	GPT_StartTimer(pGPT);

	return (STATUS_OK);
}


/*==========================================
timer service

non-blocking
==========================================*/

UINT32 getTimerCounter(void)
{
	GPT_Type *pGPT = s_gpt[s_gptIndex];
	return (GPT_GetCurrentTimerCount(pGPT));
}

/*!
 Load timer with current system tick value.
 \param timerTicks pointer to timer.
 */
void timerLoad(UINT32 *timerTicks) 
{
  	*timerTicks = getTimerCounter();
}

/*!
 Check for timer expiration with optional reload.
 \param timerTicks pointer to timer.
 \param tickCount expiration limit.
 \param reload TRUE to reload timer on expiration, else FALSE.
 \return TRUE if timer expired, else FALSE.
 */
bool timerExpired(UINT32 *timerTicks, UINT32 tickCount, bool reload) 
{
	UINT32 currentTime = getTimerCounter();

	UINT32 timePassed = currentTime - *timerTicks;
	
  	if (timePassed >= tickCount) 
	{
    	if (reload) 
		{
      		*timerTicks = currentTime;
    	}
    	return true;
  	}
  	else 
	{
    	return false;
  	}
}

/* 
	This is for unit test 
	To run unit test, we must start user_command_task.
*/
static FuncMapType s_unitTestList[] = 
{
	{"setGPIO",	  		setGPIO, 
			"void setGPIO(int group=[1:5], int pin=[0:31], int value=[0|1])" },
	{"getGPIO", 		getGPIO, 
			"UINT32 getGPIO(int group=[1:5], int pin=[0:31])" },
	{"dumpGPT", 		dumpGPT, 
			"void dumpGPT(int GPTIndex=[1:6])" },
	{"initGPT", 		initGPT, 
			"void initGPT(int GPTIndex=[1:6], divider = [1:4096)" },
	{"getTimeCounter", 	getTimerCounter, 
	"void getTimeCounter()" },
//	{"setGPTIndex", 	setGPTIndex, 		"void setGPTIndex(int index=[1:6]);"},
};


void vApplicationIdleHook( void )
{
	static UINT32 IRTimerCounter = 0;
	if (timerExpired(&IRTimerCounter, 1 , TRUE))	
	{
		IRProc();
		s_addCounter++;
	}
}

void tickTask(void *pvParameters)
{
//	static UINT32 lastCounter = 0;
//	UINT32 currentCounter = 0;
	int nDelaySecond = 5;
	for (;;)
	{
		vTaskDelay(TICK_PER_SECOND * nDelaySecond);
//		currentCounter = getTimerCounter();

		PRINTF("vApplicationIdleHook called %d times - %d\r\n", s_addCounter, s_addCounter / nDelaySecond);
		s_addCounter = 0;
		
//		PRINTF("counter = %d - %d\r\n", currentCounter, (currentCounter - lastCounter) / nDelaySecond);
//		lastCounter = currentCounter;

//		dumpIRMessage(&s_ringBuffer);
	}
}

static TaskHandle_t app_task_handle = NULL;
static struct rpmsg_lite_instance *volatile s_rpmsgDevice;
static struct rpmsg_lite_endpoint *volatile s_rpmsgEndPoint;
static volatile uint32_t s_remoteAddr;
static volatile rpmsg_queue_handle s_rpmsgQueue;
	
static void app_task(void *param)
{

    void *rx_buf;
    uint32_t len;
    int32_t result;
#if defined RPMSG_LOOP_TEST	
    void *tx_buf;
    uint32_t size;
#endif

    /* Print the initial banner */
    PRINTF("\r\nRPMSG String Echo FreeRTOS RTOS API Demo...\r\n");

#ifdef MCMGR_USED
    uint32_t startupData;

    /* Get the startup data */
    (void)MCMGR_GetStartupData(kMCMGR_Core1, &startupData);

    s_rpmsgDevice = rpmsg_lite_remote_init((void *)startupData, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);

    /* Signal the other core we are ready */
    (void)MCMGR_SignalReady(kMCMGR_Core1);
#else
    s_rpmsgDevice = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);
#endif /* MCMGR_USED */

    while (0 == rpmsg_lite_is_link_up(s_rpmsgDevice))
        ;

    s_rpmsgQueue = rpmsg_queue_create(s_rpmsgDevice);
    s_rpmsgEndPoint   = rpmsg_lite_create_ept(s_rpmsgDevice, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, s_rpmsgQueue);
    (void)rpmsg_ns_announce(s_rpmsgDevice, s_rpmsgEndPoint, RPMSG_LITE_NS_ANNOUNCE_STRING, RL_NS_CREATE);

    PRINTF("\r\nNameservice sent, ready for incoming messages...\r\n");

    for (;;)
    {
        /* Get RPMsg rx buffer with message */
        result = rpmsg_queue_recv_nocopy(s_rpmsgDevice, s_rpmsgQueue, (uint32_t *)&s_remoteAddr, (char **)&rx_buf, &len, RL_BLOCK);
        if (result != 0)
        {
            assert(false);
        }

        /* Copy string from RPMsg rx buffer */
        assert(len < sizeof(app_buf));
        memcpy(app_buf, rx_buf, len);
        app_buf[len] = 0; /* End string by '\0' */

//        if ((len == 2) && (app_buf[0] == 0xd) && (app_buf[1] == 0xa))
//            PRINTF("Get New Line From Master Side\r\n");
//        else
//            PRINTF("Get Message From Master Side : \"%s\" [len : %d]\r\n", app_buf, len);

#if defined RPMSG_LOOP_TEST	
        /* Get tx buffer from RPMsg */
        tx_buf = rpmsg_lite_alloc_tx_buffer(s_rpmsgDevice, &size, RL_BLOCK);
		PRINTF("rpmsg_lite_alloc_tx_buffer size = %d\r\n", size);
        assert(tx_buf);
        /* Copy string to RPMsg tx buffer */
        memcpy(tx_buf, app_buf, len);
        /* Echo back received message with nocopy send */
        result = rpmsg_lite_send_nocopy(s_rpmsgDevice, s_rpmsgEndPoint, s_remoteAddr, tx_buf, len);
        if (result != 0)
        {
            assert(false);
        }
#endif	
        /* Release held RPMsg rx buffer */
        result = rpmsg_queue_nocopy_free(s_rpmsgDevice, rx_buf);
        if (result != 0)
        {
            assert(false);
        }

#if defined RPMSG_DEBUG		
		/* Let's do some test here */
		rpmsg_sendTest(0xFF, len);
#endif		
    }
}

UINT8 rpmsgCheckSum(UINT8 *data, int length)
{
	int i;
	UINT8 crc = 0;
	
	for (i = 0; i < length; i++)
	{
		crc = crc ^ data[i];
	}
	
	return (crc);
}

void rpmsg_sendReceivedIR(UINT8 frameIndex, UINT8 *receivedIRData, int sizeInByte)
{
    void *tx_buf;
	RpmsgType *rpmsg;
    int32_t result;
    uint32_t bufferSize;
	int sendMsgSize; 
	
	PRINTF("sendIR: %d, len = %d\r\n", frameIndex, sizeInByte >> 2);
	sendMsgSize = sizeof(RpmsgHeaderType) + sizeInByte;
	
    /* Get tx buffer from RPMsg */
    tx_buf = rpmsg_lite_alloc_tx_buffer(s_rpmsgDevice, &bufferSize, RL_BLOCK);
	if (bufferSize < sendMsgSize)
	{
		PRINTF("ERROR: Data is too large to be sent out! (%d > %d)\r\n", sendMsgSize, bufferSize);
		sizeInByte = bufferSize -  sizeof(RpmsgHeaderType);
	}
	
	/* Fill the RPMSG Header */
	rpmsg = tx_buf;
	rpmsg->m_header.m_sendModuleID = MODULE_ID_M4;
	rpmsg->m_header.m_recvModuleID = MODULE_ID_TTY_DRIVER;
	rpmsg->m_header.m_msgID = MSG_ID_IR_RECEIVE;
	rpmsg->m_header.m_msgSubID = frameIndex;
	rpmsg->m_header.m_msgDataLength = sizeInByte;
	
    /* Copy string to RPMsg tx buffer */
	rpmsg->m_header.m_msgDataCheckSum = rpmsgCheckSum((UINT8 *)receivedIRData, sizeInByte);
	memcpy((void *)rpmsg->m_data, receivedIRData, sizeInByte);

    /* Echo back received message with nocopy send */
    result = rpmsg_lite_send_nocopy(s_rpmsgDevice, s_rpmsgEndPoint, s_remoteAddr, tx_buf, sendMsgSize);
    if (result != 0)
    {
        assert(false);
    }
}

#if defined RPMSG_DEBUG
void dumpVirtqueue(struct virtqueue *queue)
{
	d((void *)queue, sizeof(struct virtqueue), 4, 0);
	PRINTF("virtualQueue = 0x%p\r\n", queue);
	PRINTF("    vq_alignment = %d\r\n", queue->vq_alignment);
	PRINTF("    vq_ring_size = %d\r\n", queue->vq_ring_size);
	PRINTF("	vq_ring->num = %d\r\n", queue->vq_ring.num);
	PRINTF("	vq_ring->desc = 0x%p\r\n", queue->vq_ring.desc);
	PRINTF("	vq_ring->avail = %p\r\n", queue->vq_ring.avail);
	PRINTF("	vq_ring->used = %p\r\n", queue->vq_ring.used);
	
	PRINTF("    vq_ring_mem = 0x%p\r\n", queue->vq_ring_mem);
	PRINTF("    vq_max_indirect_size = %d\r\n", queue->vq_max_indirect_size);
	PRINTF("    vq_indirect_mem_size = %d\r\n", queue->vq_indirect_mem_size);
	PRINTF("    vq_queue_index = %d\r\n", queue->vq_queue_index);
	PRINTF("    vq_available_idx = %d\r\n", queue->vq_available_idx);
	
}
void dump()
{
	d((void *)s_rpmsgDevice, sizeof(struct rpmsg_lite_instance), 4, 0);
	PRINTF("s_rpmsgDevice = 0x%p\r\n", s_rpmsgDevice);

	PRINTF("    alignment = 0x%p\r\n", s_rpmsgDevice->rvq);
	dumpVirtqueue(s_rpmsgDevice->rvq);
	PRINTF("\r\n");

	PRINTF("    tx vir queue = 0x%p\r\n", s_rpmsgDevice->tvq);
	dumpVirtqueue(s_rpmsgDevice->tvq);
	PRINTF("\r\n");


	PRINTF("    link_state = %d\r\n", s_rpmsgDevice->link_state);
	PRINTF("    linked list of endpoints = 0x%p\r\n", s_rpmsgDevice->rl_endpoints);
	PRINTF("    sh_mem_base = 0x%p\r\n", s_rpmsgDevice->sh_mem_base);
	PRINTF("    sh_mem_remaining = 0x%p\r\n", s_rpmsgDevice->sh_mem_remaining);
	PRINTF("    sh_mem_total = 0x%p\r\n", s_rpmsgDevice->sh_mem_total);
}

void rpmsg_sendTest(UINT8 frameIndex, int lengthInByte)
{
	UINT8 *buffer;
	
	buffer = malloc(lengthInByte);
	memset(buffer, lengthInByte, lengthInByte);
	rpmsg_sendReceivedIR(frameIndex, buffer, lengthInByte);
	free(buffer);
//	dump();
}
#endif
/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize standard SDK demo application pins */
    /* Board specific RDC settings */
    BOARD_RdcInit();

    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();

	/* Add user code here... */
	initGPIO();
	
	/* Use GPT2 as our IR Clock. the frequency is 1000000Hz */
	initGPT(2, 24);

	/* Add function list for unit test */
	addFunctionMap(s_unitTestList, sizeof(s_unitTestList) / sizeof(FuncMapType));
	initIRMessage(&g_IRRecvBuffer);

    copyResourceTable();

#ifdef MCMGR_USED
    /* Initialize MCMGR before calling its API */
    (void)MCMGR_Init();
#endif /* MCMGR_USED */



//	xTaskCreate(tickTask, "tick_task", configMINIMAL_STACK_SIZE + 90, NULL, user_command_task_PRIORITY, NULL);
//	xTaskCreate(userCommandTask, "user_command_task", configMINIMAL_STACK_SIZE + 1000, NULL, user_command_task_PRIORITY, NULL);


    if (xTaskCreate(app_task, "APP_TASK", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &app_task_handle) != pdPASS)
    {
        PRINTF("\r\nFailed to create application task\r\n");
        for (;;)
            ;
    }

    vTaskStartScheduler();

    PRINTF("Failed to start FreeRTOS on core0.\n");
    for (;;)
        ;
}
