/**
 * @file cod_handler.h
 * @brief Header file for .cod functions
 *
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#ifndef _COD_HANDLER_H
#define _COD_HANDLER_H

int read_header_COD(FILE*, cod_t*);
int read_file_COD(const char*, cod_t*);
int read_file_Z5(FILE*, cod_t*);
int read_file_Z2(FILE*, cod_t*);
int write_cod_file(const cod_t*);

#endif