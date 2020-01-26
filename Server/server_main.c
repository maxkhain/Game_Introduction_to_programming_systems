#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include "Game.h"
#include "PrintError.h"
#include "Messages.h"
#include "AllocateAndFree.h"
#include "SocketServer.h"

/*
Main module for the client
Checking the input arguments and calling MainServer that creates the server 
Submitted by Max Khain 311409205

*/

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Not enough arguments");
		return -1;
	}
	int port_number = strtol(argv[1], NULL, 10);
	if (0 == port_number) {
		printf("Port cant be 0");
		return -1;
	}	
	MainServer(port_number);
}