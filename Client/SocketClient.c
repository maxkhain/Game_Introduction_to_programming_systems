/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 

This is the main module for the client
this modules main function is called from server_main
The client has 1 thread for recieveing called RecvDataThread
The client has 1 thread for sending called SendDataThreads
For each received message the client calles func ActionAndBuildResponse to decide what is the response message 
 */
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include "PrintError.h"
#include "AllocateAndFree.h"
#include "Messages.h"
#include "SocketSendRecvTools.h"
#include "Menus.h"
#include "Game.h"

#define SEND_STR_SIZE 256
#define NAME_MAX_LENGTH 20
#define MAX_NUM_OF_ARGUMENTS 5
#define MAX_SIZE_OF_ARGUMENTS 30
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
//global params
SOCKET m_socket;
char SendStr[SEND_STR_SIZE];
HANDLE mutex;
HANDLE flag_new_message;
HANDLE hThread[2];
HANDLE RecvThread;
char* client_name;
char *AcceptedStr = NULL;
char *TempAcceptedStr = NULL;
BOOL message_to_send = FALSE;
BOOL is_running = TRUE;
BOOL time_out = FALSE, is_stop=FALSE;
char* ip;
int port;
BOOL limit_wait = TRUE;
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
char* ActionAndBuildResponse(char** decoded_message);
void FreeParams(char** parameters);
char* ChosenMove(int move);
static DWORD Recv(void);

//Reading data coming from the server
static DWORD RecvDataThread(LPVOID count)
{
	TransferResult_t RecvRes;
	DWORD wait_res, release_res;
	BOOL res_success;
	LONG previous_count;
	int thread_num = (int)count;
	
	Sleep(200);

	while (is_running == TRUE)
	{
		wait_res = WaitForSingleObject(mutex, INFINITE);
		if (wait_res != WAIT_OBJECT_0) {
			PrintError(WAIT_FOR_SINGLE_OBJECT_ERROR, wait_res, __LINE__, __FILE__);
			return;
		}

		RecvThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)Recv,
			NULL,
			0,
			NULL
		);
		if (RecvThread == NULL)
		{
			PrintError(SEMAPHORE_CREATION_ERROR, 0, __LINE__, __FILE__);
			return;
		}
		if(limit_wait)
			wait_res = WaitForSingleObject(RecvThread, 15000);
		else
			wait_res = WaitForSingleObject(RecvThread, INFINITE);
		
				

		if (wait_res != WAIT_TIMEOUT)
		{
			strcpy(AcceptedStr, TempAcceptedStr);
		}
		else
		{
			TerminateThread(RecvThread, 0x555);
			printf("Connection to server on %s:%d has been lost\n", ip, port);
			AcceptedStr = (char*)malloc(sizeof(char)*SEND_STR_SIZE);
			if (AcceptedStr == NULL) {
				PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
				return;
			}
			strcpy(AcceptedStr, "SERVER_DENIED");
		}
		release_res = ReleaseSemaphore(
			flag_new_message,
			1, 		/* new message in buffer */
			&previous_count);
		if (release_res == FALSE) {
			PrintError(REALSE_SEMAPHORE_ERROR, release_res, __LINE__, __FILE__);
			return;
		}

		release_res = ReleaseMutex(mutex);
		if (release_res == FALSE) {
			PrintError(REALSE_MUTEX_ERROR, release_res, __LINE__, __FILE__);
			return;
		}
		Sleep(100);
	}
	free(AcceptedStr);


	return 0;
}

static DWORD Recv(void)
{
	TransferResult_t RecvRes;
	TempAcceptedStr = NULL;
	RecvRes = ReceiveString(&TempAcceptedStr, m_socket);
	if (RecvRes == TRNS_FAILED)
	{
		printf("Service socket error while reading, closing thread.\n");
		closesocket(m_socket);
		return 1;
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("Connection closed while reading, closing thread.\n");
		closesocket(m_socket);
		return 1;
	}
	
	return;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Sending data to the server
static DWORD SendDataThread(void)
{
	char SendStr[256];
	TransferResult_t SendRes;
	DWORD wait_res, release_res;
	BOOL close_check = TRUE;
	char **decoded_message = NULL;
	
	strcpy(SendStr, "CLIENT_REQUEST:");
	strcat(SendStr, client_name);
	SendRes = SendString(SendStr, m_socket);

	char* tmp_string;

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(m_socket);
		return 1;
	}

	while (is_running)
	{
		wait_res = WaitForSingleObject(flag_new_message, INFINITE);//check if new message in buffer
		if (wait_res != WAIT_OBJECT_0) {
			PrintError(WAIT_FOR_SINGLE_OBJECT_ERROR, wait_res, __LINE__, __FILE__);
			return;
		}

		wait_res = WaitForSingleObject(mutex, INFINITE);
		if (wait_res != WAIT_OBJECT_0) {
			PrintError(WAIT_FOR_SINGLE_OBJECT_ERROR, wait_res, __LINE__, __FILE__);
			return;
		}
		if (AcceptedStr != NULL) {
			decoded_message = DecodeMessage(AcceptedStr);
			tmp_string = ActionAndBuildResponse(decoded_message);
			strcpy(SendStr, tmp_string);
			time_out = FALSE;
			if (message_to_send == TRUE)			{	

				SendRes = SendString(SendStr, m_socket);
				if (is_stop)
					is_running = FALSE;
			}
			free(tmp_string);			
		}

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		release_res = ReleaseMutex(mutex);
		if (release_res == FALSE) {
			PrintError(REALSE_MUTEX_ERROR, release_res, __LINE__, __FILE__);
			return;
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainClient(char* string_ip, int server_port,char* name)
{
	SOCKADDR_IN clientService;	
	ip = string_ip;
	port = server_port;
	BOOL is_connected = FALSE;

	AcceptedStr = (char*)malloc(sizeof(char)*MAXIMUM_MESSAGE_LENGTH);
	if (AcceptedStr == NULL) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return;
	}

	TempAcceptedStr = (char*)malloc(sizeof(char)*MAXIMUM_MESSAGE_LENGTH);
	if (TempAcceptedStr == NULL) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return;
	}

	client_name = (char*)malloc(sizeof(char)*NAME_MAX_LENGTH);
	if (client_name == NULL) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return;
	}
	strcpy(client_name, name);

    // Initialize Winsock.
    WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.
	printf("Hello %s\nWe will connect you shortly...\n \n \n", client_name);
	

	//Call WSAStartup and check for errors.
    int iResult = WSAStartup( MAKEWORD(2, 2), &wsaData );
    if ( iResult != NO_ERROR )
        printf("Error at WSAStartup()\n");

	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.
	
	// Create a socket.
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	// Check for errors to ensure that the socket is a valid socket.
    if ( m_socket == INVALID_SOCKET ) {
        printf( "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return;
    }
	/*
	 The parameters passed to the socket function can be changed for different implementations. 
	 Error detection is a key part of successful networking code. 
	 If the socket call fails, it returns INVALID_SOCKET. 
	 The if statement in the previous code is used to catch any errors that may have occurred while creating 
	 the socket. WSAGetLastError returns an error number associated with the last error that occurred.
	 */


	//For a client to communicate on a network, it must connect to a server.
    // Connect to a server.
	while (is_connected==FALSE)
	{
		//Create a sockaddr_in object clientService and set  values.
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(string_ip); //Setting the IP address to connect to
		clientService.sin_port = htons(server_port); //Setting the port to connect to.
		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR)
			is_connected = TRUE;	
		else {
			printf("Failed connecting to server on %s:%d\n", string_ip, server_port);
			int user_choice = PrintToScreen(RETRY_CONNECTION_MENU);
			if (user_choice == 2)
			{
				goto Exit;
			}
		}
	}
	printf("Connected to server on %s:%d \n", string_ip,server_port);

    // Send and receive data.
	/*
		In this code, two integers are used to keep track of the number of bytes that are sent and received. 
		The send and recv functions both return an integer value of the number of bytes sent or received, 
		respectively, or an error. Each function also takes the same parameters: 
		the active socket, a char buffer, the number of bytes to send or receive, and any flags to use.

	*/	

	mutex = CreateMutex(
		NULL,               /* default security attributes */
		FALSE,	            /* don't lock mutex immediately */
		NULL);              /* un-named */

	if (mutex == NULL) {		
		PrintError(MUTEX_CREATION_FAIL, 0, __LINE__, __FILE__);
		return;
	}
	flag_new_message = CreateSemaphore(
		NULL,	            /* Default security attributes */
		0,					/* Initial Count - no message recieved */
		1,					/* Maximum Count */
		NULL);              /* un-named */

	if (flag_new_message == NULL) {
		PrintError(SEMAPHORE_CREATION_ERROR, 0, __LINE__, __FILE__);
		return;
	}
	hThread[0]=CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) SendDataThread,
		NULL,
		0,
		NULL
	);
	hThread[1]=CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) RecvDataThread,
		NULL,
		0,
		NULL
	);	


	WaitForMultipleObjects(2,hThread,FALSE,INFINITE);
	
	Sleep(500);
	TerminateThread(hThread[0],0x555);
	TerminateThread(hThread[1],0x555);
	TerminateThread(RecvThread,0x555);
	

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	free(AcceptedStr);
	free(TempAcceptedStr);
	free(client_name);
	closesocket(m_socket);
Exit:
	WSACleanup();
    
	return;
}
/*This function is responsible for the decision that client does after resieving a message from the server
input: char** of the decoded message
output: return the message to send back to server
Logic: first finding the message type, for each message type printing corresponding message to the CLI and let the user choose the 
corresponding response
*/
char* ActionAndBuildResponse(char** decoded_message)
{
	char message_type_in[MAX_SIZE_OF_ARGUMENTS], message_type_out[MAX_SIZE_OF_ARGUMENTS];
	int number_of_parameters=0;
	char** parameters;
	char*tmp_string=NULL;
	char* return_string=NULL;
	int client_input;

	parameters = AllocateArrayOfStrings( MAX_NUM_OF_ARGUMENTS, MAX_SIZE_OF_ARGUMENTS);

	return_string = (char*)malloc(sizeof(char) * MAXIMUM_MESSAGE_LENGTH);
	if (return_string == NULL) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return;
	}
	strcpy(message_type_in, decoded_message[0]);

	if (strncmp(message_type_in, "SERVER_MAIN_MENU", sizeof("SERVER_MAIN_MENU")) == 0 )
	{
		limit_wait = TRUE;
		client_input = PrintToScreen(MAIN_MENU);
		if (client_input==1)
		{
			strcpy(message_type_out, "CLINET_VERSUS");
			printf("Waiting for opponents...\n");
			limit_wait = FALSE;
		}
		else if (client_input==2)
		{
			strcpy(message_type_out, "CLIENT_CPU");
		}
		else if (client_input==3)
		{
			strcpy(message_type_out, "CLIENT_LEADEROARD");
		}
		else if (client_input==4)
		{
			strcpy(message_type_out, "CLIENT_DISCONNECT");
		}
		else
			strcpy(message_type_out, "CLIENT_MAIN_MENU");
		number_of_parameters = 0;
		message_to_send = TRUE;
	}
	else if (strncmp(message_type_in, "SERVER_APPROVED", sizeof("SERVER_APPROVED")) == 0 )
	{
		limit_wait = TRUE;
		printf("Client set up successfully! \n");
		client_input = PrintToScreen(MAIN_MENU);
		if (client_input==1)
		{
			strcpy(message_type_out, "CLINET_VERSUS");
			limit_wait = FALSE;
			printf("Waiting for opponents...\n");
		}
		else if (client_input==2)
		{
			strcpy(message_type_out, "CLIENT_CPU");
		}
		else if (client_input==3)
		{
			strcpy(message_type_out, "CLIENT_LEADEROARD");
		}
		else if (client_input==4)
		{
			strcpy(message_type_out, "CLIENT_DISCONNECT");
		}
		else 
			strcpy(message_type_out, "CLIENT_MAIN_MENU");
		number_of_parameters = 0;
		message_to_send = TRUE;
	}
	else if (strncmp(message_type_in, "SERVER_DENIED", sizeof("SERVER_DENIED")) == 0 )
	{
		limit_wait = FALSE;
		printf("Server on %s:%d denied the connection request\n", ip, port);
		client_input = PrintToScreen(RETRY_CONNECTION_MENU);		
		number_of_parameters = 0;
		if (client_input==1)
		{
			strcpy(message_type_out, "CLIENT_REQUEST");
		}
		else if (client_input==2)
		{
			strcpy(message_type_out, "CLIENT_DISCONNECT");
		}
		else 
			strcpy(message_type_out, "CLIENT_DISCONNECT");
		message_to_send = TRUE;
	}
	else if (strncmp(message_type_in, "SERVER_INVITE", sizeof("SERVER_INVITE")) == 0)
	{
		printf("Fount opponenet\n");
		message_to_send = FALSE;
	}
	else if (strncmp(message_type_in, "SERVER_PLAYER_MOVE_REQUEST", sizeof("SERVER_PLAYER_MOVE_REQUEST")) == 0 )
	{
		limit_wait = FALSE;
		client_input = PrintToScreen(CHOOSE_MOVE_MENU);
		char* client_move = ChosenMove(client_input);
		strcpy(parameters[0], client_move);		
		number_of_parameters = 1;
		strcpy(message_type_out, "CLIENT_PLAYER_MOVE");
		message_to_send = TRUE;
		printf("Waiting for the opponent to pick a move...\n");
	}
	else if (strncmp(message_type_in, "SERVER_GAME_RESULT", sizeof("SERVER_GAME_RESULT")) == 0 )
	{
		limit_wait = TRUE;
		PlayAndPrintGame(decoded_message[1], decoded_message[2], decoded_message[3], decoded_message[4]);
		message_to_send = FALSE;
		goto dont_send_message;
	}
	else if (strncmp(message_type_in, "SERVER_GAME_OVER_MENU", sizeof("SERVER_GAME_OVER_MENU")) == 0)
	{
		limit_wait = TRUE;
		client_input = PrintToScreen(GAME_OVER_MENU);
		if (client_input==1)
		{
			strcpy(message_type_out, "CLIENT_REPLAY");
			limit_wait = FALSE;
			printf("Waiting for the opponent...\n");
		}
		else
		{
			strcpy(message_type_out, "CLIENT_MAIN_MENU");
		}		
		number_of_parameters = 0;
		message_to_send = TRUE;
	}
	else if (strncmp(message_type_in, "SERVER_OPPONENT_QUIT", sizeof("SERVER_OPPONENT_QUIT")) == 0 )
	{
		limit_wait = TRUE;
		printf("Opponent left the game\n");
		message_to_send = FALSE;
	}
	else if (strncmp(message_type_in, "SERVER_NO_OPPONENT", sizeof("SERVER_NO_OPPONENT")) == 0 )
	{
		limit_wait = TRUE;
		printf("Could not find opponent\n");
		client_input = PrintToScreen(MAIN_MENU);
		if (client_input==1)
		{
			strcpy(message_type_out, "CLINET_VERSUS");
			limit_wait = FALSE;
		}
		else if (client_input==2)
		{
			strcpy(message_type_out, "CLIENT_CPU");
		}
		else if (client_input==3)
		{
			strcpy(message_type_out, "CLIENT_LEADEROARD");
		}
		else if (client_input==4)
		{
			strcpy(message_type_out, "CLIENT_DISCONNECT");
		}
		else
		{
			strcpy(message_type_out, "CLIENT_MAIN_MENU");

		}
		number_of_parameters = 0;
		message_to_send = TRUE;
	}
	else if (strncmp(message_type_in, "SERVER_LEADERBOARD", sizeof("SERVER_LEADERBOARD")) == 0)
	{
		limit_wait = TRUE;
		//todo
	}
	else if (strncmp(message_type_in, "SERVER_LEADERBOARD_MENU", sizeof("SERVER_LEADERBOARD_MENU")) == 0)
	{
		limit_wait = TRUE;
		client_input = PrintToScreen(LEADER_BOARD_MENU);
		if (client_input == 1)
		{
			strcpy(message_type_out, "CLIENT_REFRESH");
		}
		else 
		{
			strcpy(message_type_out, "CLIENT_MAIN_MENU");
		}		
		number_of_parameters = 0;
		message_to_send = TRUE;
	}
	else {
		goto not_found_message;
		message_to_send = FALSE;
	}


	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	strcpy(return_string, tmp_string);
	if (strncmp(message_type_out,"CLIENT_DISCONNECT",sizeof("CLIENT_DISCONNECT")==0))
	{		
		is_stop = TRUE;
	}


not_found_message:
dont_send_message:
	
	FreeParams(parameters);
	free(tmp_string);
	return return_string; 
}
//freeing char** pointer of arguments
void FreeParams(char** parameters)
{
	int i;	
	for (i = 0; i < MAX_NUM_OF_ARGUMENTS; i++)
	{
		free((parameters)[i]);
	}
	free((parameters));
}

/*This function translates a pc/user input in number 1-5 to the string corresponding to move chosen
input: int move number
output: char* string of the move chosen
*/
char* ChosenMove(int move)
{
	switch (move)
	{
	case 1:
		return "ROCK";
		break;
	case 2:
		return "PAPER";
		break;
	case 3:
		return "SCISSORS";
		break;
	case 4:
		return "LIZARD";
		break;
	case 5:
		return "SPOCK";
		break;

	}
	return "SPOCK";
}