////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <Sys/Run.h>

void timer_handler (int signum)
{
 static int count = 0;
 printf ("timer expired %d times\n", ++count);
}

int main ()
{
 struct sigaction sa;
 struct itimerval timer;

 /* Install timer_handler as the signal handler for SIGVTALRM. */
 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &timer_handler;
 //sigaction (SIGVTALRM, &sa, NULL);
 sigaction (SIGALRM, &sa, NULL);

 /* Configure the timer to expire after 250 msec... */
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = 250000;
 /* ... and every 250 msec after that. */
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = 250000;
 /* Start a virtual timer. It counts down whenever this process is
   executing. */
 //setitimer (ITIMER_VIRTUAL, &timer, NULL);
 setitimer (ITIMER_REAL, &timer, NULL);

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 0;
}