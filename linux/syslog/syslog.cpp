#include <iostream>
#include <syslog.h>
#include <unistd.h>

// Rin
int main()
{
	// Set log level to LOG_NOTICE.  The log Level should be: LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR, LOG_WARNING, LOG_NOTIVE, LOG_INFO, LOG_DEBUG  
	setlogmask (LOG_UPTO (LOG_NOTICE));

	openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog (LOG_ERR, "A tree falls in a forest.");
	syslog (LOG_NOTICE, "Program started by User %d.", getuid ());
	syslog (LOG_INFO, "But you can not see me, because I am under LOG_NOTICE");
	
	closelog ();

    return 0;
}

