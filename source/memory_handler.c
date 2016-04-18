/**
 * @file memory_handler.c
 * @brief Memory related functions
 *
 * This file includes functions to allocate memory for pgm_t and cod_t structures
 * as well as functions to free memory for the same structures and dict_t
 *
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "memory.h"


void free_image(cod_t *image);
void free_dictionary(dict_t *dictionary);
void free_pgm(pgm_t *pgm);
void allocate_pgm(pgm_t *image);
void allocate_cod(cod_t *cod);

/**
 * @fn void free_image(cod_t *image)
 * @brief Free memory of a cod_t structure
 * @param pointer to cod_t structure
 */
void free_image(cod_t *image){
	free(image->matrix_ptr[0]);
	free(image->matrix_ptr);
}

/**
 * @fn void free_dictionary(dict_t *dictionary)
 * @brief Free memory of a dict_t structure
 * @param pointer to dict_t structure
 */
void free_dictionary(dict_t *dictionary){
	free(dictionary->matrix_blocks[0]);
	free(dictionary->matrix_blocks);
}

/**
 * @fn void free_pgm(pgm_t *image)
 * @brief Free memory of a pgm_t structure
 * @param pointer to pgm_t structure
 */
void free_pgm(pgm_t *pgm){
	free(pgm->matrix_ptr[0]);
	free(pgm->matrix_ptr);
}

/**
 * @fn void allocate_cod(cod_t *cod)
 * @brief Allocate memory for the indexes in the cod_t strcuture
 * @param pointer to cod_t structure
 */
void allocate_cod(cod_t *cod){
	int cod_width = cod->width/cod->dic_width;
	int cod_height = cod->height/cod->dic_height;

	cod->matrix_ptr = MALLOC(cod_height * sizeof(unsigned short*));
	unsigned short* aux_ptr = MALLOC(cod_width * cod_height * sizeof(unsigned short));

	int i;
	for (i = 0; i < cod_height; i++) {
  		cod->matrix_ptr[i] = &(aux_ptr[i*cod_width]);
	}
}

/**
 * @fn void allocate_pgm(pgm_t *image)
 * @brief Allocate memory for the pixels in the pgm_t strcuture
 * @param pointer to pgm_t structure
 */
void allocate_pgm(pgm_t *image){
	image->matrix_ptr = MALLOC(image->height * sizeof(unsigned short*));
	unsigned short* aux_ptr = MALLOC(image->width * image->height * sizeof(unsigned short));

	int i;
	for (i = 0; i < image->height; i++) {
  		image->matrix_ptr[i] = &(aux_ptr[i*image->width]);
	}
}