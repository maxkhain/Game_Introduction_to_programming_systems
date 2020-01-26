//-- PrintError module-- -
// This module responsible for all the error messages.

#include "PrintError.h"
#include <stdio.h>

void PrintError(ErrorCodes error_name, long error_code, int line_number, char* file_name) {
	printf("An error has occurred:\n");
	printf("File: %s\n", file_name);
	printf("Line: %d\n", line_number);
	printf("Details: ");
	switch (error_name) {
	case(OPEN_FILE_ERROR):
		printf("fatal error : fopen has failed!\n");
	case(WSA_STARTUP_ERROR):
		printf("fatal error : WSAStartup() has failed with error code:%ld !\n", error_code);
	case (WSA_CLEANUP_ERROR):
		printf("fatal error : WSACleanup() has failed with error code:%ld !\n", error_code);
	case (SOCKET_CREATION_ERROR):
		printf("fatal error : socket() has failed with error code:%ld !\n", error_code);
	case (CLOSING_SOCKER_ERROR):
		printf("fatal error : closesocket() has failed with error code:%ld !\n", error_code);
	case(SET_ADDRESS_ERROR):
		printf("fatal error : the given address string cannot be converted into an ip address.\n");
	case (BIND_ERROR):
		printf("fatal error : bind() has failed with error code:%ld !\n", error_code);
	case (LISTENING_ERROR):
		printf("fatal error : listen() has failed with error code:%ld !\n", error_code);
	case (PLAYER_CONNECTION_ERROR):
		printf("fatal error : accept() has failed with error code:%ld !\n", error_code);
	case (SEND_ERROR):
		printf("fatal error : send() has failed with error code : %ld !\n", error_code);
	case (INVALID_MESSAGE_BUFFER):
		printf("fatal error : message buffer is invalid!\n");
	case (MALLOC_FAILED):
		printf("fatal error : malloc() has failed !\n");

	case (RECV_ERROR):
		printf("fatal error : recv() has failed with error code : %ld !\n", error_code);
	case (MUTEX_CREATION_FAIL):
		printf("fatal error : mutex creation has failed with error code : %ld !\n", error_code);
	case (USERNAME_ALREADY_EXISTS):
		printf("Username already exist! request declined");
	case (PLAYER_DISCONNECTED):
		printf("Player disconnected. Ending communication\n");
	case(SEMAPHORE_CREATION_ERROR):
		printf("Semaphore creation falied. Ending communication with error code : %ld !\n", error_code);
	case (WAIT_FOR_SINGLE_OBJECT_ERROR):
		printf("WaitForSingleObject falied. Ending communication with error code : %ld !\n", error_code);
	case (REALSE_MUTEX_ERROR):
		printf("RealeseMutex() falied. Ending communication with error code : %ld !\n", error_code);
	case (REALSE_SEMAPHORE_ERROR):
		printf("RealeseSemaphore() falied. Ending communication with error code : %ld !\n", error_code);
	case (THREAD_CREATION_FAIL):
		printf("Thread creation falied falied. Ending communication with error code : %ld !\n", error_code);
	default:
		break;
	}
}