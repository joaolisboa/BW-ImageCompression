/**
 * @file file_handler.c
 * @brief Filename related functions
 * @author João Lisboa 2130158, Luís Guerra 2130159
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *edit_extension (const char* mystr, char dot, char sep);	
void path_combine(const char* parent, const char *entry, char **target);

/**
* @fn void path_combine(const char* parent, const char *entry, char**target)
* @brief This function will add the path of a file to it's filename
* @param receives a string with the path
* @param receives a string with the filename with no path
* @param receives a pointer to where the filename is
*/
void path_combine(const char* parent, const char *entry, char**target){
	if((*target=realloc(*target, (strlen(parent) + strlen(entry) + 2)*sizeof(char))) == NULL){
		printf("Error allocating\n");
	}

	int last = strlen(parent);

	if(parent[last-1]=='/'){
		sprintf(*target, "%s%s", parent, entry);
	}else{
		sprintf(*target, "%s/%s", parent, entry);
	}
}

/**
* @fn char *edit_extension(const char* mystr, char dot, char sep)
* @brief This function will remove the extension and the new .pgm extension
* @param receives a string with the original filename
* @param receives a char with the separator for the extension(usually '.')
* @param receives a char with the separator for the path
* @return returns the original filename with the previous extension changed to .pgm
*/
char *edit_extension(const char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;

    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    if (lastdot != NULL) {
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                *lastdot = '\0';
            }
        } else {
            *lastdot = '\0';
        }
        strcat(retstr, ".pgm");
    }
    return retstr;
}