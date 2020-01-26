#pragma once

typedef enum
{
	SERVER_MAIN_MENU,
	SERVER_APPROVED,
	SERVER_DENIED,
	SERVER_INVITE,
	SERVER_PLAYER_MOVE_REQUEST,
	SERVER_GAME_RESULT,
	SERVER_GAME_OVER_MENU,
	SERVER_OPPONENT_QUIT,
	SERVER_NO_OPPONENT,
	SERVER_LEADERBOARD,
	SERVER_LEADERBOARD_MENU,
	CLIENT_REQUEST,
	CLIENT_MAIN_MENU,
	CLIENT_CPU,
	CLINET_VERSUS,
	CLIENT_LEADEROARD,
	CLIENT_PLAYER_MOVE,
	CLIENT_REPLAY,
	CLIENT_REFRESH,
	CLIENT_DISCONNECT
}MessageType;




#define MAXIMUM_MESSAGE_LENGTH 200
#define MAXIMUM_MESSAGE_ARGS 10

char** DecodeMessage(char *message);
char* EncodeMessage(char* message_type, char* parameters[], int number_of_parameters);
char* ConvertEnumToString(MessageType message);
