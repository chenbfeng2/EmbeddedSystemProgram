/*
 * question1.c
 *
 *  Created on: Nov 10, 2014
 *      Author: chenbfeng2
 */


#include <stdio.h>
#include <pthread.h>

static pthread_t main_id;
static pthread_t first_id;
static pthread_t second_id;
static pthread_mutex_t mymutex;
static int sum;
static void *body1(void)
{

	int adder=2;
	//check the value every time before adding
	while(sum<300)
	{
		usleep(100);
	  //lock the mutex to update the sum
	  pthread_mutex_lock(&mymutex);
	  //update sum and print the current value
	  sum+=adder;
	  printf("thread 1 add 2 to sum\n");
	  printf("sum=%d\n",sum);
	  //unlock the mutex
	  pthread_mutex_unlock(&mymutex);
	}
    return 0;
}
static void *body2(void)
{

	int adder=3;
	//check the value every time before adding
	while(sum<300)
	{
		usleep(100);
		//lock the mutex to update the sum
	  pthread_mutex_lock(&mymutex);
	  //update sum and print the current value
	  sum+=adder;
	  printf("thread 2 add 3 to sum\n");
	  printf("sum=%d\n",sum);
	  //unlock the mutex
	  pthread_mutex_unlock(&mymutex);
	}
    return 0;
}

int main(int argc, char *argv[])
{
    pthread_attr_t myattr1;
    pthread_attr_t myattr2;
    pthread_mutexattr_t mymutexattr;
    //set up the main thread
    struct sched_param param;
    param.sched_priority=99;
    main_id = pthread_self();
    pthread_setschedparam(main_id,SCHED_RR,&param);
    puts("main: before pthread_create\n");
    //set up the child thread parameters.
    struct sched_param param2;
    param2.sched_priority = 10;
    struct sched_param param3;
    param3.sched_priority = 10;
    pthread_attr_init(&myattr2);
    pthread_attr_init(&myattr1);

    //set up the scheduling policy.
    pthread_attr_setinheritsched(&myattr1, PTHREAD_EXPLICIT_SCHED);
   	pthread_attr_setinheritsched(&myattr2, PTHREAD_EXPLICIT_SCHED);
   	pthread_attr_setschedpolicy(&myattr1,SCHED_RR);
   	pthread_attr_setschedpolicy(&myattr2, SCHED_RR);
    pthread_attr_setschedparam(&myattr1, &param2);
    pthread_attr_setschedparam(&myattr2, &param3);

    //set up the mutex
    pthread_mutexattr_init(&mymutexattr);
    pthread_mutex_init(&mymutex, &mymutexattr);
    pthread_mutexattr_destroy(&mymutexattr);

    //creat the thread
    pthread_create(&first_id, &myattr1, body1, NULL);
    pthread_create(&second_id, &myattr2, body2, NULL);
    /* the thread attribute is no more needed */
    pthread_attr_destroy(&myattr1);
    pthread_attr_destroy(&myattr2);

    //join the thread after finish
    pthread_join(first_id, NULL);
    pthread_join(second_id,NULL);
    printf("main: after pthread_create\n");


    return 0;
}
