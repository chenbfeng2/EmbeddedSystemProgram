/*
 * ex_cond.c
 *
 *  Created on: Sep 1, 2014
 *      Author: gabor
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int valueget;
static char nameget;
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
    //initiate the mutex and condition variable in the data buffer
    pthread_mutexattr_t mymutexattr1;
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
    int fwrp=b.writep;
    //lock the mutex
    pthread_mutex_lock(&(b.bufferlocker));
    //check the condition if buffer is full or not.
    while (b.counter>=16) {
        pthread_cond_wait(&(b.bufferwait), &(b.bufferlocker));
    }
    //write the data into the buffer
    b.content[fwrp].c=name;
    b.content[fwrp].integer=value;
    //update the write pointer and counter number
    b.writep++;
    //reset point if it reach the end of buffer
    if (b.writep>15)
    {
    	b.writep=0;
    }
    b.counter++;
    //print the information write into the buffer
    printf("enqueue %c %d\n",name,value);
    fflush(stdout);
    //wake up the thread
    pthread_cond_broadcast(&(b.bufferwait));
    //unlock the mutex
    pthread_mutex_unlock(&(b.bufferlocker));
    return 0;
}

void dequeue(void)
{
    int frdp=b.readp;
    //lock the mutex for critical part
    pthread_mutex_lock(&(b.bufferlocker));
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
    //reset the data read pointer if it is at the end of buffer.
    if (b.readp>15)
    {
    	b.readp=0;
    }
    //print out the data read from buffer
    printf("dequeue %c %d\n",nameget,valueget);
    fflush(stdout);
    //broadcast the condition variable.
    pthread_cond_broadcast(&(b.bufferwait));
    // unlock the mutex
    pthread_mutex_unlock(&(b.bufferlocker));
}

int comsumerth(void){
    int i=0;
    char p='p';
    //dequeue the data from buffer for 1000 times.
    for (i=0;i<1000;i++)
    {
        dequeue();
    }


    return 0;
}

int producerth(void){

    int i=0;
    char p='p';
    //enqueue the data for 1000 times
    for (i=0; i<1000; i++)
    {
        enqueue(p, i);

        fflush(stdout);
    }
    return 0;
}

int main()
{
    pthread_t thread1, thread2, main_id;

    main_id=pthread_self();


    //initial the data buffer and create the threads
    buffer_init(b);
    pthread_create( &thread1, NULL, &producerth, NULL);
    pthread_create( &thread2, NULL, &comsumerth, NULL);

    //join the thread when finished
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);

    exit(0);
}

