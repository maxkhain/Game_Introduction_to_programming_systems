#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXIMUM_MESSAGE_LENGTH 200
#define MAXIMUM_MESSAGE_ARGS 10

typedef enum {
	CHOOSE_MOVE_MENU,
	RETRY_CONNECTION_MENU,
	MAIN_MENU,
	GAME_OVER_MENU,
	LEADER_BOARD_MENU


}menuType;

int PrintToScreen(menuType menu_for_print);