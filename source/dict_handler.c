/**
 * @file dict_handler.c
 * @brief Read dictionary files
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "debug.h"
#include "memory.h"
#include "validations.h"

int read_dict_blocks(FILE *fptr, dict_t *dictionary);
int read_dict(char *filename, dict_t *dictionary);

/**
 * @fn int read_dict_blocks(FILE *fptr, dict_t *dictionary)
 * @brief Read .dic file body/blocks
 * @param *fptr - .dic file pointer
 * @param *dictionary - dict_t struct where data will be stored
 * @return int - 1 in case of success
 */
int read_dict_blocks(FILE *fptr, dict_t *dictionary){

	int block_size = dictionary->block_width * dictionary->block_height;
	//allocate necessary memory for dictionary blocks
	dictionary->matrix_blocks = MALLOC(dictionary->num_blocks * sizeof(unsigned short*));
	unsigned short* aux_ptr = MALLOC(dictionary->num_blocks *block_size * sizeof(unsigned short));
	int i;
	for (i = 0; i < dictionary->num_blocks; i++) {
		dictionary->matrix_blocks[i] = &(aux_ptr[i*block_size]);
	}

	char *line = NULL;
	size_t len = 0;
	int x = 0;
	int y = 0;
	char str[500];
	ssize_t read;

	int n;
	char *token, *save_ptr = NULL;
	char *endptr = NULL;

	//get a line with a block
	while(getline(&line, &len, fptr) != -1){
		read = sscanf(line, "%s", str);
		//go to next line if it's a comment
		if(read == 1 && str[0] != '#'){
			if(y >= dictionary->num_blocks){
				PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
				return 0;
			}
			//split line into tokens
			token = strtok_r(line, " \r\t\n\0", &save_ptr);
			//ignore token if it's a comment
			while(token != NULL && token[0] != '#') {
				endptr = NULL;
				if(!isdigit(token[0])){
					PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
					return 0;
				}
				//get value from token
				n = strtol(token, &endptr, 10);
				dictionary->matrix_blocks[y][x] = n;
				//get next token from the line
				token = strtok_r(NULL, " \r\t\n\0", &save_ptr);
				x++;
			}

			if(x < block_size){
				PRINT(stderr, "FAILURE:missing pixels in DIC file '%s'\n", dictionary->filename);
				return 0;
			}else if(x > block_size){
				PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
				return 0;
			}

			y++;
			x = 0;
		}
	}
	free(line);

	if(y < dictionary->num_blocks){
		PRINT(stderr, "FAILURE:missing blocks in DIC file '%s'\n", dictionary->filename);
		return 0;
	}

	return 1;
}

/**
 * @fn int read_dict(char *filename, dict_t *dictionary)
 * @brief Read .dic file
 * @param *filename - .dic filename
 * @param *dictionary - dict_t struct where data will be stored
 * @return int - 1 in case of success
 */
int read_dict(char *filename, dict_t *dictionary){

	FILE *fptr = fopen(filename, "r");
	if(fptr == NULL){
		ERROR(IO_ERROR, "File %s does not exist\n", filename);
		return 0;
	}

	strcpy(dictionary->filename,filename);
	//read and validate dictionary header
	if(!validate_dict(fptr, dictionary)){
		return 0;
	}
	//read dictionary body/blocks
	if(!read_dict_blocks(fptr, dictionary)){
		return 0;
	}

	if(fclose(fptr) != 0){
		ERROR(IO_ERROR, "An error ocurred while closing %s\n", filename);
		return 0;
	}

	return 1;
}