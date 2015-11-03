/*
 * An example of how to use signals to implement a timer facility using
 * POSIX threads.
 *
 * Rationale: when using normal Unix signals (SIGALRM), the signal handler executes
 * in a signal handler context.  In a signal handler context, only async-signal-safe
 * functions may be called.  Few POSIX functions are async-signal-safe.
 *
 * Instead of handling the timer activity in the signal handler function,
 * we create a separate thread to perform the timer activity.
 * This timer thread receives a signal from a semaphore, which is signaled ("posted")
 * every time a timer signal arrives.
 *
 * You'll have to redefine "timer_func" below and "set_periodic_timer."
 *
 * Read the man pages of the functions used, such as signal(2), signal(7),
 * alarm(2).
 *
 * Written by Godmar Back <gback@cs.vt.edu>, February 2006.
 */

/* required header files */
#include <pthread.h>
#include <semaphore.h>  /* semaphore operations */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>

/* forward declaration: user-defined timer function */
static void timer_func();

/* ----------------------------------------------------------------------------------------
 * begin provided code part
 * variables needed for timer
 */
static sem_t timer_sem;		/* semaphore that's signaled if timer signal arrived */
static unsigned int timer_stopped;	/* non-zero if the timer is to be timer_stopped */
static pthread_t timer_thread;	/* thread in which user timer functions execute */

/* Timer signal handler.
 * On each timer signal, the signal handler will signal a semaphore.
 */
static void
timersignalhandler(int sig)
{
	/* called in signal handler context, we can only call
	 * async-signal-safe functions now!
	 */
	sem_post(&timer_sem);	/* the only async-signal-safe function pthreads defines, same as V(sem) */
}

//calculate the difference between the time out start and end time.
long long int diff(struct timespec start, struct timespec end)
{
	long long int temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp = (end.tv_sec-start.tv_sec-1)*1000000000;
		temp += 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp = (end.tv_sec-start.tv_sec)*1000000000;
		temp+=end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

/* Timer thread.
 * This dedicated thread waits for posts on the timer semaphore.
 * For each post, timer_func() is called once.
 *
 * This ensures that the timer_func() is not called in a signal context.
 */
static void *
timerthread(void *_)
{
	struct timespec start;
	struct timespec end;
	unsigned long long int differ[100];
	unsigned long long int sum=0;
	long long int sd=0;
	int i=0;
	while (!timer_stopped) {
		clock_gettime(CLOCK_REALTIME,&start);
		int rc = sem_wait(&timer_sem);		/* same as P(sem) */
		if (rc == -1 && errno == EINTR)     /* call failed due to interrupt, retry */
		    continue;
		if (rc == -1) {                     /* called due to something else */
		    printf("timerthread failed on sem_wait\n");
		    exit(-1);
		}
		//clock_gettime(CLOCK_REALTIME,&start);
		timer_func();	/* user-specific timerfunc, can do anything it wants */
		clock_gettime(CLOCK_REALTIME,&end);
    	differ[i]=diff(start,end);
    	printf("%lld\n",differ[i]);
    	i++;
	}
		//calculate the average timeout duration
	    for (i=0;i<100; i++)
	    {
	    	sum+=differ[i];
	    }
	    printf("average time out %lld\n",sum/100);

	    //calculate the standard deviation of the time out duration
	    for (i=0;i<100;i++)
	    {
	    	sd+=(differ[i]-sum/100)*(differ[i]-sum/100);
	    }
	    long long int result=0;
	    result= sqrt(sd);
	    printf("stand divireation %lld\n",result);
	return 0;
}

/* Initialize timer */
void
init_timer(void)
{
	/* One time set up */
	sem_init(&timer_sem, /*not shared*/ 0, /*initial value*/0); /* init semaphore */
	pthread_create(&timer_thread, (pthread_attr_t*)0, timerthread, (void*)0); /* create timer thread */
	signal(SIGALRM, timersignalhandler); /* set up SIGALRM signal handler */
}

/* Shut timer down */
void
shutdown_timer()
{
	timer_stopped = 1;
	sem_post(&timer_sem); /* release timer semaphore */
	pthread_join(timer_thread, 0); /* wait on timer thread to terminate */
}

/* Set a periodic timer.  You may need to modify this function. */
void
set_periodic_timer(long delay)
{
	struct itimerval tval = { /* This syntax is to initialize fields of the struct */
		/* subsequent firings */ .it_interval = { .tv_sec = 0, .tv_usec = delay },
		/* first firing */       .it_value = { .tv_sec = 0, .tv_usec = delay }
	};

	setitimer(ITIMER_REAL, &tval, (struct itimerval*)0); /* set up periodic timer */
}

/* end provided code part */
/*----------------------------------------------------------------------------------------*/

/* application-specific part
 *
 * The following is a simple demonstration of how to use these timers.
 */

static sem_t demo_over;	/* a semaphore to stop the demo when done */

/* Put your timer code in here */
static void
timer_func()
{
	/* The code below is just a demonstration. */
	static int i = 0;

	printf ("Timer called %d!\n", i);
	if (++i == 100) {
		shutdown_timer();
		sem_post(&demo_over);	/* signal main thread to exit */
	}
}

int
main()
{
	sem_init(&demo_over, /*not shared*/ 0, /*initial value*/0);

	init_timer();
	set_periodic_timer(/* 200ms */200000);

	/* Wait for timer_func to be called five times - note that
	 * since we're using signals, sem_wait may fail with EINTR.
     */
	while (sem_wait(&demo_over) == -1 && errno == EINTR)
	    continue;
	return 0;
}
