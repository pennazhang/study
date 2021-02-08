//####
//# Legal Notice :
//#    Copyright, Harman International, 2020
//#    Private, proprietary information, the sole property of Harman.  The
//#    contents, ideas, and concepts expressed herein are not to be disclosed
//#    except within the confines of a confidential relationship and only
//#    then on a need to know basis.
//#
//#    Any entity in possession of this Harman Software shall not, and shall not
//#    permit any other person to, disclose, display, loan, publish, transfer
//#    (whether by sale, assignment, exchange, gift, operation of law or
//#    otherwise), license, sublicense, copy, or otherwise disseminate this
//#    Harman Software.
//#
//#    This Harman Software is owned by Harman and is protected by United States
//#    copyright laws, patent laws, international treaty provisions, and/or
//#    state of Texas trade secret laws.
//#
//#    Portions of this Harman Software may, from time to time, include
//#    pre-release code and such code may not be at the level of performance,
//#    compatibility and functionality of the final code. The pre-release code
//#    may not operate correctly and may be substantially modified prior to
//#    final release or certain features may not be generally released. Harman
//#    is not obligated to make or support any pre-release code. All pre-release
//#    code is provided "as is" with no warranties.
//#
//#    This Harman Software is provided with restricted rights. Use,duplication,
//#    or disclosure by the Government is subject to restrictions as set forth
//#    in subparagraph (1)(ii) of The Rights in Technical Data and Computer
//#    Software clause at DFARS 252.227-7013 or subparagraphs (1) and (2) of
//#    the Commercial Computer Software Restricted Rights at 48 CFR 52.227-19,
//#    as applicable.
//####
#pragma once

#include "typedef.h"
#include <iostream>
#include <chrono>
#include <unistd.h>

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

// enabled legacy mmap cir

#define MAX_IR_VALUES       4096

#define IR_PRONTO_RAW_OSCILLATED_CODE   0x0


#define IR_FIFO_SIZE    10

// This struct describe the first 4 Words of Pronto Code.
typedef struct 
{
    // Normally, the type should be 0.
    UINT32 m_type;
    
    // The frequency of carrier.
    UINT32 m_freq;
    
    // the pair count of the once sequence. 1 pair = 1 Pulse + 1 Space.
    UINT32 m_seq1Count;
    
    // The pair count of the repeat sequence.
    UINT32 m_seq2Count;
} ProntoCodeHeaderType;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
//! Single IR command to send (encoding of Raw Oscillated Code)
typedef struct
{
    //! Carrier frequency in pronto code
    UINT8 m_carryFreq;
    //! Duration of bits (in useconds)
    double m_periodInUs;

    //! Number of burst pairs in sequence #1
    unsigned short m_seq1Count;
    //! Number of burst pairs in sequence #2
    unsigned short m_seq2Count;

    //! sequence #1 array of burst pair words
    unsigned int m_seq1[MAX_IR_VALUES];
    //! sequence #2 array of burst pair words
    unsigned int m_seq2[MAX_IR_VALUES];

    //! Sequence #1 output pulse values
    unsigned int  m_seq1Out[MAX_IR_VALUES];
    //! Sequence #2 output pulse values
    unsigned int  m_seq2Out[MAX_IR_VALUES];
    
    int loadIRCommand(char const*irCmdBuf);

    void dump();
    
    MsgType * createIRBuffer();
    
    static int freqToHz(UINT8 prontoCodeFreq);  
} ProntoCodeType;

typedef enum
{
    IR_TEST_END = 0,
    IR_TEST_START,
    IR_TEST_SEND_IR,
    IR_TEST_RECV_IR,
} IRTestStatus;

class IRSettings
{
public:
    void getIrConfig();
    IRSettings();
    ~IRSettings();

    bool getRepeatFlag();
    void setRepeatFlag(bool flag);

    int getCommandHoldOff();
    void setCommandHoldOff(int commandHoldOff); 

    int getRepeatHoldOff();
    void setRepeatHoldOff(int commandHoldOff); 

    void setTestStatus(IRTestStatus status) { m_testStatus = status; }
    IRTestStatus getTestStatus() {  return m_testStatus; }

    void getTestResult(UINT32 &wordCount, UINT32 &totalTime);
    void setTestResult(MsgType *);

public:
    //! Enable or disable pass through function. 
    //! true: the pass through function is enabled.
    //! false: the pass through function is disabled.
    BOOL m_passThroughFlag;
    
    //! Peer IP address for pass through functoin.
    //! Only valid when pass through function is enabled.
    UINT8 m_irPassThroughPeerIP[4];
    
    unsigned char m_myip[4];

protected:
    //! Time to pause between the end of one command and the
    //! start of another (milliseconds)
    int m_timeCommandHoldoff;

    //! Time to pause between once sequence and repeat sequence (in milliseconds)
    unsigned int m_timeRepeatHoldoff;


    //! When send MSG_ID_IR_RAW_REPEAT, if the m_repeatFlag is set as TRUE, then the IR driver will send the IR info again and agina. 
    //! Only when m_repeatFlag is set as FALSE, then the IR driver will stop repeating the IR.
    BOOL m_repeatFlag;  

    // Just for DirectControl commands: testIR
    IRTestStatus m_testStatus;

    // Just for DirectControl commands: testIR
    // we only check the received word count and total time.
    UINT32 m_testWordCount, m_testTotalTime;

};

extern IRSettings g_irSetting;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
class TimePoint
{
public:
    TimePoint();
    void getCurrentTime();
    
    // Check for timer expiration with optional reload.
    // \param timeSpanInMs expiration limit in millseconds.
    // \param reload TRUE to reload timer on expiration, else FALSE.
    // \return TRUE if timer expired, else FALSE.
    //
    BOOL isTimeExpired(UINT32 timeSpanInMs, bool reload);

    // Get the time span between two TimePoint in ms. 
    // Here is an example of timeDiff:
    //  TimePoint beginTime;
    //  std::this_thread::sleep_for(std::chrono::milliseconds(1000));       
    //  TimePoint endTime;
    //
    //  beginTime.timeDiff(&endTime) = 1000
    //  endTime.timeDiff(&beginTime) = -1000;
    int timeDiff(TimePoint *anotherPoint);

protected:
    high_resolution_clock::time_point m_timePoint;
};

/* This class can ensure that there is only one instance of the application in the Linux system */
class SingletonProcess
{
public:
    SingletonProcess();
    ~SingletonProcess();
    bool isSingleton();
protected:
    bool m_singletonFlag;   
};


bool isFdReadable(int fd, int timeout_in_ms);

// strip the string from both header and tail. copied from Linux kernel.
char *strstrip(char *s);

bool getNameByPid(pid_t pid, char task_name[128]);

void dumpDecimal(UINT8 *data, int totalSizeInByte, int unitSizeInByte, int unitCountPerLine = 100);

void dumpIRRaw(MsgType* msg);

UINT32 getTotalTime(MsgType *msg);