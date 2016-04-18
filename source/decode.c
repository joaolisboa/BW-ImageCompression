/**
 * @file decode.c
 * @brief Decode related functions
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#include "structs.h"
#include "debug.h"
#include "memory.h"
#include "pgm_handler.h"
#include "cod_handler.h"
#include "dict_handler.h"
#include "file_handler.h"
#include "memory_handler.h"
#include "validations.h"

int decode(const char *cod_filename, char *dic_filename);
int folderDecode(const char* folder, char *dic_filename);
pgm_t* create_pgm(const cod_t *image, const dict_t *dictionary);

/**
 * @fn int decode(const char *cod_filename, char *dic_filename)
 * @brief Decode a .cod file
 * @param *cod_filename - .cod filename
 * @param *dic_filename - .dic filename
 * @return int - 1 in case of success
 */
int decode(const char *cod_filename, char *dic_filename){
	//create cod_t image to be decoded and dictionary
	cod_t image;
	dict_t dictionary;
	int success = 0;

	//read .cod file
	if(read_file_COD(cod_filename, &image)){
		//read .dic file
		if(read_dict(dic_filename, &dictionary)){
			//determine if .cod is compatible with dictionary
			if((success = validate_dict_dimensions(image, dictionary)) && running){
				pgm_t *decoded_pgm;
				//create pgm image - decode operation
				decoded_pgm = create_pgm(&image, &dictionary);
				//successful if pgm image was created the file was written
				if(decoded_pgm != NULL && write_pgm_file(decoded_pgm) && running){
					fprintf(stderr, "operation: %s DECODE %s\n", cod_filename, dic_filename);
					free_pgm(decoded_pgm);
					success = 1;	
				}
				free(decoded_pgm);
			//cod and dictionary block dimensions don't match
			}else if(success == 0){
				PRINT(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'\n", cod_filename, dic_filename);
			//dictionary doesn't have enough indexes for the operation
			}else if(success == -1){
				PRINT(stderr, "FAILURE:index out of range of file '%s' with dict '%s'\n", cod_filename, dic_filename);
			}
			free_dictionary(&dictionary);
		}
		free_image(&image);
	}
	return success;
}

/**
 * @fn int folderDecode(const char* folder, char* dic_filename)
 * @brief Decode a folder
 * @param *folder - folder name
 * @param *dic_filename - .dic filename
 * @return int - 1 in case of success
 */
int folderDecode(const char* folder, char* dic_filename){
	struct dirent entry, *result = NULL;
	struct stat stats;
	char *fullname;
	int success = 0;

	DIR *dptr= opendir(folder);
	if(dptr==NULL){
		printf("ERROR: Could not open directory %s\n", folder);
	}

	while(readdir_r(dptr, &entry, &result) == 0 && result != NULL && running){
		if(result->d_name[0] == '.'){
			continue;
		}
		path_combine(folder, result->d_name, &fullname);

		if(lstat(fullname, &stats) != 0){
			ERROR(IO_ERROR, "ERROR: Could not read stats about file %s\n", fullname);
		}
		if(S_ISDIR(stats.st_mode)){
			success = folderDecode(fullname, dic_filename);
		}else if(S_ISREG(stats.st_mode)){
			success = decode(fullname, dic_filename);
		}
	}
	
	closedir(dptr);
	flag = 1;
	return success;
}

/**
 * @fn pgm_t* create_pgm(const cod_t *image, const dict_t *dictionary)
 * @brief Decode operation which returns a pgm_t struct
 * @param *image - cod_t to be decoded
 * @param *dictionary - dictionary for decoding
 * @return int - 1 in case of success
 */
pgm_t* create_pgm(const cod_t *image, const dict_t *dictionary){

	pgm_t *decoded_pgm = MALLOC(sizeof(pgm_t));

	//copy image dimensions and type
	decoded_pgm->height = image->height;
	decoded_pgm->width = image->width;
	decoded_pgm->type = image->file_type;
	decoded_pgm->max_value = 0;
	//remove .cod and add .pgm extension to filename
	char *file = edit_extension(image->filename, '.', '/');
	strcpy(decoded_pgm->filename, file);
	//allocate necessary memory for the pixels
	allocate_pgm(decoded_pgm);

	int x, y;
	int auxX, auxY;
	//offset to move from block to block
	int xIn = 0, yIn = 0;

	// These for cycles will get a dictionary block for each position in the cod struct
	for(y = 0; y < image->cod_height; y++){
		for(auxY = 0; auxY < dictionary->block_height; auxY++){
			for(x = 0; x < image->cod_width; x++){
				for(auxX = 0; auxX < dictionary->block_width; auxX++){
					if(running){
						if(image->matrix_ptr[y][x] >= dictionary->num_blocks){
							PRINT(stderr, "FAILURE:index out of range of file '%s' with dict '%s'\n", image->filename, dictionary->filename);
							return NULL;
						}
						//get equivalent pixel from dictionary
						decoded_pgm->matrix_ptr[yIn][xIn] = 
								dictionary->matrix_blocks[image->matrix_ptr[y][x]][(auxY*dictionary->block_width)+auxX];
						//copy pixel value if it's higher than the previous one
						if(decoded_pgm->matrix_ptr[yIn][xIn] > decoded_pgm->max_value){
							decoded_pgm->max_value = decoded_pgm->matrix_ptr[yIn][xIn];
						}
					}else{
						return NULL;
					}
					xIn++;
				}
			}
			yIn++;
			xIn = 0;	
		}	
	}

	return decoded_pgm;
}