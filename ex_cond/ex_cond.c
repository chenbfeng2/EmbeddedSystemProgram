/*
 * ex_cond.c
 *
 *  Created on: Sep 1, 2014
 *      Author: gabor
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//#define SYNC
//initial the mutex and condition variables.
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;
//declare the thread functions
void *functionCount1();
void *functionCount2();

int  count = 0;
//define the constant used in thread.
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6


int main()
{
   pthread_t thread1, thread2;
   //create the thread and join them when finish
   pthread_create( &thread1, NULL, &functionCount1, NULL);
   pthread_create( &thread2, NULL, &functionCount2, NULL);
   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL);

   exit(0);
}

void *functionCount1()
{
   for(;;)
   {
	   //enclose the mutex operation in the if statement
#ifdef SYNC
	  pthread_mutex_lock( &condition_mutex );
	  //wait for condition if the count number is between 3 and 6.
      while( count >= COUNT_HALT1 && count <= COUNT_HALT2 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
#endif
      //increase the count and print out the result.
      count++;
      printf("Counter value functionCount1: %d\n",count);
      //unlock the mutex when finished
#ifdef SYNC
      pthread_mutex_unlock( &count_mutex );
#endif
      if(count >= COUNT_DONE) return(NULL);
    }
   return NULL;
}

void *functionCount2()
{
    for(;;)
    {
    	 //enclose the mutex operation in the if statement
#ifdef SYNC
       pthread_mutex_lock( &condition_mutex );
       if( count < COUNT_HALT1 || count > COUNT_HALT2 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );

       pthread_mutex_lock( &count_mutex );
#endif
       //increase the count and print out the result.
       count++;
       printf("Counter value functionCount2: %d\n",count);
       //unlock the mutex when finished
#ifdef SYNC
       pthread_mutex_unlock( &count_mutex );
#endif
       if(count >= COUNT_DONE) return(NULL);
    }
    return NULL;
}
