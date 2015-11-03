/*
 * ex_iolib.c
 *
 *  Created on: Oct 20, 2014
 *      Author: chenbfeng2
 */

#include <stdio.h>
#include <stdlib.h>
#include "../BBBIOlib/BBBio_lib/BBBiolib.h"
#include <pthread.h>

/* Servo values for TG9 servos: */
/* Servo 0 degree angle pulse high time in msec */
#define SRV_0    0.45
/* Servo 180 degree angle pulse high time in msec */
#define SRV_180  2.45

/* Pulse repetition frequency in Hz */
#define FRQ 50.0f
/* Pulse period in msec */
#define PER (1.0E3/FRQ)
static int valueget=0;
static char nameget='P';
//declare the data element type.
struct elem{
    char c;
    int integer;
};
//declare the data buffer struct
struct buffer{
    struct elem content[20];
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

char dequeue(void)
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
    //printf("	dequeue %c %d \n",nameget,valueget);
    //fflush(stdout);
    //broadcast the condition variable. and unlock the mutex
    pthread_cond_broadcast(&(b.bufferwait));
    pthread_mutex_unlock(&(b.bufferlocker));
    return nameget;
}


int producerth1(void){
	int count = 0;
	int flag=0;
	iolib_setdir(8,11, BBBIO_DIR_IN);   /* Set pin P8 - 11 as input */

while(count < 10)
{

	char l='L';
	iolib_delay_ms(100);
	if (is_high(8,11))   /* Check if in is high (i.e. button pressed) */
	{
		if (flag==1)
		{
			flag=0;
			enqueue(l,count);
			count++;
			continue;
		}
		else
			continue;
	}
	if (is_low(8,11))    /* Check if in is low (i.e button is released) */
	{
		if (flag==0)
		{
			flag=1;
		}
		else
			continue;
	}
}
	return 0;
}

int producerth2(void){
	int count = 0;
	int flag=0;
	iolib_setdir(8,12, BBBIO_DIR_IN);   /* Set pin P8 - 11 as input */

while(count < 10)
{

	char l='R';
	iolib_delay_ms(100);
	if (is_high(8,12))   /* Check if in is high (i.e. button pressed) */
	{
		if (flag==1)
		{
			flag=0;
			enqueue(l,count+10);
			count++;
			continue;
		}
		else
			continue;
	}
	if (is_low(8,12))    /* Check if in is low (i.e button is released) */
	{
		if (flag==0)
		{
			flag=1;
		}
		else
			continue;
	}
}
	return 0;
}

int comsumerth(void){
    int i=0;
    char result;
    int angle=10;
    int initialp=90;
    int m=90;
    float SM_1_duty ;	/* Servomotor 1 , connect to ePWM0A */
    //dequeue the data from buffer for 3000 times.
    SM_1_duty = 100.0 - ((SRV_0/PER) + (initialp/180.0) * ((SRV_180-SRV_0)/PER))*100.0;
    printf("Angle : %d , duty : %f\n" ,initialp ,SM_1_duty);
    BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , FRQ, SM_1_duty , SM_1_duty); /* Set up PWM */
    BBBIO_ehrPWM_Enable(BBBIO_PWMSS0); /* Enable PWM, generate waveform */
	sleep(2); /* Allow time for servo to settle and for humans to see something. */
	//BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);
    for (i=0;i<20;i++)
    {
        result=dequeue();
        printf("instruction dequeue: %c\n",result);
        if (result=='L')
        {
        	m=m-angle;
    	    /* Calculate duty cycle */
    		/* Note: the 100-X duty cyle is to account for the level shifter that inverts */
    		SM_1_duty = 100.0 - ((SRV_0/PER) + (m/180.0) * ((SRV_180-SRV_0)/PER))*100.0;
    		printf("Angle : %d , duty : %f\n" ,m ,SM_1_duty);
    	    BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , FRQ, SM_1_duty , SM_1_duty); /* Set up PWM */
    		BBBIO_ehrPWM_Enable(BBBIO_PWMSS0); /* Enable PWM, generate waveform */
    		sleep(2); /* Allow time for servo to settle and for humans to see something. */
    		//BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);
        }
        else if (result=='R')
        {
        	m=m+angle;
    	    /* Calculate duty cycle */
    		/* Note: the 100-X duty cyle is to account for the level shifter that inverts */
    		SM_1_duty = 100.0 - ((SRV_0/PER) + (m/180.0) * ((SRV_180-SRV_0)/PER))*100.0;
    		printf("Angle : %d , duty : %f\n" ,m ,SM_1_duty);
    	    BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , FRQ, SM_1_duty , SM_1_duty); /* Set up PWM */
    		BBBIO_ehrPWM_Enable(BBBIO_PWMSS0); /* Enable PWM, generate waveform */
    		sleep(2); /* Allow time for servo to settle and for humans to see something. */
    		//BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);
        }
    }
    return 0;
}

int main(void)
{
	iolib_init(); /* Initialize I/O library - required */

	    pthread_t thread1, thread2,thread3, main_id;
	    //initial the data buffer and create the threads
	    main_id=pthread_self();
	    buffer_init(b);
	    pthread_create( &thread1, NULL, &producerth1, NULL);
	    pthread_create( &thread2, NULL, &producerth2, NULL);
	    pthread_create( &thread3, NULL, &comsumerth, NULL);

	    //join the thread when finished
	    pthread_join( thread1, NULL);
	    pthread_join( thread2, NULL);
	    pthread_join( thread3, NULL);
	iolib_free();  /* Release I/O library prior to program exit - required */
	return(0);
}
