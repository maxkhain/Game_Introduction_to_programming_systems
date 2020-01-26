#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Messages.h"

#define MAX_BUFFER_SIZE 256
#define HEX_BASE 10
typedef enum
{
	ROCK,
	PAPER,
	SCISSORS,
	LIZARD,
	SPOCK
}gameMove;

char* PlayGame(char* move_player_one, char* move_player_two);
//char* PlayAndEncodeGame(char* opponent_name, gameMove opponent_move, gameMove move_player_two);
char* enumToStringMove(gameMove move);