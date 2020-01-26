#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Game.h"

/*
Printing game result for client
input: string opponenet name, string your move, string opponent move, pointer to char winner
logic: according to winner the function will print the result and each players move
*/
void PlayAndPrintGame(char* opponent_name,char* your_move, char* opponenet_move, char* winner)
{	
	char* result_message = (char*)malloc(MAX_BUFFER_SIZE);
	char* move_player_one_string = (char*)malloc(MAX_BUFFER_SIZE);
	char* move_player_two_string = (char*)malloc(MAX_BUFFER_SIZE);
	char first_line_string[MAX_BUFFER_SIZE], second_line_string[MAX_BUFFER_SIZE];
	

	strcpy(first_line_string, "You played: ");
	strcat(first_line_string, your_move);
	strcpy(second_line_string, opponent_name);
	strcat(second_line_string, " played: ");
	strcat(second_line_string, opponenet_move);

	strcpy(result_message, first_line_string);
	strcat(result_message, "\n");
	strcat(result_message, second_line_string);
	strcat(result_message, "\n");
	if (strncmp(winner, "0",1)!=0) {
		if (strncmp(winner, "1", 1) == 0)
			strcat(result_message, "You");		
		else if(strncmp(winner, "2", 1) == 0)
			strcat(result_message, opponent_name);
		strcat(result_message, " won!\n");
	}
	else
		strcat(result_message, "The game is a tie\n");
	printf("%s", result_message);
	free(result_message);
	free(move_player_one_string);
	free(move_player_two_string);

}


char* enumToStringMove(gameMove move)
{
	switch (move)
	{
	case ROCK:
		return "Rock";
		break;
	case PAPER:
		return "Paper";
		break;
	case SCISSORS:
		return "Scissors";
		break;
	case LIZARD:
		return "Lizard";
		break;
	case SPOCK:
		return "Spock";
		break;

	default:
		break;
	}
	return "";
}