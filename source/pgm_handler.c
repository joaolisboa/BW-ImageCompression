/**
 * @file pgm_handler.c
 * @author João Emanuel de Oliveira Lisboa - 2130158
 * @author Luís Eduardo Moutinho Guerra - 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "debug.h"
#include "memory_handler.h"


int read_header_PGM(FILE *file, pgm_t *image);
int read_file_pgm(char *filename, pgm_t *image);
int read_body_p2(FILE *fptr, pgm_t *image);
int read_body_p5(FILE *fptr, pgm_t *image);
int write_pgm_file(const pgm_t *decoded_image);

/**
* @fn int read_body_p5(FILE *fptr, pgm_t *image)
* @brief This function reads a .pgm body in binary mode
* @param receives a pointer to the FILE being read
* @param receives a pointer to the image in a pgm_t struct where the values will be stored
* @return int, 1 in case of success
*/
int read_body_p5(FILE *fptr, pgm_t *image){
	int x, y;
	int n = 0;
	size_t read;
	int bytes = 1;

	if(image->max_value > 256){
		bytes = 2;
	}
	for(y = 0; y < image->height; y++){
		for(x = 0; x < image->width; x++){
			if((read = fread(&n, 1, bytes, fptr)) != 1){
				return 0;
			}else{
				image->matrix_ptr[y][x] = n;
			}
		}
	}	
	return 1;	
}

/**
* @fn int read_body_p2(FILE *fptr, pgm_t *image)
* @brief This function reads a .pgm body in text mode
* @param receives a pointer to the FILE being read
* @param receives a pointer to the image in a pgm_t struct where the values will be stored
* @return int, 1 in case of success
*/
int read_body_p2(FILE *fptr, pgm_t *image){
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int x = 0;
	int y = 0;

	int n;
	int num = 0;
	char *token, *save_ptr = NULL;
	char *endptr = NULL;
	char str[512];

	while(getline(&line, &len, fptr) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			if(y >= image->height){
				PRINT(stderr, "FAILURE:file '%s' is not a valid PGM file\n", image->filename);
				return 0;
			}
			token = strtok_r(line, " \r\t\n\0", &save_ptr);
			
			while(token != NULL) {
				if(token[0] != '#'){
					endptr = NULL;
					n = strtol(token, &endptr, 10);
					token = strtok_r(NULL, " \r\t\n\0", &save_ptr);

					image->matrix_ptr[y][x] = n;
					x++;
					num++;
					if(x == image->width){
						y++;
						x = 0;
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
* @fn int read_file_pgm(char *filename, pgm_t *image)
* @brief This function will open and read the file
* @param receives the name of the file that will be open
* @param receives a pointer to the image in a pgm_t struct where the values will be stored
* @return int, 1 in case of success
*/
int read_file_pgm(char *filename, pgm_t *image){

	strcpy(image->filename,filename);
	FILE *fptr = fopen(filename, "r");
	if(fptr == NULL){
		ERROR(IO_ERROR, "File %s does not exists\n", filename);
	}else{
		if(read_header_PGM(fptr, image) && running){
			allocate_pgm(image);
			int success;
			switch(image->type){
				case P2:
					success = read_body_p2(fptr, image);
					break;
				case P5:
					success = read_body_p5(fptr, image);
					break;
			}
				
			if(fclose(fptr) != 0){
				ERROR(IO_ERROR, "File %s could not be closed properly\n", filename);
			}
			return success;
		}	
	}
	return 0;
}

/**
* @fn int read_header_PGM(FILE *file, pgm_t *image)
* @brief This function will read the header of a.pgm file
* @param receives the name of the file that will be open
* @param receives a pointer to the image in a pgm_t struct where the values will be stored
* @return int, 1 in case of success
*/
int read_header_PGM(FILE *file, pgm_t *image){
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char str[500];

	while((read = getline(&line, &len, file)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			read = sscanf(line, "P%d", &image->type);
			if(read != 1 || (image->type != P2 && image->type != P5)) {
				PRINT(stderr, "FAILURE:file '%s' contains an invalid PGM format\n", image->filename);
				return 0;
			}
			break;
		}
	}

	while((read = getline(&line, &len, file)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			read = sscanf(line, "%d %d", &image->width, &image->height);
			if(read != 2 || image->width < 1 || image->height < 1){
				PRINT(stderr, "FAILURE:file '%s' is not a valid PGM file\n", image->filename);
				return 0;
			}
			break;
		}
	}

	while((read = getline(&line, &len, file)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			read = sscanf(line, "%d", &image->max_value);
			if(read != 1 || image->max_value < 0 || image->max_value > 65535){
				PRINT(stderr, "FAILURE:file '%s' is not a valid PGM file\n", image->filename);
				return 0;
			}
			break;
		}
	}
	free(line);

	return 1;
}

/**
* @fn int write_pgm_file(const pgm_t *decoded_pgm)
* @brief This function will write a pgm file
* @param receives a pointer to a pgm_t structure
* @return int, 1 in case of success
*/
int write_pgm_file(const pgm_t *decoded_pgm){

	/*char template[512] = "tmp_XXXXXX";
	int fd = mkstemp(template);
	FILE *fptr = fdopen(fd, "w");*/
	if(running){
		FILE *fptr = fopen(decoded_pgm->filename, "w");
		if(fptr == NULL){
			ERROR(IO_ERROR, "Error while opening file %s\n", decoded_pgm->filename);
		}else{
			fprintf(fptr, "P%d\n", decoded_pgm->type);
			fprintf(fptr, "%d %d\n", decoded_pgm->width, decoded_pgm->height);
			fprintf(fptr, "%d\n", decoded_pgm->max_value);

			int x, y;
			if(decoded_pgm->type == P2){
				for(y = 0; y < decoded_pgm->height && running; y++){
					for(x = 0; x < decoded_pgm->width; x++){
						fprintf(fptr, "%d\n", decoded_pgm->matrix_ptr[y][x]);
					}
				}
			}else if(decoded_pgm->type == P5){
				int bytes = 1;
				if(decoded_pgm->max_value > 256){
					bytes = 2;
				}
				for(y = 0; y < decoded_pgm->height && running; y++){
					for(x = 0; x < decoded_pgm->width; x++){
						fwrite(&decoded_pgm->matrix_ptr[y][x], bytes, 1, fptr);
					}
				}
			}
			if(fclose(fptr) != 0){
				ERROR(IO_ERROR, "File %s could not be closed properly\n", decoded_pgm->filename);
			}
			//rename(template, decoded_pgm->filename);
			return 1;
		}
	}
	return 0;
}