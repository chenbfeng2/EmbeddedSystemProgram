/*
 * question4.c
 *
 *  Created on: Nov 10, 2014
 *      Author: chenbfeng2
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//initial the mutex and condition variables.
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

//declare the thread functions
void *functionCount2();

int  count = 0;
int condition_flag=0;

int main()
{
   pthread_t thread1, thread2;
   //create the thread and join them when finish
   pthread_create( &thread2, NULL, &functionCount2, NULL);
   //lock the mutex
	  pthread_mutex_lock( &condition_mutex );

	  //wait for condition if the child thread is running.
      while( condition_flag!=1 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      printf("child thread finished\n",count);
   pthread_join( thread2, NULL);

   exit(0);
}


void *functionCount2()
{
    for(;;)
    {

       //increase the count and print out the result.

       count++;
       printf("Counter value functionCount2: %d\n",count);

       //set up the conditional variable when finish
       if (count>=10)
       {
    	   condition_flag=1;
       }

       //inform the main thread when finish
       pthread_mutex_lock( &condition_mutex );
       if( condition_flag==1 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );
       //end the thread
       if (count>=10)
       {
    	  break;
       }
    }
    return NULL;
}
