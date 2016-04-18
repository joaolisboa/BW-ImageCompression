/**
 * @file cod_handler.c
 * @brief Functions to handle .cod files
 *
 * Includes functions to read and write .cod files. This includes
 * functions to read the header and the body in text or binary mode.
 *
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "debug.h"
#include "memory_handler.h"
#include "validations.h"


int read_header_COD(FILE* fptr, cod_t *image);
int read_file_COD(const char *filename, cod_t *image);
int read_file_Z5(FILE *fptr, cod_t *image);
int read_file_Z2(FILE *fptr, cod_t *image);
int write_cod_file(const cod_t *image);

/**
 * @fn int write_cod_file(const cod_t *coded_image)
 * @brief Writing a .cod file with the encoded image
 * @param *coded_image - final encoded image
 * @return int - 1 in case of success
 */
int write_cod_file(const cod_t *coded_image){

	if(running){
		FILE *fptr = fopen(coded_image->filename, "w");
		if(fptr == NULL){
			ERROR(IO_ERROR, "Error while opening file %s\n", coded_image->filename);
		}else if(coded_image->max_value != -1){
			//write header to file
			fprintf(fptr, "Z%d\n", coded_image->file_type);
			fprintf(fptr, "%d %d\n", coded_image->width, coded_image->height);
			fprintf(fptr, "%d\n", coded_image->max_value);
			fprintf(fptr, "%d %d\n", coded_image->dic_width, coded_image->dic_height);

			int x, y;
			//write the dictionary indexes in text mode
			if(coded_image->file_type == P2){
				for(y = 0; y < coded_image->cod_height && running; y++){
					for(x = 0; x < coded_image->cod_width; x++){
						fprintf(fptr, "%d\n", coded_image->matrix_ptr[y][x]);
					}
				}
			//write the dictionary indexes in binary mode
			}else if(coded_image->file_type == P5){
				int bytes = 1;
				//determine how many bytes must be used to write
				if(coded_image->max_value > 256){
					bytes = 2;
				}
				for(y = 0; y < coded_image->cod_height && running; y++){
					for(x = 0; x < coded_image->cod_width; x++){
						fwrite(&coded_image->matrix_ptr[y][x], bytes, 1, fptr);
					}
				}
			}
			if(fclose(fptr) != 0){
				ERROR(IO_ERROR, "File %s could not be closed properly\n", coded_image->filename);
			}
			return 1;
		}
	}
	return 0;
}

/**
 * @fn int read_file_COD(const char *filename, cod_t *image)
 * @brief Read a .cod file with given filename
 * @param *filename - .cod filename
 * @param *image - cod_t struct where read data will be stored
 * @return int - 1 in case of success
 */
int read_file_COD(const char *filename, cod_t *image){
	int success = 0;
	FILE *fptr = fopen(filename, "r");
	if(fptr == NULL){
		ERROR(IO_ERROR, "File %s does not exists\n", filename);
	}else{
		strcpy(image->filename, filename);
		//read file header
		success = read_header_COD(fptr, image);
	}

	if(success && running){
		//allocate necessary memory for the data being read
	  	allocate_cod(image);

		switch(image->file_type){
			case Z2:
				//read file body in text mode
				success = read_file_Z2(fptr, image);
				break;

			case Z5:
				//read file body in binary mode
				success = read_file_Z5(fptr, image);
				break;
		}

		if(fclose(fptr) != 0){
			ERROR(IO_ERROR, "File %s could not be closed properly\n", filename);
		}
	}
	return success;
}

/**
 * @fn int read_file_Z2(FILE *fptr, cod_t *image)
 * @brief Read a .cod body in text mode
 * @param *fptr - .cod file
 * @param *image - cod_t struct where read data will be stored
 * @return int - 1 in case of success
 */
int read_file_Z2(FILE *fptr, cod_t *image){
	char *line = NULL;
	size_t len = 0;
	int x = 0;
	int y = 0;
	char str[512];
	ssize_t read;

	int n;
	char *token, *save_ptr = NULL;
	char *endptr = NULL;

	//store encoded image dimensions
	image->cod_width = image->width/image->dic_width;
	image->cod_height = image->height/image->dic_height;

	while(getline(&line, &len, fptr) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(str[0] != '#' && read != -1){
			if(y >= image->cod_height){
				PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
				return 0;
			}
			//split the line into tokens
			token = strtok_r(line, " \r\t\n\0", &save_ptr);
			while(token != NULL) {
				//go to next token if it's a comment
				if(token[0] != '#'){
					if(!isdigit(token[0])){
						PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
						return 0;
					}else{
						endptr = NULL;
						//get index from token
						n = strtol(token, &endptr, 10);
						//get next token
						token = strtok_r(NULL, " \r\t\n\0", &save_ptr);
						if(n > image->max_dic_index){
							PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
							return 0;
						}
						image->matrix_ptr[y][x] = n;
						x++;
						if(x == image->cod_width){
							y++;
							x = 0;
						}
					}
				}else{
					break;
				}
			}
		}
	}

	free(line);
	return 1;
}

/**
 * @fn int read_file_Z5(FILE *fptr, cod_t *image)
 * @brief Read a .cod body in binary mode
 * @param *fptr - .cod file
 * @param *image - cod_t struct where read data will be stored
 * @return int - 1 in case of success
 */
int read_file_Z5(FILE *fptr, cod_t *image){
	int x, y;
	int n = 0;
	size_t read;
	int bytes = 1;

	//store encoded image dimensions
	image->cod_width = image->width/image->dic_width;
	image->cod_height = image->height/image->dic_height;

	//determine how many bytes is required to read each index
	if(image->max_dic_index > 256){
		bytes = 2;
	}
	for(y = 0; y < image->cod_height; y++){
		for(x = 0; x < image->cod_width; x++){
			//read next number of bytes
			if((read = fread(&n, bytes, 1, fptr)) != 1){
				return 0;
			}else{
				image->matrix_ptr[y][x] = n;
			}
		}
	}
	return 1;
}

/**
 * @fn int read_header_COD(FILE* fptr, cod_t *image)
 * @brief Read a .cod header
 * @param *fptr - .cod file
 * @param *image - cod_t struct where read data will be stored
 * @return int - 1 in case of success
 */
int read_header_COD(FILE* fptr, cod_t *image){

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char str[500];

	//read file type(text/z2 or binary/z5)
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(str[0] != '#' && line[0] != '\n' && read != -1){
			if(isdigit(str[1])){
				sscanf(line, "Z%d", &image->file_type);
				if (image->file_type != Z2 && image->file_type != Z5){
					PRINT(stderr, "FAILURE:file '%s' contains an invalid COD format\n", image->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
				return 0;
			}
		}
	}

	//read original image dimensions
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(str[0] != '#' && line[0] != '\n' && read != -1){
			if(isdigit(str[0])){
				int n = sscanf(line, "%d %d", &image->width, &image->height);
				if(n!=2 || image->width < 1 || image->height < 1){
					PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
				return 0;
			}
		}
	}

	//read max index - highest index value in the body
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(str[0] != '#' && line[0] != '\n' && read != -1){
			if(isdigit(str[0])){
				int n = sscanf(line, "%d", &image->max_dic_index);

				if(n!=1 || image->max_dic_index < 0 || image->max_dic_index > 65535){
					PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
				return 0;
			}
		}
	}

	//read dictionary block dimensions
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(str[0] != '#' && line[0] != '\n' && read != -1){
			int n = sscanf(line, "%d %d", &image->dic_width, &image->dic_height);

			if(n!=2 || image->dic_width < 1 || image->dic_height < 1){
				PRINT(stderr, "FAILURE:file '%s' is not a valid COD file\n", image->filename);
				return 0;
			}

			if(!canBeDivided(image->width, image->height, image->dic_width, image->dic_height)){
				PRINT(stderr, "FAILURE:file '%s' contains an invalid block size\n", image->filename);
				return 0;
			}
			break;
		}
	}

	free(line);
	return 1;
}