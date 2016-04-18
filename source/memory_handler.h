/**
 * @file memory_handler.h
 * @brief Header for memory handling functions
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#ifndef _MEMORY_HANDLER_H
#define _MEMORY_HANDLER_H

void free_image(cod_t*);
void free_dictionary(dict_t*);
void free_pgm(pgm_t*);
void allocate_pgm(pgm_t*);
void allocate_cod(cod_t*);

#endif