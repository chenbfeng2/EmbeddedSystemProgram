/*
 * alarm.c
 *
 *  Created on: Oct 28, 2014
 *      Author: chenbfeng2
 */


/*
 * ex_iolib.c
 *
 *  Created on: Oct 20, 2014
 *      Author: chenbfeng2
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

// the structure to store the time information
struct time{
	int h;
	int min;
	int sec;
};

//the structure used for store current time information
struct timeinfo{
    struct time current;
    struct time alarm;
    pthread_mutex_t bufferlocker;
    pthread_cond_t bufferwait;
};
static struct timeinfo currenttime;

static int timerstop=0;
static int alarmflag=0;

//initial the alarm and clock structure.
void buffer_init(struct timeinfo timestart)
{
    timestart.current.h=0;
   timestart.current.min=0;
   timestart.current.sec=0;
   timestart.alarm.h=0;
   timestart.alarm.min=0;
   timestart.alarm.sec=0;
    pthread_mutexattr_t mymutexattr1;
    //initiate the mutex and condition variable in the clock
    pthread_mutexattr_init(&mymutexattr1);
    pthread_mutex_init(&(timestart.bufferlocker), &mymutexattr1);
    pthread_mutexattr_destroy(&mymutexattr1);
    pthread_condattr_t mycondattr2;
    pthread_condattr_init(&mycondattr2);
    pthread_cond_init(&(timestart.bufferwait), &mycondattr2);
    pthread_condattr_destroy(&mycondattr2);
}

//set the alarm time
int setalarm(int i, int j, int k)
{
	 pthread_mutex_lock(&(currenttime.bufferlocker));
	 	 	 	 	 	 alarmflag=1;
					   currenttime.alarm.sec=k;
					   currenttime.alarm.min=j;
					   currenttime.alarm.h=i;
					    pthread_mutex_unlock(&(currenttime.bufferlocker));
    return 0;
}


//set the current time value
char settimer(int i, int j, int k)
{
	 pthread_mutex_lock(&(currenttime.bufferlocker));
					   currenttime.current.sec=k;
					   currenttime.current.min=j;
					   currenttime.current.h=i;
					    pthread_mutex_unlock(&(currenttime.bufferlocker));
					    return 0;
}

//a realtime thread that keep updating the time and check for alarm
int runtime(void){
	int i,j,k;
	int alarmtime=0;
	struct time temp;
	while(1){
		//break out for 1s approximately.
			sleep(1);
				while(1){
				 pthread_mutex_lock(&(currenttime.bufferlocker));
				 //update the current time value every 1 second.
				   currenttime.current.sec+=1;
				   if(currenttime.current.sec>59)
					   currenttime.current.min+=1;
				   currenttime.current.h+=currenttime.current.min/60;
				   currenttime.current.sec=currenttime.current.sec%60;
				   currenttime.current.min=currenttime.current.min%60;
				   currenttime.current.h=currenttime.current.h%24;
				   //check the alarm setup condition
				   if (alarmflag==1)
				   {
					   //call function to check if the alarm is reached.
					   alarmtime=checkalarm();
				   }
				   else if (alarmflag==-1)
				   {
					   alarmtime=0;
					   alarmflag++;
				   }
				   //print out alarm information when alarm time is reached.
					if (alarmtime==1)
					{
						alarmflag=0;
						temp=currenttime.alarm;
						printf("***ALARM %d:%d:%d***\n",temp.h,temp.min,temp.sec);
						fflush(stdout);
					}
				    pthread_mutex_unlock(&(currenttime.bufferlocker));
				    break;
	}
				//stop the clock when timerstop is true
				if (timerstop==1)
				{
					break;
				}
}
	return 0;
}

//thread that keep scanning input from the command line window.
int clockset(void){
	int count = 0;
	int flag=0;
	char ipt[100]="";
	struct time temp;

	    int i,j,k;
	    char x[10];
	    char alarm[10]="$";

		for(;;){
			gets(ipt);
			sscanf(ipt,"%s %d %d %d",x,&i,&j,&k);
			//set up current time if input is s.
			if  (strcmp(x, "s") == 0)
			{
				settimer(i,j,k);
				printf("timer start point set as: %d:%d:%d\n",i,j,k);
				i=0;
				j=0;
				k=0;
				//?
				//printf("timer start point set as: %d:%d:%d\n",i,j,k);
			}
			//set up alarm if input is a.
			else if (strcmp(x, "a") == 0)
			{
				setalarm(i,j,k);
				printf("alarm set as: %d:%d:%d\n",i,j,k);
				i=0;
				j=0;
				k=0;
			}
			//print out current time if input is ?.
			else if (strcmp(x, "?") == 0)
			{
				temp=currenttime.current;
			printf("the current time is: %d:%d:%d\n",temp.h,temp.min,temp.sec);
			}
			//print out alarm time if input is $.
			else if (strcmp(x, alarm) == 0)
			{
				printf("get alarm time\n");
				temp=currenttime.alarm;
				printf("the Alarm time is set at: %d:%d:%d \n",temp.h,temp.min,temp.sec);
			}
			//stop the timer if input is q.
			else if (strcmp(x, "q") == 0)
			{
				timerstop=1;
				break;
			}
			//disable the alarm if the input is !.
			else if (strcmp(x, "!")==0)
			{
				alarmflag=-1;
			}
			//alert the user if get invalid input.
			else
				{
					printf("wrong input!\n");
				}
			//clean the input buffer
			memset(ipt,0,strlen(ipt));
		}

	    return 0;
}

int checkalarm(void){
		struct time temp1=currenttime.alarm;
		struct time temp2=currenttime.current;

		//check the current time if it is equal to alarm time.
		if (temp1.h==temp2.h)
		{
			if(temp1.min==temp2.min)
		{
			if(temp1.sec==temp2.sec)
			return 1;
			else return 0;
		}
			else return 0;
		}
		else return 0;
}

int main(void)
{
	    pthread_t thread1, thread2,thread3, main_id;
	    //initial the data buffer and create the threads
	    main_id=pthread_self();
	    setbuf(stdout,NULL);
	    buffer_init(currenttime);
	    pthread_create( &thread1, NULL, &runtime, NULL);
	    pthread_create( &thread2, NULL, &clockset, NULL);
	    //join the thread when finished
	    pthread_join( thread1, NULL);
	    pthread_join( thread2, NULL);
	return(0);
}
