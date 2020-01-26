// --- Messages module ---
// This module responsible for decoding and encoding messages that will be sent by the
// server/client.

#define _CRT_SECURE_NO_WARNINGS

#include "Messages.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "AllocateAndFree.h"


/* Decoding recieved message
input: char* to message 
output: array of char* of the parsed message
*/
char** DecodeMessage(char *message)
{
	char buffer[MAXIMUM_MESSAGE_LENGTH];
	char** parsed_message = AllocateArrayOfStrings(MAXIMUM_MESSAGE_ARGS, MAXIMUM_MESSAGE_LENGTH);
	char current_char;
	int current_char_index = 0, current_buffer_index = 0, parsed_message_index = 0;
	int number_of_parameters = 0;
	do {
		current_char = message[current_char_index];
		if (current_char != ':' && current_char != ';') {
			buffer[current_buffer_index] = current_char;
			current_buffer_index++;
			current_char_index++;
			continue;
		}
		if (current_char == ';')
			number_of_parameters++;

		buffer[current_buffer_index] = '\0';
		strcpy(parsed_message[parsed_message_index], buffer);
		current_buffer_index = 0;
		current_char_index++;
		parsed_message_index++;

	} while (current_char != '\0');
	strcpy(parsed_message[parsed_message_index], buffer);
	parsed_message_index++;
	return parsed_message;
}

/*
Encoding message into format: <message_type>:<param1>;<param2>;...>
input: string message type, pointer to char array of parameters, number of parameters
output:char* to message string ready to send

*/
char* EncodeMessage(char* message_type, char* parameters[], int number_of_parameters)
{
	char *encoded_message = AllocateString(MAXIMUM_MESSAGE_LENGTH);
	if ( encoded_message== NULL)
		return NULL;
	strcpy(encoded_message, "");
	strcat(encoded_message, message_type);
	strcat(encoded_message, ":");
	int parameters_index = 0;
	for (parameters_index = 0; parameters_index < number_of_parameters; parameters_index++) {
		strcat(encoded_message, parameters[parameters_index]);
		strcat(encoded_message, ";");
	}
	int encoded_message_length = strlen(encoded_message);
	encoded_message[encoded_message_length - 1] = '\0';

	return encoded_message;
}

/*
Basic conversion of messageType to the corresponding string
*/
char* ConvertEnumToString(MessageType message)
{
	switch (message)
	{
	case(SERVER_MAIN_MENU):
		return "SERVER_MAIN_MENU";
		break;
	case(SERVER_APPROVED):
		return "SERVER_APPROVED";
		break;
	case(SERVER_DENIED):
		return "SERVER_DENIED";
		break;
	case(SERVER_INVITE):
		return "SERVER_INVITE";
		break;
	case(SERVER_PLAYER_MOVE_REQUEST):
		return "SERVER_PLAYER_MOVE_REQUEST";
		break;
	case(SERVER_GAME_RESULT):
		return "SERVER_GAME_RESULT";
		break;
	case(SERVER_GAME_OVER_MENU):
		return "SERVER_GAME_OVER_MENU";
		break;
	case(SERVER_OPPONENT_QUIT):
		return "SERVER_OPPONENT_QUIT";
		break;
	case(SERVER_NO_OPPONENT):
		return "SERVER_NO_OPPONENT";
		break;
	case(SERVER_LEADERBOARD):
		return "SERVER_LEADERBOARD";
		break;
	case(SERVER_LEADERBOARD_MENU):
		return "SERVER_LEADERBOARD_MENU";
		break;
	case(CLIENT_REQUEST):
		return "CLIENT_REQUEST";
		break;
	case(CLIENT_MAIN_MENU):
		return "CLIENT_MAIN_MENU";
		break;
	case(CLIENT_CPU):
		return "CLIENT_CPU";
		break;
	case(CLINET_VERSUS):
		return "CLINET_VERSUS";
		break;
	case(CLIENT_LEADEROARD):
		return "CLIENT_LEADEROARD";
		break;
	case(CLIENT_PLAYER_MOVE):
		return "CLIENT_PLAYER_MOVE";
		break;
	case(CLIENT_REPLAY):
		return "CLIENT_REPLAY";
		break;
	case(CLIENT_REFRESH):
		return "CLIENT_REFRESH";
		break;
	case(CLIENT_DISCONNECT):
		return "CLIENT_DISCONNECT";
		break;

	
	default:
		printf("Invalid message type!\n");
		break;
	}
}
