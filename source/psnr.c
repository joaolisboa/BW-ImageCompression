/**
 * @file psnr.c
 * @brief Calculate PSNR - difference between pgm images
 * @author João Lisboa - 2130158, Luís Guerra - 2130159
 */

#include <stdio.h>
#include <math.h>

#include "structs.h"
#include "pgm_handler.h"
#include "memory_handler.h"
#include "validations.h"

double calculate_PSNR(pgm_t imageF, pgm_t imageG);
double calculate_MSE(pgm_t imageF, pgm_t imageG);
double process_PSNR(char* filenameG, char* filenameF);

/**
 * @fn double calculate_PSNR(pgm_t imageF, pgm_t imageG)
 * @brief Calculates the Peak signal-to-noise ratio between two images
 * @param imageF - structure with the original image
 * @param imageG - structure with the decoded image
 * @return double - psnr value in case of success, double - 0 in case of error
 */
double calculate_PSNR(pgm_t imageF, pgm_t imageG){
	double mse = calculate_MSE(imageF, imageG);
	if(mse != -1){	
		double psnr = 20 * log10((imageF.max_value)/sqrt(mse));
	   	return psnr;
	}
	return 0;
}

/**
 * @fn double calculate_MSE(pgm_t imageF, pgm_t imageG)
 * @brief Calculates the Mean squared error between two images
 * @param imageF - structure with the original image
 * @param imageG - structure with the decoded image
 * @return double - mse value in case of success, double - -1 in case of error
 */
double calculate_MSE(pgm_t imageF, pgm_t imageG){
	double mse = 0;
	int i, j;

	for(i=0; i < imageF.height; i++){
		for(j=0; j < imageF.width; j++){
			if(!running){
				return -1;
			}
			mse += ((imageF.matrix_ptr[i][j]) - (imageG.matrix_ptr[i][j])) * ((imageF.matrix_ptr[i][j]) - (imageG.matrix_ptr[i][j]));
		}
	}
	mse = mse/(imageF.width * imageF.height);

	return mse;
}

/**
 * @fn double process_PSNR(char* filenameF, char* filenameG)
 * @brief Handles the error checking between the two images to verify they are ready to be used in the calculations involved by the PSNR calculation.
 * @param filenameF - name of the original image
 * @param filenameG - name of the decoded image
 * @return double - psnr value in case of success, double - 0 in case of error
 */
double process_PSNR(char* filenameF, char* filenameG){
	pgm_t imageF;
	pgm_t imageG;
	double psnr = 0;
	if(read_file_pgm(filenameF, &imageF) && read_file_pgm(filenameG, &imageG)){
		if(!matching_image_dimensions(imageG, imageF)){
			fprintf(stderr, "FAILURE:incompatible dimensions for PSNR calculation between files '%s' and '%s'\n", imageF.filename, imageG.filename);
		}else if(running){
			psnr = calculate_PSNR(imageF, imageG);
			free_pgm(&imageF);
			free_pgm(&imageG);
		}
	}
	return psnr;
}