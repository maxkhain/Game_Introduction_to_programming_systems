#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Game.h"

/*This is the game function
input: player 1 move, player 2 move
output: 0-tie, 1-player 1 won, 2-player 2 won
logic: Game rules

*/
char* PlayGame(char* move_player_one, char* move_player_two)
{
	char* winner=(char*)malloc(sizeof(char)*2); //defalut player 2 wins

	strcpy(winner, "2");
	if (strncmp(move_player_one, move_player_two, sizeof(move_player_one)) == 0)//tie	
		strcpy(winner, "0");
	else {
		if (strncmp(move_player_one, "ROCK", sizeof(move_player_one)) == 0) {
			if (strncmp(move_player_two, "SCISSORS", sizeof(move_player_two)) == 0|| strncmp(move_player_two, "LIZARD", sizeof(move_player_two))== 0)
				strcpy(winner, "1");
		}
			
		if (strncmp(move_player_one, "PAPER", sizeof(move_player_one)) == 0) {
			if (strncmp(move_player_two, "ROCK", sizeof(move_player_two)) == 0 || strncmp(move_player_two, "SPOCK", sizeof(move_player_two)) == 0)
				strcpy(winner, "1");
		}
		if (strncmp(move_player_one, "SCISSORS", sizeof(move_player_one)) == 0) {
			if (strncmp(move_player_two, "PAPER", sizeof(move_player_two)) == 0 || strncmp(move_player_two, "LIZARD", sizeof(move_player_two)) == 0)
				strcpy(winner, "1");
		}
		if (strncmp(move_player_one, "LIZARD", sizeof(move_player_one)) == 0) {
			if (strncmp(move_player_two, "SPOCK", sizeof(move_player_two)) == 0 || strncmp(move_player_two, "PAPER", sizeof(move_player_two)) == 0)
				strcpy(winner, "1");
		}
		if (strncmp(move_player_one, "SPOCK", sizeof(move_player_one)) == 0) {
			if (strncmp(move_player_two, "ROCK", sizeof(move_player_two)) == 0 || strncmp(move_player_two, "SCISSORS", sizeof(move_player_two)) == 0)
				strcpy(winner, "1");
		}	
		
	}
		
	return winner;
}

/*
Basic convert enum to corresponding string
*/
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

