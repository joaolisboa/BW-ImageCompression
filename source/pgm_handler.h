/**
 * @file pgm_handler.h
 * @brief Header for reading and writing pgm file functions
 *
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#ifndef _PGM_HANDLER_H
#define _PGM_HANDLER_H

int read_header_PGM(FILE*, pgm_t*);
int read_file_pgm(const char*, pgm_t*);
int read_body_p2(FILE*, pgm_t*);
int read_body_p5(FILE*, pgm_t*);
int read_dict(char*, dict_t*);
int write_pgm_file(const pgm_t*);

#endif