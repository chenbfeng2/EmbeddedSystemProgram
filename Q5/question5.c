/*
 * question5.c
 *
 *  Created on: Nov 10, 2014
 *      Author: chenbfeng2
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


static int resettimer=0;
static int programstop=0;
static pthread_t thread1;
static pthread_t thread2;
static pthread_t main_id;

//a realtime thread that keep updating the time and check for input
int runtime(void){
	int i,j,k;
	int count=10;
	while(1){
		//initial the counting variables.
		count=10;
		resettimer=0;
				while(1){
				 printf("the count number is %d\n", count);
				 sleep(1);
				 count--;
				 //restart the count when input comes.
				if (resettimer==1)
				{
					break;
				}

				//end the thread when no input in 10s
				else if (count==0)
					break;
				}

				//cancel the inout thread when end
				if (resettimer==0)
				{
					programstop=1;
				    if(pthread_cancel(thread2)!=0)
				    {
				        printf("thread cancel failed\n");
				    }
					break;
				}
				else
					continue;
	}
	return 0;
}

//thread that keep scanning input from the command line window.
int clockset(void){
	char ipt[100]="";

		for(;;){
			gets(ipt);
			if (ipt!="")
			{
				//inform the other thread to reset counting.
				printf("input receive and timer reset\n");
				resettimer=1;
			}

		}

	    return 0;
}



int main(void)
{
	    //initial the data buffer and create the threads
	    main_id=pthread_self();
	    setbuf(stdout,NULL);
	    pthread_create( &thread1, NULL, &runtime, NULL);
	    pthread_create( &thread2, NULL, &clockset, NULL);
	    //join the thread when finished
	    pthread_join( thread1, NULL);
	    pthread_join( thread2, NULL);
	return(0);
}

