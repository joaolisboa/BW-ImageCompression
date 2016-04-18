/**
 * @file structs.h
 * @brief Data structures.
 * Data structures for PGM images, COD files and Dictionary files
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include <pthread.h>

#define IO_ERROR 1
#define THREAD_ERROR 2
#define	Z2 2
#define Z5 5
#define P2 2
#define P5 5
#define DIC_EXTENSION ".dic"
#define COD_EXTENSION ".cod"

int running;							/**< @brief Global running variable */

int flag;
#define PRINT(...)if(flag){fprintf(__VA_ARGS__);}

/**
 * @struct dict_t 
 * @brief Dictionary Structure
 */
typedef struct dict_t{
	char filename[512];					/**< @brief Dictionary filename */
	int num_blocks;						/**< @brief Number of blocks */
	int block_height;					/**< @brief Block height */
	int block_width;					/**< @brief Block width  */
	unsigned short **matrix_blocks;		/**< @brief Blocks organized one block per line */
}dict_t;

/**
 * @struct cod_t 
 * @brief COD Structure
 */
typedef struct cod_t{
	int file_type;						/**< @brief COD file type - Z2(text) or Z5(binary) */
	char filename[512];					/**< @brief COD filename */
	int height;							/**< @brief Original image height */
	int width;							/**< @brief Original image width */
	int max_dic_index;					/**< @brief Number of block of the dictionary */
	int dic_height;						/**< @brief Dictionary block height */
	int dic_width;						/**< @brief Dictionary block width */
	int max_value;						/**< @brief Max dictionary index - highest index used */
	int cod_height;						/**< @brief COD height */ 
	int cod_width;						/**< @brief COD width */ 
	unsigned short **matrix_ptr;		/**< @brief Indexes */ 
}cod_t;

/**
 * @struct pgm_t 
 * @brief PGM image Structure
 */
typedef struct pgm_t{
	int type;							/**< @brief P2 or P5 */
	char filename[512];					/**< @brief Image filename */
	int height;							/**< @brief Image height in pixels */
	int width;							/**< @brief Image width in pixels */
	int max_value;						/**< @brief Highest pixel value */
	unsigned short **matrix_ptr;		/**< @brief Pixels */
}pgm_t;

/**
 * @struct PARAM_T
 * @brief Parameters for threads structure
 * Structure where data will be stored to sync between threads
 */
typedef struct {
	pgm_t *pgm;							/**< @brief Image  */
	dict_t *dict;						/**< @brief Dictionary */
	cod_t *cod;							/**< @brief COD */
	unsigned short y, x;				/**< @brief Offset */
	pthread_mutex_t mutex;				/**< @brief Mutex used for synching between threads */
}PARAM_T;

#endif