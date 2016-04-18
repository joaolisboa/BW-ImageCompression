/**
* @file main.char
* @brief Main of PACODEC
* @date 2015
* @author João Lisboa 2130158, Luís Guerra 2130159
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>

#include "structs.h"
#include "prog_opt.h"
#include "debug.h"
#include "memory.h"
#include "signal_handler.h"
#include "pgm_handler.h"
#include "cod_handler.h"
#include "dict_handler.h"
#include "encode.h"
#include "decode.h"
#include "psnr.h"
#include "time_handler.h"

/**
* main
* @brief Main Function
* @param argc
* @param argv
* @return int
*/
int main(int argc, char *argv[]){

	running = 1;
	flag = 1;
	/* Used to determine if the execution was successful */
	int success = 0;
	install_signal_handler();

	struct gengetopt_args_info ArgsInfo;
	if (cmdline_parser(argc, argv, &ArgsInfo)) {
		ERROR(IO_ERROR, "ERROR: An error occured while parsing the command line arguments.\n");
		exit(EXIT_FAILURE);
	}
	/* Get time of day for first */
	gettimeofday(&start_time, NULL);
	
	// --decode option
	if(ArgsInfo.decode_given) {	
		success = decode(ArgsInfo.decode_arg, ArgsInfo.dict_arg);
	}
	// --decode-dir option
	else if(ArgsInfo.decode_dir_given) {	
		flag = 0;
		success = folderDecode(ArgsInfo.decode_dir_arg, ArgsInfo.dict_arg);
	}
	// --encode option
	else if(ArgsInfo.encode_given) {	
		success = encode(ArgsInfo.encode_arg, ArgsInfo.dict_arg);
	}
	// --parallel-encode option
	else if(ArgsInfo.parallel_encode_given) {
		int num_threads = 0;
		if(ArgsInfo.threads_given){
			num_threads = ArgsInfo.threads_arg;
		}
		success = parallel_encode_image(ArgsInfo.parallel_encode_arg, ArgsInfo.dict_arg, num_threads);
	}
	// --dict option
	else if(ArgsInfo.dict_given){
		dict_t dictionary;
		if((success = read_dict(ArgsInfo.dict_arg, &dictionary))){
			fprintf(stderr, "operation:OK:%s\n", ArgsInfo.dict_arg);
		}
	}
	// --PSNR option
	else if(ArgsInfo.PSNR_given){
		double psnr = process_PSNR(ArgsInfo.PSNR_arg[0], ArgsInfo.PSNR_arg[1]);
		if(psnr != 0){
			success = 1;
			PRINT(stderr, "PSNR:OK:%s:%s:%.5f\n", ArgsInfo.PSNR_arg[0], ArgsInfo.PSNR_arg[1], psnr);	
		}
	}
	// -a/--about option
	else if(ArgsInfo.about_given){
		printf("About:\nJoao Lisboa - 2130158\nLuis Guerra - 2130159");
	}else{
		fprintf(stderr, "No arguments received\n");
	}


	if(running && success){
		PRINT(stderr, "Execution time: %.3fs\n", get_execution_time());
	}

	cmdline_parser_free(&ArgsInfo);

	return 0;
}


