#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include "Game.h"
#include "SocketClient.h"
#include "Game.h"

/*
Main module for the client
Checking the input arguments and calling Main client that creates the client and connects to server
Submitted by Max Khain 311409205

*/


#define MAX_SIZE_IP 256
#define MAX_SIZE_NAME 20


int main(int argc, char* argv[])
{	
	if (argc != 4) {
		printf("Not enough arguments");
		return -1;
	}
	char IP[MAX_SIZE_IP], name[MAX_SIZE_NAME];
	int port_number = strtol(argv[2], NULL, 10);
	if (0 == port_number) {
		printf("Port cant be 0");
		return -1;
	}	
	strcpy(IP, argv[1]);
	strcpy(name, argv[3]);

	MainClient(IP, port_number, name);
	printf("End of Client");
}