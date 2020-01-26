#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256
typedef enum
{
	ROCK,
	PAPER,
	SCISSORS,
	LIZARD,
	SPOCK
}gameMove;

void PlayAndPrintGame(char* opponent_name, char* your_move, char* move_player_two, char* winner);
char* enumToStringMove(gameMove move);