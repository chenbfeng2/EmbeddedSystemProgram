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
#include <math.h>

#define NSEC_PER_SEC 1000000000ULL

static struct timespec r;
static long long int period;

//add a specific time slot to the current time
static inline void timespec_add_us(uint64_t d)
{
    d *= 1000;
    d += r.tv_nsec;
    //printf("%lld\n",d);
    //check if the nanosecond is overflowed.
    while (d >= NSEC_PER_SEC) {
        d -= NSEC_PER_SEC;
	r.tv_sec = r.tv_sec+1;
    }
    r.tv_nsec = d;
    //printf("%d,%lld\n",r.tv_sec,r.tv_nsec);
}

//calculate the time difference of the timeout.
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

//print the time out every 200ms
static long long int wait_next_activation(int i)
{	int s=0;
	struct timespec start;
	struct timespec end;
	long long int temp=0;
	//get the real time clock start time
	clock_gettime(CLOCK_REALTIME,&start);
	//time out for 200ms
    s=clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &r, NULL);
    //record the time out stop time
    clock_gettime(CLOCK_REALTIME,&end);
    //print the time out information
    if (s==0)
    	{
    	printf("timer called %d!\n",i);
    	}

    //clock_gettime(CLOCK_REALTIME,&end);
    temp=diff(start,end);
    timespec_add_us(period);
    return temp;
}

//initial the timer
static int start_periodic_timer(uint64_t offs, int t)
{
    clock_gettime(CLOCK_REALTIME, &r);
    timespec_add_us(offs);
    period = t;
    return 0;
}

void timerthread(void)
{
    int i=0;
    long long int s=200000;
    unsigned long long int differ[100];
	unsigned long long int sum=0;
	long long int sd=0;
    uint64_t ss=200000;
    start_periodic_timer(ss,s);
    //calculate the time difference for every interaction.
    for (i=0;i<100;i++) {
    	differ[i]=wait_next_activation(i);
    	printf("%lld\n",differ[i]);
    }
    //Calculate the average timeout.
    for (i=0;i<100; i++)
    {
    	sum+=differ[i];
    }
    printf("average time out %lld\n",sum/100);

    //calculate the standard deviation.
    for (i=0;i<100;i++)
    {
    	sd+=(differ[i]-sum/100)*(differ[i]-sum/100);
    }
    long long int result=0;
    result= sqrt(sd);
    printf("stand divireation %lld\n",result);
}


int main()
{
    pthread_t timerA;
    
    pthread_create(&timerA, NULL, &timerthread,NULL);
    
    /* Wait for timer_func to be called five times - note that
     * since we're using signals, sem_wait may fail with EINTR.
     */
    pthread_join(timerA, 0);
    return 0;
    
}
