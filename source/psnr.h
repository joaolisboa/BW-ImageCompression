/**
 * @file psnr.h
 * @brief Header for PSNR calculation functions
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#ifndef _PSNR_H
#define _PSNR_H

double calculate_PSNR(pgm_t, pgm_t);
double calculate_MSE(pgm_t, pgm_t);
double process_PSNR(char*, char*);

#endif