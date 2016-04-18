/**
 * @file validations.h
 * @brief Header for validations
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#ifndef _VALIDATIONS_H
#define _VALIDATIONS_H

int canBeDivided(int, int, int, int);
int path_check_extension(const char*, const char*);
int validate_dict(FILE*, dict_t*);
int matching_image_dimensions(pgm_t, pgm_t);
int validate_dict_dimensions(cod_t, dict_t);

#endif