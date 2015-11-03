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
static pthread_t second_id;
static pthread_t third_id;

static void *body(void *p)
{
    char *st= p;
    long long sum=0;
    int i=0, j=0;
    for (i=0; i<10; i++)
    {
        printf("%s,%d\n",st,i+1);
        for (j=0;j<1000000;j++)
        {
            sum+=j;
        }
       // printf("%lld\n",sum);

    }
    return 0;
}

int main(int argc, char *argv[])
{
    pthread_attr_t myattr1;
    pthread_attr_t myattr2;
    pthread_attr_t myattr3;
    int err;
    char *parameter1="A";
    char *parameter2="B";
    char *parameter3="C";
    void *returnvalue;
    struct sched_param param;
    param.sched_priority=99;
    main_id = pthread_self();
    pthread_setschedparam(main_id,SCHED_FIFO,&param);
    puts("main: before pthread_create\n");

    struct sched_param param1;
    param1.sched_priority = 1;

    struct sched_param param2;
    param2.sched_priority = 10;
    struct sched_param param3;
    param3.sched_priority = 10;

    pthread_attr_init(&myattr3);
    pthread_attr_init(&myattr2);
    pthread_attr_init(&myattr1);
   pthread_attr_setinheritsched(&myattr1, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setinheritsched(&myattr2, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setinheritsched(&myattr3, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy(&myattr1,SCHED_FIFO);
  pthread_attr_setschedpolicy(&myattr2, SCHED_FIFO);
    pthread_attr_setschedpolicy(&myattr3, SCHED_FIFO);

    pthread_attr_setschedparam(&myattr1, &param1);
    pthread_attr_setschedparam(&myattr2, &param2);
    pthread_attr_setschedparam(&myattr3, &param3);

    err = pthread_create(&first_id, &myattr1, body, (void *)parameter1);

    //pthread_attr_destroy(&myattr1);

    //pthread_join(first_id, &returnvalue);


    //int pthread_attr_setinheritsched(pthread_attr_t *myattr2, int inheritsched);
    //pthread_attr_setschedpolicy(&myattr2, SCHED_FIFO);
    //pthread_attr_setschedparam(&myattr2, &param2);
    err = pthread_create(&second_id, &myattr2, body, (void *)parameter2);
    //pthread_attr_destroy(&myattr2);


    //pthread_attr_init(&myattr3);
    //int pthread_attr_setinheritsched(pthread_attr_t *myattr3, int inheritsched);
    //pthread_attr_setschedpolicy(&myattr3, SCHED_FIFO);
        err = pthread_create(&third_id, &myattr3, body, (void *)parameter3);
    /* the thread attribute is no more needed */
    pthread_attr_destroy(&myattr1);

    pthread_attr_destroy(&myattr2);
    pthread_attr_destroy(&myattr3);

    pthread_join(first_id, &returnvalue);
    pthread_join(second_id, &returnvalue);
    pthread_join(second_id, &returnvalue);
    printf("main: after pthread_create\n");

    printf("main: returnvalue is %d\n", (int)returnvalue);

    return 0;
}
