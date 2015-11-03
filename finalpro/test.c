#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>  /* semaphore operations */
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <math.h>
#include <termios.h>
#include "../BBBIOlib/BBBio_lib/BBBiolib.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;
struct row {int* r[3000];
			int rdpt;
			int wrtpt;
			} ;
struct row dataout[16];
int tempBuffer[17][300];
int numberArray[148];
int stop=0,start=0;
int programend=0;
long int finalmark=0;
#define NSEC_PER_SEC 1000000000ULL

static struct timespec r;
static long long int period;

//add a specific time slot to the current time
static inline void timespec_add_us(uint64_t d)
{
    d *= 1000;
    d += r.tv_nsec;
    //check if the nanosecond is overflowed.
    while (d >= NSEC_PER_SEC) {
        d -= NSEC_PER_SEC;
	r.tv_sec = r.tv_sec+1;
    }
    r.tv_nsec = d;
    //printf("%d,%lld\n",r.tv_sec,r.tv_nsec);
}

//calculate the time difference of the timeout.
long long int diff(struct timespec start, struct timespec end)
{
	long long int temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp = (end.tv_sec-start.tv_sec-1)*1000000000;
		temp += 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp = (end.tv_sec-start.tv_sec)*1000000000;
		temp+=end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

//print the time out every 200ms
static long long int wait_next_activation(int i)
{	int s=0;
	struct timespec start;
	struct timespec end;
	long long int temp=0;
	//get the real time clock start time
	clock_gettime(CLOCK_REALTIME,&start);
	//time out for 200ms
    s=clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &r, NULL);
    //record the time out stop time
    clock_gettime(CLOCK_REALTIME,&end);
    //print the time out information
    /*if (s==0)
    	{
    	printf("timer called %d!\n",i);
    	}
	*/

    timespec_add_us(period);
    //clock_gettime(CLOCK_REALTIME,&end);
    //temp=diff(start,end);
    return temp;
}

//initial the timer
static int start_periodic_timer(uint64_t offs, int t)
{
    clock_gettime(CLOCK_REALTIME, &r);
    timespec_add_us(offs);
    period = t;
    return 0;
}

int clean_stdin(void)
{
    while (getchar()!='\n');
    return 1;
}
	int databuild(void)
{
	int length=0;
	length=sizeof(numberArray)/sizeof(numberArray[0]);
	int x=0,y=0,z=0;
	int tempTime,tempType,tempLine;
	int line1=0,line2=4,line3=8,line4=12;
	int previousTime1=8000;
	int previousTime2=8000;
	int previousTime3=8000;
	int previousTime4=8000;

	for (x=0;x<length;x++)
	{
		tempTime=((numberArray[x])/100)-10000;
		tempType=numberArray[x]%10;
		tempLine=(numberArray[x]%100)/10;
		//printf("fhefwjpf is %d, %d ,%d, %d\n",length ,tempTime,tempType,tempLine);
		switch(tempLine)
		{
		case 1:
			//printf("case 1\n");
			fflush(stdout);
			patternGeneration(line1,tempType,tempTime,previousTime1);
			//printf("%s\n",tempBuffer[2]);
			/*for (y=0;y<4;y++)
			{
			strcat(dataout[y],tempBuffer[y]);
		}*/
			previousTime1=tempTime;
			//memset(tempBuffer, 0, sizeof tempBuffer);
			break;
		case 2:
			patternGeneration(line2, tempType,tempTime,previousTime2);

			previousTime2=tempTime;
			//memset(tempBuffer, 0, sizeof tempBuffer);
			break;
		case 3:
			patternGeneration(line3,tempType,tempTime,previousTime3);

			previousTime3=tempTime;
			//memset(tempBuffer, 0, sizeof tempBuffer);
			break;
		case 4:
			patternGeneration(line4,tempType,tempTime,previousTime4);
			previousTime4=tempTime;
			//memset(tempBuffer, 0, sizeof tempBuffer);
			break;
		default:
			//printf("recognize fault\n");
			break;
		}
	}
return 0;
}
void datainitial(void)
{	int k=0;
	int i=0,j=0,a;
	int n;
	FILE *myFile;
	int num;
	    myFile = fopen("test.txt", "r");
	    while(fscanf(myFile, "%d", &num) > 0) {
	        numberArray[i] = num;
	        printf("%d\n",numberArray[i]);
	        i++;
	    }
	    printf("scan finished");
	    fclose(myFile);
	    n=sizeof(numberArray)/sizeof(int);
	for (i = 0; i < n; ++i)
	    {
	        for (j = i + 1; j < n; ++j)
	        {
	            if (numberArray[i] < numberArray[j])
	            {
	                a = numberArray[i];
	                numberArray[i] = numberArray[j];
	                numberArray[j] = a;
	            }
	        }
	    }
	for (k=0;k<16;k++)
	{
		dataout[k].wrtpt=0;
		dataout[k].rdpt=4000+65-60;
	}

}
void datafinalize(void)
{
	int i=0,j=0,k=0;
	for(i=0;i<16;i++)
	{
		for (j=0;j<65;j++)
		{
			dataout[i].r[dataout[i].wrtpt]=0;
			dataout[i].wrtpt++;
		}
	}
}

void patternGeneration(int lineNum, int tempType, int tempTime, int previousTime)
{
	//int filedata[]={1000011,1000021,1000031,1000041,1000111,1000521,1000731,1000741,1001041,1001111,1001521,1001831};
	//int length=sizeof(filedata);
	int star[4][3] = {{0,0,0},{0,2,0},{3,1,3},{0,2,0}};
	int end[4][3] = {{0,0,0},{0,0,0},{2,2,2},{0,0,0}};
	int shift1[4][3]={{0,3,0},{0,3,0},{0,1,2},{0,0,0}};
	int shift2[4][3]={{0,0,0},{0,3,0},{2,1,0},{0,3,0}};
	//char *channel[4][10000];
	int x=0,y=0,z=0;
	int cont=0;
	int timeInterval=previousTime-tempTime;
		switch (tempType) {
		case 1:
			for (y=0;y<4;y++)
					{
				for (z=0;z<timeInterval;z+=2)
				{
					dataout[lineNum+y].r[dataout[lineNum+y].wrtpt+1]=0;
					dataout[lineNum+y].wrtpt++;
				}
				for(x=0;x<3;x++) {
					dataout[lineNum+y].r[dataout[lineNum+y].wrtpt-1+x]=star[y][x];
				}
				dataout[lineNum+y].wrtpt++;
				}
			break;
		case 2:
			 cont=1;
			for (y=0;y<4;y++)
					{
				/*for (z=0;z<timeInterval;z+=2)
					{if (y==1)
										{
											dataout[lineNum+y].r[dataout[lineNum+y].wrtpt]=3;
											dataout[lineNum+y].wrtpt++;
										}
										else{
												dataout[lineNum+y].r[dataout[lineNum+y].wrtpt+1]=0;
														dataout[lineNum+y].wrtpt++;
										}
					}
				for(x=0;x<3;x++) {
					dataout[lineNum+y].r[dataout[lineNum+y].wrtpt-1+x]=star[y][x];

								}
				dataout[lineNum+y].wrtpt++;
								}*/
				for (z=0;z<timeInterval;z+=2)
				{
					dataout[lineNum+y].r[dataout[lineNum+y].wrtpt+1]=0;
					dataout[lineNum+y].wrtpt++;
				}
				for(x=0;x<3;x++) {
					dataout[lineNum+y].r[dataout[lineNum+y].wrtpt-1+x]=star[y][x];
				}
				dataout[lineNum+y].wrtpt++;
				}
						  break;

		case 3:
			cont=0;
			for (y=0;y<4;y++)
								{				for (z=0;z<timeInterval;z+=2)
								{
									dataout[lineNum+y].r[dataout[lineNum+y].wrtpt+1]=0;
									dataout[lineNum+y].wrtpt++;
								}
								for(x=0;x<3;x++) {
									dataout[lineNum+y].r[dataout[lineNum+y].wrtpt-1+x]=star[y][x];
								}
								dataout[lineNum+y].wrtpt++;
								}

			break;

		default:
		  // Code
		  break;
	}
		//return (char*)channel;
}

int* patternEnq(void)
{
char *buffer[17][400];
int i=0,j=0,k=0,q=0,p=0;
char space[2000]="";
char *empty=" ";
char finalout[3000]="";
char *enter="\n";
int speed=2;
char* blue= ANSI_COLOR_CYAN;
char* Green=ANSI_COLOR_GREEN;
char* yellow=ANSI_COLOR_YELLOW;
char* mag=ANSI_COLOR_RESET;
 int m=0;
    long long int s=40000;
    uint64_t ss=30000;
    //calculate the time difference for every interaction.
char *pattern="*";
char *linepot="|";
char *linept2="-";
printf("game start");
sleep(1);
pthread_mutex_lock( &condition_mutex );
start_periodic_timer(ss,s);
for (k=0;k<3000;k++)
{
	i=0;
	strcat(buffer,blue);
	for (q=0;q<4;q++)
	{

		for (i=0;i<60;i++)
					{
			if (q==0&&(i>54&&i<60))
			{
				strcat(buffer,linept2);
			}

			else if (i==54)
			{
				strcat(buffer,linepot);
			}
			else if (dataout[q].r[(dataout[q].rdpt)+i]==1)
					{
						strcat(buffer,pattern);
					}
					else if (dataout[q].r[(dataout[q].rdpt)+i]==2)
					{
						strcat(buffer,linepot);
					}
					else if (dataout[q].r[(dataout[q].rdpt)+i]==3)
										{
											strcat(buffer,linept2);
										}

					else
						strcat(buffer,empty);
					}
		dataout[q].rdpt-=speed;
		strcat(finalout,buffer);
		strcat(finalout,enter);
		memset(buffer, 0, sizeof buffer);
		i=0;
	}

	strcat(buffer,yellow);
	for (q=0;q<4;q++)
	{

		for (i=0;i<60;i++)
		{if (q==0&&(i>54&&i<60))
		{
			strcat(buffer,linept2);
		}

		else if (i==54)
			{
				strcat(buffer,linepot);
			}
			else if (dataout[q+4].r[(dataout[q+4].rdpt)+i]==1)
							{
								strcat(buffer,pattern);
							}
							else if (dataout[q+4].r[(dataout[q+4].rdpt)+i]==2)
							{
								strcat(buffer,linepot);
							}
							else if (dataout[q+4].r[(dataout[q+4].rdpt)+i]==3)
												{
													strcat(buffer,linept2);
												}

							else
								strcat(buffer,empty);
							}
				dataout[q+4].rdpt-=speed;
				//printf("%s",buffer);
				//strcat(buffer,temp);
				//strncpy(output[q+9],buffer,65);
				strcat(finalout,buffer);
				strcat(finalout,enter);
				memset(buffer, 0, sizeof buffer);
				i=0;
			}

	strcat(buffer,Green);
	for (q=0;q<4;q++)
			{

		for (i=0;i<60;i++)
		{if (q==0&&(i>54&&i<60))
		{
			strcat(buffer,linept2);
		}

		else if (i==54)
			{
				strcat(buffer,linepot);
			}
			else if (dataout[q+8].r[(dataout[q+8].rdpt)+i]==1)
							{
								strcat(buffer,pattern);
							}
							else if (dataout[q+8].r[(dataout[q+8].rdpt)+i]==2)
							{
								strcat(buffer,linepot);
							}
							else if (dataout[q+8].r[(dataout[q+8].rdpt)+i]==3)
												{
													strcat(buffer,linept2);
												}

							else
								strcat(buffer,empty);
							}
				dataout[q+8].rdpt-=speed;
				//printf("%s",buffer);
				//strcat(buffer,temp);
				//strncpy(output[q+9],buffer,65);
				strcat(finalout,buffer);
				strcat(finalout,enter);
				memset(buffer, 0, sizeof buffer);
				i=0;
			}

	strcat(buffer,mag);
	for (q=0;q<4;q++)
	{

		for (i=0;i<60;i++)
		{if (q==0&&(i>54&&i<60))
		{
			strcat(buffer,linept2);
		}

		else if (i==54)
			{
				strcat(buffer,linepot);
			}
			else if (dataout[q+12].r[(dataout[q+12].rdpt)+i]==1)
							{
								strcat(buffer,pattern);
							}
							else if (dataout[q+12].r[(dataout[q+12].rdpt)+i]==2)
							{
								strcat(buffer,linepot);
							}
							else if (dataout[q+12].r[(dataout[q+12].rdpt)+i]==3)
												{
													strcat(buffer,linept2);
												}

							else
								strcat(buffer,empty);
							}				dataout[q+12].rdpt-=speed;
				//printf("%s",buffer);
				//strcat(buffer,temp);
				//strncpy(output[q+9],buffer,65);
				strcat(finalout,buffer);
				strcat(finalout,enter);
				memset(buffer, 0, sizeof buffer);
				i=0;
			}


	//dataout;
	//memset(buffer, 0, sizeof buffer);

		  //wait for condition if the count number is between 3 and 6.
	      while( stop==1 )
	      {
	         pthread_cond_wait( &condition_cond, &condition_mutex );
	      }


wait_next_activation(m);
system("clear");
printf("%s",finalout);
fflush(stdout);
//printf("%lld\n",differ[m]);
m++;



memset(space, 0, sizeof space);
memset(finalout,0,sizeof finalout);
pthread_mutex_unlock( &condition_mutex );
//getinput();
}
start=0;
programend=1;
printf("music end!!");
return 0;
}
int* getmark(void)
{
	while(programend==0)
	{
		if (is_high(8,16))   /* Check if in is high (i.e. button pressed) */
				{
					finalmark+=10;
				}
	}
	printf("mark recorded!");
	return 0;
}
int* playmusic(void)

{

	int del=6000;
	//pin_low(8,16);
	iolib_setdir(8,16, BBBIO_DIR_OUT);
	//sleep(4);
	pin_high(8,16);
	printf("play music initial");
	iolib_delay_ms(del);  /* Delay for 'del' msec */
	pin_low(8,16);		  /* Set pin to low - LED off */

	return 0;

}

int *pausecheck(void)
{


	 /* Initialize I/O library - required */
	   /* Set pin P8 - 11 as input */
	//iolib_setdir(8,12, BBBIO_DIR_OUT);  /* Set pin P8 - 12 as output */
	printf("pause check initial");
	int count = 0;
	iolib_setdir(8,12, BBBIO_DIR_IN);
	while(programend==0)
	{
		//usleep(1000);
		if (stop==0)
		{
		if(is_high(8,12))  /* Check if in is high (i.e. button pressed) */
			{printf("the game is paused!!\n");
			stop=1;}
		}
		else if(stop==1)
			{
			       pthread_mutex_lock( &condition_mutex );
			       if(is_low(8,12))   /* Check if in is high (i.e. button pressed) */
			       {
			       			stop=0;
			       			pthread_cond_signal( &condition_cond );
			       }
			       pthread_mutex_unlock( &condition_mutex );
			}
		/*
		pin_high(8,12);
		iolib_delay_ms(del);
		pin_low(8,12);
		iolib_delay_ms(del);
		*/

	}
	printf("pausecheck finish");/* Release I/O library prior to program exit - required */
	return 0;
}

int main()
{
	char *temp;
	int songNum=0;
	int played=0;
	char decision;
	int ch[4];
	int yorn[4];
	int i=0;
    pthread_t thread1, thread2,thread3,thread4, main_id;
    //initial the data buffer and create the threads
    main_id=pthread_self();
    start=1;
    iolib_init();

    //iolib_setdir(8,12, BBBIO_DIR_IN);
    iolib_setdir(8,16, BBBIO_DIR_OUT);
    pin_low(8,16);    //is_low(8,12);
    printf("welcome to the music game! Please choose a song to play:\n");
    while(start==1)
    {
    printf("1.sample 1\n2.sample 2\n3. sample 3\n");
    i=0;
    while ( ((ch[i]=getchar()) != EOF)&&(i<3 ))
    		{
    	printf("%d",ch[i]);
    		i++;
    		}
    switch(ch[2])
    {
    	case 65:
    			datainitial();
    		    databuild();
    		    datafinalize();
    		    //playmusic();
    		    pthread_create( &thread1, NULL, &playmusic, NULL);
    		    pthread_create( &thread2, NULL, &patternEnq, NULL);
    		    pthread_create( &thread3, NULL, &pausecheck, NULL);
    		    pthread_create( &thread4, NULL, &getmark, NULL);
    		    pthread_join( thread1, NULL);
    		    pthread_join( thread2, NULL);
    		    pthread_join( thread3, NULL);
    		    pthread_join( thread4, NULL);
    		    played=1;
    		    i=0;
    		    break;
    	default:
    		printf("cannot find this song, please choose another one!\n");
    		break;
    }

   // pthread_create( &thread4, NULL, &patternPrint, NULL);
    if(played==1)
    {
    	printf("Good finish! your final score is 89\n");
    	printf("Do you wanna play again? Y/N\n");
    }


    	 while(played==1)
    	    {
    		 //clean_stdin();
    		 i=0;
    		    while ( ((yorn[i]=getchar()) != EOF)&&(i<3 ))
    		    		{
    		    			printf("%d",yorn[i]);
    		    			i++;
    		    		}
    		    switch(yorn[2])
    		        {
    		        	case 66:
    		        		 start=1;
    		        	    played=0;
    		        	    //clean_stdin();
    		        	    break;
    		        	case 67:
    		        		iolib_free();
    		        		exit(0);
    		        	default:
    		        		printf("Please choose again\n");
    		        		break;
    		        }

    //join the thread when finished
       }

    }

}
