/*
 * question3.c
 *
 *  Created on: Nov 10, 2014
 *      Author: chenbfeng2
 */

#include <stdio.h>
#include <stdlib.h>
#include "../BBBIOlib/BBBio_lib/BBBiolib.h"
/* -------------------------------------------------------------- */
int main(void)
{	//initial bbbio
	iolib_init();

	//set up the PWM setting and the duty cycle.
	BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , 1000000.0f ,50.0 , 50.0);
	printf("PWM %d setting\n", BBBIO_PWMSS0);
	sleep(5);
	printf("PWM %d enable\n", BBBIO_PWMSS0);
	//enable the IO pin to output waveform
	BBBIO_ehrPWM_Enable(BBBIO_PWMSS0);
	//disable the output pin after 5s.
	sleep(5);
	printf("PWM %d disable\n", BBBIO_PWMSS0);
	BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);

	//free the BBBio
	iolib_free();
	return(0);
}
