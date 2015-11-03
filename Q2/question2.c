/*
 * question2.c
 *
 *  Created on: Nov 10, 2014
 *      Author: chenbfeng2
 */


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
static long long int period=2000000;

//add a specific time slot to the current time
static inline void timespec_add_us(uint64_t d)
{
    d *= 1000;
    d += r.tv_nsec;
    //check if the nanosecond is overflowed.
    while (d >= NSEC_PER_SEC) {
        d -= NSEC_PER_SEC;
	r.tv_sec = r.tv_sec+1;
    }
    r.tv_nsec = d;
}



int main()
{
	//set the buffer
	setbuf(stdout,NULL);
	struct timespec start;
	int s=0;
  for(;;)
  {
	  //add 2s to current time
	  timespec_add_us(period);
	  //sleep the timer for 2s
	  s=clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &r, NULL);
	  //get and print out the current time.
	  clock_gettime(CLOCK_REALTIME,&r);
	  printf("the current time is %d %d\n",r.tv_sec,r.tv_nsec);

  }
    return 0;

}
