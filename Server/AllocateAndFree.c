// --- AllocateAndFree module ---
// This module is responsible for creating a creating arrays - string arrays, integer arrays
// and character arrays.

#include "AllocateAndFree.h"
#include <stdio.h>
#include "PrintError.h"
#include <stdlib.h>

char** AllocateArrayOfStrings(int number_of_string, int number_of_chars_in_string)
{
	char** array_of_strings = (char**)calloc(number_of_string, sizeof(char*));
	if (NULL == array_of_strings) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return NULL;
	}
	for (int array_index = 0; array_index < number_of_string; array_index++) {
		array_of_strings[array_index] = (char*)calloc(number_of_chars_in_string, sizeof(char));
		if (NULL == array_of_strings[array_index]) {
			for (int j = 0; j < array_index; array_index++)
				free(array_of_strings[j]);
			PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
			return NULL;
		}
	}
	return array_of_strings;
}


char* AllocateString(int number_of_chars_in_string)
{
	char *string = (char*)malloc(sizeof(char)*number_of_chars_in_string);
	if (NULL == string) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return NULL;
	}
	return string;
}


