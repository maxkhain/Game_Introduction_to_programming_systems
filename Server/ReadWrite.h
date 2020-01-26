#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>



#define MAX_BUFFER_SIZE 1024
#define MAX_NUM_OF_ROOMS 5
#define MAX_NUM_OF_RESIDENTS 15
#define DEC_BASE 10
#define MAX_NUM_SERVERS 10
#define START_SIZE_OF_STRING 25

void Initialize();
char** ReadScoreTable();
char *inputString(FILE* fp, size_t size);
void WriteToScoreTable(char* player_one_name, char* player_two_name, int winner);
void WriteToCSV(FILE* fp, char* player_one_name, char* player_two_name, int winner);

