/**
 * @file signal_handler.c
 * @brief Includes function to install signal and to handle a signal
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "signal_handler.h"
#include "debug.h"
#include "structs.h"

void signal_handler(int signal);
void install_signal_handler();

/**
 * @fn void signal_handler(int signal)
 * @brief defines application behaviour when receiving signals
 * @param signal - code of the signal
 */
void signal_handler(int signal){
	int aux = errno; 
	(void)signal;

	if(signal == SIGINT){
		running = 0;
		time_t time_T;
	  	struct tm *date;
		time(&time_T);
	  	date = localtime(&time_T);
		fprintf(stderr, "[SIGINT] - Operation interrupted by user @%d-%02d-%02d %02dh%02d\n", 
																date->tm_year + 1900, 
																date->tm_mon + 1, 
																date->tm_mday, 
																date->tm_hour, 
																date->tm_min);
	}
	errno = aux;
}

/**
 * @fn void signal_handler(int signal)
 * @brief defines application behaviour when receiving signals
 * @param signal - code of the signal
 */
void install_signal_handler(void){
	struct sigaction act;
	act.sa_handler = &signal_handler;

	sigemptyset(&act.sa_mask); 
	act.sa_flags = 0;
	act.sa_flags = SA_RESTART;
	//act.sa_flags = SA_SIGINFO;
	//sigfillset(&act.sa_mask);

	if(sigaction(SIGINT, &act, NULL) != 0){
		ERROR(ERROR_UNKNOWN, "Failed to install signal handler");
	}

}