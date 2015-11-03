/*
 *  ex_2lock.c
 *
 *  Created on: Sep 24, 2014
 *      Author: chenbfeng2
 */
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

static pthread_mutex_t mymutex1;
static pthread_mutex_t mymutex2;
static int th_cnt;

void *bodyA(void *arg)
{
    int i,j,k,p,q,r;
    int sum;

    //print information and do calculations
    printf("%s - lock(mutex1)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }
    //try to lock mutex1
    pthread_mutex_lock(&mymutex1);

    //print information and do calculations
    printf("%s - lock(mutex2)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }
    //try to lock mutex2
    pthread_mutex_lock(&mymutex2);

    //print information and do calculations
    printf("%s - unlock(mutex2)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to unlock mutex2
    pthread_mutex_unlock(&mymutex2);

    //print information and do calculations
    printf("%s - unlock(mutex1)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to unlock mutex1
    pthread_mutex_unlock(&mymutex1);

    //print information and do calculations
    printf("%s - thread A finish\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }


    return NULL;
}

void *bodyB(void *arg)
{
    int i,j,k,p,q,r;
    int sum;

    //print information and do calculations
    printf("%s - lock(mutex1)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to lock mutex2
    pthread_mutex_lock(&mymutex1);

    //print information and do calculations
    printf("%s - lock(mutex2)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to lock mutex1
    pthread_mutex_lock(&mymutex2);

    //print information and do calculations
    printf("%s - unlock(mutex2)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to unlock mutex1
    pthread_mutex_unlock(&mymutex2);

    //print information and do calculations
    printf("%s - unlock(mutex1)\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }

    //try to unlock mutex2
    pthread_mutex_unlock(&mymutex1);

    //print information and do calculations
    printf("%s - thread B finish\n",arg);
    for (i=0;i<100;i++)
    {
        sum=0;
        for (p=0;p<1000000; p++) {
            sum+=p;
        }
    }
    return 0;
}


int main()
{
    pthread_t A, B, main_id;
    pthread_attr_t attrA, attrB;
    char parameterA[]="A";
    char parameterB[]="B";

    main_id=pthread_self();


    struct sched_param paramA;
    struct sched_param paramB;
    struct sched_param param;

    //set the priority parameters
    param.sched_priority=99;
    paramA.sched_priority=1;
    paramB.sched_priority=1;

    //initial the two mutexes
    pthread_mutexattr_t mymutexattr1;

    pthread_mutexattr_init(&mymutexattr1);
    pthread_mutex_init(&mymutex1, &mymutexattr1);
    pthread_mutexattr_destroy(&mymutexattr1);

    pthread_mutexattr_t mymutexattr2;

    pthread_mutexattr_init(&mymutexattr2);
    pthread_mutex_init(&mymutex2, &mymutexattr2);
    pthread_mutexattr_destroy(&mymutexattr2);

    //set the priority schedule of the main thread
    pthread_setschedparam(main_id, SCHED_RR, &param);

    pthread_attr_init(&attrA);
    pthread_attr_init(&attrB);

    //set the priority policy and value of two child threads.
    pthread_attr_setinheritsched(&attrA, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attrA, SCHED_RR);
    pthread_attr_setschedparam(&attrA, &paramA);

    pthread_attr_setinheritsched(&attrB, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attrB, SCHED_RR);
    pthread_attr_setschedparam(&attrB, &paramB);


    //creat the threads
    pthread_create(&A, &attrA, bodyA, &parameterA);

    pthread_attr_destroy(&attrA);
    pthread_create(&B, &attrB, bodyB, &parameterB);
    pthread_attr_destroy(&attrB);

    //join the threads when finish
    if (pthread_join(A,NULL)!=0)
    {
        printf("Join thread A error!\n");
    }
    if (pthread_join(B,NULL)!=0)
    {
        printf("Join thread B error!\n");
    }
    return 0;
}

