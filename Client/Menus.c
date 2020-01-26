#define _CRT_SECURE_NO_WARNINGS
#include "Menus.h"


//this function print the corresponding menu and waits for user too select the response
//retured -1 if no response is needed
int PrintToScreen(menuType menu_for_print)
{
	int response;
	printf("Choose what to do next:\n");
	switch (menu_for_print)
	{
	case CHOOSE_MOVE_MENU:
		printf("1. Rock \n2. Paper \n3. Scissors \n4. Lizard \n5. Spock \n");
	break;
	case RETRY_CONNECTION_MENU:
		printf("1. Try to reconnect\n2. Exit \n");
	break;
	case MAIN_MENU:
		printf("1. Play against another client \n2. Play against the server \n3. View the leaderboard \n4. Quit\n");
	break;
	case GAME_OVER_MENU:
		printf("1. Play again \n2. Return to the main menu \n");
	break;
	case LEADER_BOARD_MENU:
		printf("1. Refresh leaderboard \n2. Return to the main menu \n");
	break;
	default:
		break;
	}
	scanf("%d", &response);
	return response;
}