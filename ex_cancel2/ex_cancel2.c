/*
 * ex_cancel2.c
 *
 *  Created on: Sep 18, 2014
 *      Author: chenbfeng2
 */


//
//  main.c
//  ex_prio
//
//  Created by CHENBO FENG on 9/16/14.
//  Copyright (c) 2014 CHENBO FENG. All rights reserved.
//


#include <stdio.h>
#include <pthread.h>

static pthread_t main_id;
static pthread_t first_id;
static unsigned int val;
void *body (void *arg)
{
    unsigned int i, j, k;

    for(j=0; j<100;j++)
    {
    	for(i = 0, val = 0;i < 1000000; i++){ val+=i; }
        printf("%s%d\n",arg,j);
    }
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
    for(j=0; j<800;j++)
    {
        for(i = 0, val = 0;i < 1000000; i++){ val+=i; }
        printf("%s%d\n",arg,j);
    }
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    for(k=0; k<100;k++)
    {
    	for(i = 0, val = 0;i < 1000000; i++){ val+=i; }
        printf("%s%d\n",arg,k);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_attr_t myattr1;
    int err;
    char *parameter1="A——thread";
    void *returnvalue;
    struct sched_param param;
    param.sched_priority=99;
    main_id = pthread_self();
    //pthread_setschedparam(main_id,SCHED_FIFO,&param);
    puts("main: before pthread_create\n");

    struct sched_param param1;
    param1.sched_priority = 1;
    pthread_attr_init(&myattr1);
    err = pthread_create(&first_id, &myattr1, body, (void *)parameter1);
    usleep(10000000);
        if(pthread_cancel(first_id)!=0)
    {
        printf("thread cancel failed\n");
    }
    else
        printf("thread cancel successful\n");
    if(pthread_join(first_id, NULL)!=0)
    {
        printf("join thread faild\n");
    }
    else
    {
        printf("join thread successfull\n");
    }
    printf("%d\n",val);
    pthread_attr_destroy(&myattr1);
    return 0;
}
