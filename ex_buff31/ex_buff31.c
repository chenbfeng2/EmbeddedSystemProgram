/*
 * ex_buff31.c
 *
 *  Created on: Oct 2, 2014
 *      Author: chenbfeng2
 */
/*
 * ex_cond.c
 *
 *  Created on: Sep 1, 2014
 *      Author: gabor
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int valueget=0;
static char nameget='P';
//declare the data element type.
struct elem{
    char c;
    int integer;
};
//declare the data buffer struct
struct buffer{
    struct elem content[16];
    int readp;
    int writep;
    int counter;
    pthread_mutex_t bufferlocker;
    pthread_cond_t bufferwait;
};
static struct buffer b;
void buffer_init(struct buffer b)
{
	//initiate the data buffer
    b.readp=0;
    b.writep=0;
    b.counter=0;
    pthread_mutexattr_t mymutexattr1;
    //initiate the mutex and condition variable in the data buffer
    pthread_mutexattr_init(&mymutexattr1);
    pthread_mutex_init(&(b.bufferlocker), &mymutexattr1);
    pthread_mutexattr_destroy(&mymutexattr1);
    pthread_condattr_t mycondattr2;
    pthread_condattr_init(&mycondattr2);
    pthread_cond_init(&(b.bufferwait), &mycondattr2);
    pthread_condattr_destroy(&mycondattr2);
}

int enqueue(char name, int value)
{
	//lock the mutex
    pthread_mutex_lock(&(b.bufferlocker));
    b.writep=(b.writep)%16;
    int fwrp=b.writep;
    //check the condition if buffer is full or not.
    while (b.counter>=16) {
        pthread_cond_wait(&(b.bufferwait), &(b.bufferlocker));
    }
    //write the data into the buffer
    b.content[fwrp].c=name;
    b.content[fwrp].integer=value;
    //update the write pointer and counter number
    b.writep++;
    b.counter++;
    //print the information write into the buffer
    printf("enqueue %c %d\n",b.content[fwrp].c,b.content[fwrp].integer);
    fflush(stdout);
    //wake up the thread  and unlock the mutex
    pthread_cond_broadcast(&(b.bufferwait));
    pthread_mutex_unlock(&(b.bufferlocker));
    return 0;
}

void dequeue(void)
{
	//lock the mutex for critical part
    pthread_mutex_lock(&(b.bufferlocker));
    b.readp=(b.readp)%16;
    int frdp=b.readp;
    //check the condition if buffer is empty or not.
    while (b.counter<=0) {
        pthread_cond_wait(&(b.bufferwait), &(b.bufferlocker));
    }
    //get the data from the buffer
    nameget=b.content[frdp].c;
    valueget=b.content[frdp].integer;
    //update the data read pointer and the counter number
    b.readp++;
    b.counter--;
    //print out the data read from buffer
    printf("	dequeue %c %d \n",nameget,valueget);
    fflush(stdout);
    //broadcast the condition variable. and unlock the mutex
    pthread_cond_broadcast(&(b.bufferwait));
    pthread_mutex_unlock(&(b.bufferlocker));
}

int comsumerth(void){
    int i=0;
    //dequeue the data from buffer for 3000 times.
    for (i=0;i<3000;i++)
    {
        dequeue();
    }


    return 0;
}


int producerth1(void){

    int i=0;
    char p='p';
    //enqueue the data for 1000 times
    for (i=0; i<1000; i++)
    {
        enqueue(p, i+1000);

    }
    return 0;
}
int producerth2(void){

    int i=0;
    char p='p';
    //enqueue the data for 1000 times
    for (i=0; i<1000; i++)
    {
        enqueue(p,2000+i);
    }
    return 0;
}
int producerth3(void){

    int i=0;
    char p='p';
    //enqueue the data for 1000 times
    for (i=0; i<1000; i++)
    {
        enqueue(p, i+3000);

        //fflush(stdout);
    }
    return 0;
}

int main()
{
    pthread_t thread1, thread2,thread3,thread4, main_id;
    //initial the data buffer and create the threads
    main_id=pthread_self();
    buffer_init(b);
    pthread_create( &thread1, NULL, &producerth1, NULL);
    pthread_create( &thread2, NULL, &producerth2, NULL);
    pthread_create( &thread3, NULL, &producerth3, NULL);
    pthread_create( &thread4, NULL, &comsumerth, NULL);

    //join the thread when finished
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);
    pthread_join( thread4, NULL);

    exit(0);
}



