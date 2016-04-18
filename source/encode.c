/**
 * @file encode.c
 * @brief Encode functions
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>

#include "structs.h"
#include "debug.h"
#include "memory.h"
#include "pgm_handler.h"
#include "cod_handler.h"
#include "dict_handler.h"
#include "memory_handler.h"
#include "validations.h"

int encode(char *pgm_filename, char *dic_filename);
int get_best_index(const dict_t *dictionary, int *block);
int parallel_encode_image(char *pgm_filename, char *dic_filename, unsigned int threads);
cod_t* parallel_encode(pgm_t *pgm, dict_t *dic, unsigned int threads);
void *create_cod_parallel(void *arg);
cod_t* create_cod(const pgm_t *image, const dict_t *dictionary);

/**
 * @fn int encode(char *pgm_filename, char *dic_filename)
 * @brief This function is the base of the encode operation for single thread
 * @param *pgm_filename - PGM filename
 * @param *dic_filename - Dictionary filename
 * @return int - 1 in case of success
 */
int encode(char *pgm_filename, char *dic_filename){
	pgm_t image;
	dict_t dictionary;
	int success = 0;

	//read pgm image 
	if(read_file_pgm(pgm_filename, &image)){
		//read dictionary
		if(read_dict(dic_filename, &dictionary)){
			//determine if the image can be encoded with the given dictionary
			if(canBeDivided(image.width, image.height, dictionary.block_width, dictionary.block_height) && running){	
				cod_t *coded_image;
				//create coded image - encode operation
				coded_image = create_cod(&image, &dictionary);
				//write success message if the encode operation was successful and the file was written
				if(coded_image != NULL && write_cod_file(coded_image) && running){
					fprintf(stderr, "operation: %s ENCODE %s\n", pgm_filename, dic_filename);
					free_image(coded_image);
					success = 1;
				}
				free(coded_image);
			}else{
				PRINT(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'\n", pgm_filename, dic_filename);
			}
			free_dictionary(&dictionary);
		}
		free_pgm(&image);
	}
	return success;
}

/**
 * @fn cod_t* create_cod(const pgm_t *image, const dict_t *dictionary)
 * @brief Encode operation for single thread
 * @param *image - PGM image to be encoded
 * @param *dictionary - Dictionary used for encoding
 * @return cod_t* - Encoded image
 */
cod_t* create_cod(const pgm_t *image, const dict_t *dictionary){

	//create coded image
	cod_t *coded_image = MALLOC(sizeof(cod_t));
	coded_image->file_type = image->type;
	
	//add .cod extension to the filename
	strcpy(coded_image->filename, image->filename);
	strcat(coded_image->filename, COD_EXTENSION);
	
	//copy original image dimensions
	coded_image->height = image->height;
	coded_image->width = image->width;
	//store encoded image dimensions
	coded_image->cod_height = image->height/dictionary->block_height;
	coded_image->cod_width = image->width/dictionary->block_width;
	//copy block dimensions of the used dictionary
	coded_image->dic_height = dictionary->block_height;
	coded_image->dic_width = dictionary->block_width;
	coded_image->max_dic_index = dictionary->num_blocks-1;
	coded_image->max_value = 0;
	//allocate memory for indexes
	allocate_cod(coded_image);

	int x, y;
	int auxX, auxY;
	//offsets for the block indexes
	int xIn = 0, yIn = 0;
	int num = 0;
	int index = 0;
	//array used to store a block of pixels from the image
	int block[dictionary->block_height*dictionary->block_width];

	for(y = 0; y < coded_image->cod_height; y++){
		for(x = 0; x < coded_image->cod_width; x++){
			if(running){
				//go from block to block
				//for each block, store the image pixels in the block[] array
				for(auxY = 0; auxY < dictionary->block_height; auxY++){
					for(auxX = 0; auxX < dictionary->block_width; auxX++){
						block[num] = image->matrix_ptr[auxY + yIn][auxX + xIn];
						num++;
					}			
				}

				num=0;
				//get best index from the dictionary and store in the encoded image
				index = get_best_index(dictionary, block);
				coded_image->matrix_ptr[y][x] = index;
				//store max_value if index is higher than the previous max_index
				if(index > coded_image->max_value){
					coded_image->max_value = index;
				}
			}else{
				return NULL;
			}
			xIn += dictionary->block_width;
		}	
		yIn += dictionary->block_height;
		xIn = 0;
	}

	return coded_image;
}

/**
 * @fn int parallel_encode_image(char *pgm_filename, char *dic_filename, unsigned int threads)
 * @brief Base of the parallel encode operation
 * @param *pgm_filename - PGM image filename
 * @param *dic_filename - Dictionary filename
 * @param threads - Number of threads
 * @return int - 1 in case of success
 */
int parallel_encode_image(char *pgm_filename, char *dic_filename, unsigned int threads){
	int success = 0;

	if(threads == 0){
		threads = sysconf(_SC_NPROCESSORS_ONLN);
	}
	if(threads == 1){
		//encode operation for a single thread
		success = encode(pgm_filename, dic_filename);
	}else{
		//image to be encoded
		pgm_t image;
		//dictionary used for the encode operation
		dict_t dictionary;

		//read pgm image from given filename
		if(read_file_pgm(pgm_filename, &image)){
			//read dictionary
			if(read_dict(dic_filename, &dictionary)){
				//determine if the image can be encoded with the dictionary
				if(!canBeDivided(image.width, image.height, dictionary.block_width, dictionary.block_height)){
					fprintf(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'\n", pgm_filename, dic_filename);
					success = 0;
				}else{
					cod_t *coded_image;
					//encode operation with multiple threads
					coded_image = parallel_encode(&image, &dictionary, threads);
					//write success image and free memory if encode operation was successfull
					//and file was written
					if(coded_image != NULL && write_cod_file(coded_image) && running){
						fprintf(stderr, "operation: %s ENCODE %s\n", pgm_filename, dic_filename);
						free_image(coded_image);
						success = 1;
					}
				}
				free_dictionary(&dictionary);
			}
			free_pgm(&image);
		}
	}
	return success;
}

/**
 * @fn cod_t* parallel_encode(pgm_t *pgm, dict_t *dict, unsigned int threads)
 * @brief Initialization of the data for threads and the creation of threads
 * @param *pgm - PGM image used for encoding
 * @param *dict - Dictionary used for encoding
 * @param threads - Number of threads
 * @return cod_t* - Encoded image
 */
cod_t* parallel_encode(pgm_t *pgm, dict_t *dict, unsigned int threads){
	//param struct includes data that will be shared by the threads
	PARAM_T param;
	//copy pgm image and dictionary to the param structure
	param.pgm = pgm;
	param.dict = dict;
	param.cod = MALLOC(sizeof(cod_t));
	//initialize offsets for the encode operation
	param.y = 0;
	param.x = 0;

	//copy original image dimensions
	param.cod->width = pgm->width;
	param.cod->height = pgm->height;
	//copy image type
	param.cod->file_type = pgm->type;
	//store encoded image dimensions
	param.cod->cod_height = pgm->height/dict->block_height;
	param.cod->cod_width = pgm->width/dict->block_width;
	//copy block dimensions of the used dictionary
	param.cod->dic_height = dict->block_height;
	param.cod->dic_width = dict->block_width;
	param.cod->max_dic_index = dict->num_blocks-1;
	param.cod->max_value = 0;

	//add .cod extension to the filename
	strcpy(param.cod->filename, pgm->filename);
	strcat(param.cod->filename, COD_EXTENSION);
	//allocate memory for indexes 
	allocate_cod(param.cod);

	if(pthread_mutex_init(&param.mutex, NULL) != 0){
		ERROR(THREAD_ERROR, "Error initializing pthread mutex\n");
	}

	pthread_t *tids = MALLOC(sizeof(pthread_t) * threads);
	if(tids == NULL){
		ERROR(THREAD_ERROR, "Error allocating threads\n");
	}

	unsigned int i;
	for(i = 0; i < threads; i++){
		if(pthread_create(&tids[i], NULL, create_cod_parallel, &param) != 0){
			ERROR(THREAD_ERROR, "Error creating threads\n");
		}
	}
	for(i = 0; i < threads; i++){
		if(pthread_join(tids[i], NULL) != 0){
			ERROR(THREAD_ERROR, "Error joining threads\n");
		}
	}
	free(tids);
	if(pthread_mutex_destroy(&param.mutex) != 0){
		ERROR(THREAD_ERROR, "Error while finishing threads\n");
	}
	return param.cod;
}

/**
 * @fn void *create_cod_parallel(void *arg)
 * @brief Encode operation with threads
 * @param *arg - param structure
 */
void *create_cod_parallel(void *arg){
	PARAM_T *param = (PARAM_T*)arg;

	int index = 0;
	//array used to store a block of pixels from the image
	int block[param->dict->block_height * param->dict->block_width];
	int auxX, auxY;
	int num = 0;
	int y, x;
	//offsets for the block indexes
	int yIn, xIn;

	while(running){
		//lock mutex and get offsets 'y' and 'x'
		if(pthread_mutex_lock(&(param->mutex)) != 0){
			ERROR(THREAD_ERROR, "Error locking mutex\n");
		}
		x = param->x;
		param->x ++;
		y = param->y;
		if(param->x == param->cod->cod_width){
			param->y ++;
			param->x = 0;
		}
		//unlock mutex after data has been read and altered
		if(pthread_mutex_unlock(&(param->mutex)) != 0){
			ERROR(THREAD_ERROR, "Error unlocking mutex\n");
		}
		if(y == param->cod->cod_height){
			//reached end of the image
			break;
		}
		//go from block to block
		//for each block, store the image pixels in the block[] array
		xIn = x * param->dict->block_width;
		yIn = y * param->dict->block_height;
		for(auxY = 0; auxY < param->dict->block_height; auxY++){
			for(auxX = 0; auxX < param->dict->block_width; auxX++){
				block[num] = param->pgm->matrix_ptr[auxY + yIn][auxX + xIn];
				num++;
			}			
		}
		num=0;
		//get best index from the dictionary and store in the encoded image
		index = get_best_index(param->dict, block);
		param->cod->matrix_ptr[y][x] = index;
		if(pthread_mutex_lock(&(param->mutex)) != 0){
			ERROR(THREAD_ERROR, "Error locking mutex\n");
		}
		//store max_value if index is higher than the previous max_index
		if(index > param->cod->max_value) param->cod->max_value = index;
		if(pthread_mutex_unlock(&(param->mutex)) != 0){
			ERROR(THREAD_ERROR, "Error unlocking mutex\n");
		}
	}
	return 0;
}

/**
 * @fn int get_best_index(const dict_t *dictionary, int *block)
 * @brief Get the index of a dictionary block that is the closest to the original image
 * @param *dictionary - Dictionary 
 * @param *block - block of pixels from the original image
 * @param int - best index of the dictionary
 */
int get_best_index(const dict_t *dictionary, int *block){
	unsigned long long erro;
	unsigned long long best = ULLONG_MAX;
	int x, y;
	int index = 0;

	//go through the dictionary
	//and find a block that is the most similar to the original
	for(y = 0; y < dictionary->num_blocks; y++){
		erro = 0;
		if(running){
			//quadratic error calculation for each block
			for(x = 0; x < dictionary->block_height * dictionary->block_width; x++){
				erro += (block[x] - dictionary->matrix_blocks[y][x]) * (block[x] - dictionary->matrix_blocks[y][x]);
			}
			if(erro < best){
				best = erro;
				index = y;
			}
		}else{
			break;
		}
	}
	//return the index of the most similar block
	return index;
}


