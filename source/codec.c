/**
 * @file codec.c
 * @author João Emanuel de Oliveira Lisboa - 2130158
 * @author Luís Eduardo Moutinho Guerra - 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "structs.h"
#include "prog_opt.h"
#include "debug.h"
#include "memory.h"
#include "signal_handler.h"
#include "file_handler.h"
#include "memory_handler.h"
#include "psnr.h"
#include "validations.h"
#include "time_handler.h"

void decode(const char *cod_filename, char *dic_filename);
void folderDecode(const char* folder, char *dic_filename);

void decode(const char *cod_filename, char *dic_filename){
	cod_t image;
	dict_t dictionary;
	if(read_file_COD(cod_filename, &image) && read_dict(dic_filename, &dictionary)){
		int success = validate_dict_dimensions(image, dictionary);
		if(success){
			if(write_pgm_file(image, dictionary)){
				fprintf(stderr, "operation: %s DECODE %s\n", cod_filename, dic_filename);
			}
		}else if(success == 0){
			PRINT(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'\n", cod_filename, dic_filename);
		}else if(success == -1){
			PRINT(stderr, "FAILURE:index out of range of file '%s' with dict '%s'\n", cod_filename, dic_filename);
		}
		free_image(&image);
		free_dictionary(&dictionary);
	}
}

void folderDecode(const char* folder, char* dic_filename){
	struct dirent entry, *result = NULL;
	struct stat stats;
	char *fullname;

	DIR *dptr= opendir(folder);
	if(dptr==NULL){
		printf("ERROR: Could not open directory %s\n", folder);
	}

	while(readdir_r(dptr, &entry, &result) == 0 && result != NULL){
		if(result->d_name[0] == '.'){
			continue;
		}
		path_combine(folder, result->d_name, &fullname);

		if(lstat(fullname, &stats) != 0){
			ERROR(IO_ERROR, "ERROR: Could not read stats about file %s\n", fullname);
		}
		if(S_ISDIR(stats.st_mode)){
			folderDecode(fullname, dic_filename);
		}else if(S_ISREG(stats.st_mode)){
			decode(fullname, dic_filename);
		}
	}
	
	closedir(dptr);
	flag = 1;
}
