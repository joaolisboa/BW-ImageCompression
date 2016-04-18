/**
 * @file time_handler.c
 * @brief Include global start time variable and function to get execution time
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#include <stdlib.h>
#include <sys/time.h> 

#include "time_handler.h"

struct timeval start_time;

double get_execution_time();

/**
 * @fn double get_execution_time()
 * @brief returns the time it took to execute a function
 * @return double - time
 */
double get_execution_time(){
	struct timeval end_time;
	gettimeofday(&end_time, NULL);
	double time = (end_time.tv_sec - start_time.tv_sec) + ((end_time.tv_usec - start_time.tv_usec)/1000000.0);
	return time;
}