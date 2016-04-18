/**
 * @file validations.c
 * @brief Functions for validations of dictionary and images
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"

int canBeDivided(int xI, int yI, int xB, int yB);
int path_check_extension(const char *fullname, const char *extension);
int validate_dict(FILE *fptr, dict_t *dictionary);
int matching_image_dimensions(pgm_t imageA, pgm_t imageB);
int validate_dict_dimensions(cod_t image, dict_t dictionary);

/**
 * @fn int validate_dict(FILE *fptr, dict_t *dictionary)
 * @brief Validate a .dic file
 * @param *fptr - file pointer
 * @param *dictionary - struct to be filed with the dictionary data
 * @return int - 1 in case of success, int - 0 in case of error
 */
int validate_dict(FILE *fptr, dict_t *dictionary){
	char *line = NULL;		
	size_t len = 0;
	ssize_t read;
	char str[500];

	if(path_check_extension(dictionary->filename, DIC_EXTENSION)){
		PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
		return 0;
	}

	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			if(isdigit(str[0])){
				int n = sscanf(line, "%d", &dictionary->num_blocks);
				if(n != 1 || dictionary->num_blocks > 65535){
					PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
				return 0;
			}
		}
	}
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			if(isdigit(str[0])){
				int n = sscanf(line, "%d", &dictionary->block_width);

				if(n!=1 || dictionary->block_width < 1){
					PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
				return 0;
			}
		}
	}
	while((read = getline(&line, &len, fptr)) != -1){
		read = sscanf(line, "%s", str);
		if(str[0] != '#' && read != -1){
			if(isdigit(str[0])){
				int n = sscanf(line, "%d", &dictionary->block_height);
				if(n!=1 || dictionary->block_height < 1){
					PRINT(stderr, "FAILURE:file '%s 'is not a valid DIC file\n", dictionary->filename);
					return 0;
				}
				break;
			}else{
				PRINT(stderr, "FAILURE:file '%s' is not a valid DIC file\n", dictionary->filename);
				return 0;
			}
		}
	}
	free(line);
	return 1;
}

/**
 * @fn int canBeDivided(int xI, int yI, int xB, int yB)
 * @brief Checks if two ints can be divided by other two ints
 * @param xI - int x to be divided
 * @param yI - int y to be divided
 * @param xB - int x to be divided by
 * @param yB - int x to be divided by
 * @return int - 1 in case of true, int - 0 in case of false
 */
int canBeDivided(int xI, int yI, int xB, int yB){
	if(xI % xB == 0){
		if(yI % yB == 0){
			return 1;
		}
	}
	return 0;
}

/**
 * @fn int path_check_extension(const char *filename, const char *extension)
 * @brief Checks if the file name in *filename ends with the extension in *extension
 * @param *filename - filename to be tested
 * @param *extension - extension to apply
 * @return int - 1 in case of true, int - 0 in case of false
 */
int path_check_extension(const char *filename, const char *extension){
	 char *dot_pos = strrchr(filename, '.');
	 if(dot_pos != NULL){
	 	if(strcasecmp(dot_pos, extension)){
	 		return 1;
	 	}
	 }
	 return 0;
}

/**
 * @fn int matching_image_dimensions(pgm_t imageA, pgm_t imageB)
 * @brief Checks if two images have the same dimensions
 * @param imageA - first image in comparison
 * @param imageB - second image in comparison
 * @return int - 1 in case of true, int - 0 in case of false
 */
int matching_image_dimensions(pgm_t imageA, pgm_t imageB){
	if(imageA.width == imageB.width && imageA.height == imageB.height){
		return 1;	
	}

	return 0;
}

/**
 * @fn int validate_dict_dimensions(cod_t image, dict_t dictionary)
 * @brief Checks if a dictionary has the apropriate block dimensions and number of blocks to decode a compressed image
 * @param image - image to be tested
 * @param dictionary - dictionary to be tested
 * @return int - 1 in case of the dictionary being valid, int - 0 in case of incompatible dimensions and int - -1 in case of the dictionary lacking blocks
 */
int validate_dict_dimensions(cod_t image, dict_t dictionary){
	if(image.dic_width != dictionary.block_width || image.dic_height != dictionary.block_height){
		return 0;
	}
	else if(image.max_dic_index >= dictionary.num_blocks){
		return -1;
	}
	return 1;
}