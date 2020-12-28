#include <iostream>
#include <chrono>
#include <unistd.h>

typedef unsigned int 	UINT32;

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

typedef high_resolution_clock::time_point TimePointType;

void timeLoad(TimePointType *currentTime) 
{
  	*currentTime = high_resolution_clock::now();
}

/* return the time span in Mill_Second. */
UINT32 timeDiff(TimePointType *beginTime, TimePointType *endTime)
{
	milliseconds timeInterval = std::chrono::duration_cast<milliseconds>(*endTime - *beginTime);
	return (timeInterval.count());
}

/*!
 Check for timer expiration with optional reload.
 \param timerTicks pointer to timer.
 \param tickCount expiration limit.
 \param reload TRUE to reload timer on expiration, else FALSE.
 \return TRUE if timer expired, else FALSE.
 */
bool timeExpired(TimePointType *timerTicks, UINT32 tickCountInMs, bool reload) 
{
	TimePointType currentTime = high_resolution_clock::now();

	milliseconds timeInterval= std::chrono::duration_cast<milliseconds>(currentTime - *timerTicks);
	UINT32 timePassed = timeInterval.count();
	
  	if (timePassed >= tickCountInMs) 
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

int main()
{
    using namespace std;
    cout << "Hello, world!\n";


#ifdef _WIN32_WINNT
    cout << "_WIN32_WINNT = " << hex << _WIN32_WINNT << endl;
#endif
#ifdef _WIN32_WINDOWS
    cout << "_WIN32_WINDOWS = " << hex << _WIN32_WINDOWS << endl;
#endif
#ifdef WINVER 
    cout << "WINVER = " << hex << WINVER << endl;
#endif

#ifdef __linux__
	cout << "__linux__ = " << __linux__ << endl;
	
	TimePointType beginTime;
	timeLoad(&beginTime);
	
	for (int count = 0; count < 3; )
	{
		usleep(1000);
		if (timeExpired(&beginTime, 1000, true))
		{
			count++;
			printf("time expired = %d second\n", count);
		}
	}
#endif

    return 0;
}

