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

#include <iostream>
#include <string>
#include <sysexits.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <dbus-c++-1/dbus-c++/dbus.h>
#include "config.h"
#include "utility.h"
#include "typedef.h"
#include "dbusThread.h"


//
// SingleonProcess is used to enusre that only one application instance is running in the system.
// Used to store the pid of ir-daemon */
const char g_SingletonFileName[] = "/tmp/hello-daemon-singleton"; 

//
// Process Start.
//
DBusThread g_dbusThread("DBusThread");
bool g_stopFlag = false;


void sighandler(int sig)
{
    /* Stop the MainThread and it's sub thread  */
    g_stopFlag = true;
}

STATUS main(int argc, char** argv)
{
    
    SingletonProcess singletonProcess(g_SingletonFileName);

    /* Only one instance for this application */
    if (singletonProcess.isSingleton() == false)
    {
        fprintf(stderr, "Another instance is already running.\n");
        exit(1);
   }

    g_dbusThread.start();

//    return (0);
    // Signal Handlers
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    for (;;)
    {
        if (g_stopFlag == true)
        {
            logInfo("Quit process...\n");
            break;
        }

        sleep(1);
    }
    
    g_dbusThread.stop();
}
