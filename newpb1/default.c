/*
 * default.c
 *
 *  Created on: Sep 2, 2014
 *      Author: chenbfeng2
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
	//initial the array to store the inout text.
	char ipt[100]="";
	//initial the quit instruction array
	char qt[]="quit";
	int i=0;
	//the loop continue reading and printing strings
	for(;;){
		//get the input from user
		gets(ipt);
		//compare the string if it is same as ""quit.
		if (strcmp(ipt,qt)!=0)
		{
			//print the string in reverse order.
			for (i=(int)strlen(ipt)-1;i>=0;i--)
			{
				printf("%c",ipt[i]);
			}
			printf("\n");
		}
		else{
			//end the loop if user input quit.
			break;
		}
	}
    return 0;
}

