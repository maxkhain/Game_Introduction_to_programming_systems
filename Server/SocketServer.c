/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
/* 
This is the main module for the server
this modules main function is called from server_main
For every client connecting the server is creating a thread whitch is resposible for recv and send called ServiceThread
For each received message the server calles func ActionAndBuildResponse to decide what is the response message
 */
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "Game.h"
#include "PrintError.h"
#include "SocketServer.h"
#include "SocketSendRecvTools.h"
#include "AllocateAndFree.h"

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
typedef struct MyStruct{
	SOCKET socket;
	int number_of_thread;
}ThreadInput;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE ThreadHandles[NUM_OF_WORKER_THREADS];
ThreadInput ThreadInputs[NUM_OF_WORKER_THREADS];
Player players[MAX_NUM_PLAYERS];
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
void freeDoublePointer(char*** array_of_strings);
static int FindFirstUnusedThreadSlot();
static void CleanupWorkerThreads();
static DWORD ServiceThread(ThreadInput *input );
void ActionAndBuildResponse(char** decoded_message,int thread_num, SOCKET t_socket);
void FreeParams(char** parameters);
int FindOpponent(int my_number);
char* ChosenMove(int move);
void InitilaizePlayers();
void FreePlayers();
void ActionReplay(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
void ActionPlayerMove(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
void ActionVersus(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
void ActionCPU(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
void ActionMainMenu(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
void ActionRequest(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket);
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void MainServer(int server_port)
{
	int Ind;
	int Loop;
	SOCKET MainSocket = INVALID_SOCKET;
	unsigned long Address;
	SOCKADDR_IN service;
	int bindRes;
	int ListenRes;

	InitilaizePlayers();

	// Initialize Winsock.
    WSADATA wsaData;
    int StartupRes = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );	           

    if ( StartupRes != NO_ERROR )
	{
        printf( "error %ld at WSAStartup( ), ending program.\n", WSAGetLastError() );
		// Tell the user that we could not find a usable WinSock DLL.                                  
		return;
	}
 
    /* The WinSock DLL is acceptable. Proceed. */

    // Create a socket.    
    MainSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( MainSocket == INVALID_SOCKET ) 
	{
        printf( "Error at socket( ): %ld\n", WSAGetLastError( ) );
		goto server_cleanup_1;
    }

    // Bind the socket.
	/*
		For a server to accept client connections, it must be bound to a network address within the system. 
		The following code demonstrates how to bind a socket that has already been created to an IP address 
		and port.
		Client applications use the IP address and port to connect to the host network.
		The sockaddr structure holds information regarding the address family, IP address, and port number. 
		sockaddr_in is a subset of sockaddr and is used for IP version 4 applications.
   */
	// Create a sockaddr_in object and set its values.
	// Declare variables

	Address = inet_addr( SERVER_ADDRESS_STR );
	if ( Address == INADDR_NONE )
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
				SERVER_ADDRESS_STR );
		goto server_cleanup_2;
	}

    service.sin_family = AF_INET;
    service.sin_addr.s_addr = Address;
    service.sin_port = htons( server_port ); //The htons function converts a u_short from host to TCP/IP network byte order 
	                                   //( which is big-endian ).
	/*
		The three lines following the declaration of sockaddr_in service are used to set up 
		the sockaddr structure: 
		AF_INET is the Internet address family. 
		"127.0.0.1" is the local IP address to which the socket will be bound. 
	    port is the port number to which the socket will be bound.
	*/

	// Call the bind function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
    bindRes = bind( MainSocket, ( SOCKADDR* ) &service, sizeof( service ) );
	if ( bindRes == SOCKET_ERROR ) 
	{
        printf( "bind( ) failed with error %ld. Ending program\n", WSAGetLastError( ) );
		goto server_cleanup_2;
	}
    
    // Listen on the Socket.
	ListenRes = listen( MainSocket, SOMAXCONN );
    if ( ListenRes == SOCKET_ERROR ) 
	{
        printf( "Failed listening on socket, error %ld.\n", WSAGetLastError() );
		goto server_cleanup_2;
	}

	// Initialize all thread handles to NULL, to mark that they have not been initialized
	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
		ThreadHandles[Ind] = NULL;

    printf( "Waiting for a client to connect...\n" );
    
	for ( Loop = 0; Loop < MAX_LOOPS; Loop++ )
	{
		SOCKET AcceptSocket = accept( MainSocket, NULL, NULL );
		if ( AcceptSocket == INVALID_SOCKET )
		{
			printf( "Accepting connection with client failed, error %ld\n", WSAGetLastError() ) ; 
			goto server_cleanup_3;
		}

        printf( "Client Connected.\n" );

		Ind = FindFirstUnusedThreadSlot();

		if ( Ind == NUM_OF_WORKER_THREADS ) //no slot is available
		{ 
			printf( "No slots available for client, dropping the connection.\n" );
			closesocket( AcceptSocket ); //Closing the socket, dropping the connection.
		} 
		else 	
		{
			(ThreadInputs[Ind].socket) = AcceptSocket; // shallow copy: don't close 
											  // AcceptSocket, instead close 
											  // ThreadInputs[Ind] when the
											  // time comes.
			ThreadInputs[Ind].number_of_thread = Ind+1; // 0 is saved for pc
			ThreadHandles[Ind] = CreateThread(
				NULL,
				0,
				( LPTHREAD_START_ROUTINE ) ServiceThread,
				&( ThreadInputs[Ind] ),
				0,
				NULL
			);
		}
    } // for ( Loop = 0; Loop < MAX_LOOPS; Loop++ )

server_cleanup_3:

	CleanupWorkerThreads();

server_cleanup_2:
	if ( closesocket( MainSocket ) == SOCKET_ERROR )
		printf("Failed to close MainSocket, error %ld. Ending program\n", WSAGetLastError() ); 

server_cleanup_1:
	if ( WSACleanup() == SOCKET_ERROR )		
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError() );
	FreePlayers();
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static int FindFirstUnusedThreadSlot()
{ 
	int Ind;

	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
	{
		if ( ThreadHandles[Ind] == NULL )
			break;
		else
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject( ThreadHandles[Ind], 0 ); 
				
			if ( Res == WAIT_OBJECT_0 ) // this thread finished running
			{				
				CloseHandle( ThreadHandles[Ind] );
				ThreadHandles[Ind] = NULL;
				break;
			}
		}
	}

	return Ind;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

static void CleanupWorkerThreads()
{
	int Ind; 

	for ( Ind = 0; Ind < NUM_OF_WORKER_THREADS; Ind++ )
	{
		if ( ThreadHandles[Ind] != NULL )
		{
			// poll to check if thread finished running:
			DWORD Res = WaitForSingleObject( ThreadHandles[Ind], INFINITE ); 
				
			if ( Res == WAIT_OBJECT_0 ) 
			{
				closesocket( ThreadInputs[Ind].socket );
				CloseHandle( ThreadHandles[Ind] );
				ThreadHandles[Ind] = NULL;
				break;
			}
			else
			{
				printf( "Waiting for thread failed. Ending program\n" );
				return;
			}
		}
	}
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//Service thread is the thread that opens for each successful client connection and "talks" to the client.
static DWORD ServiceThread(ThreadInput *input )
{

	BOOL Done = FALSE;
	
	TransferResult_t RecvRes;
	SOCKET t_socket = input->socket;
	int player_number = input->number_of_thread;

		
	while ( !Done ) 
	{		
		char *AcceptedStr = NULL;
		char **decoded_message = NULL;
		RecvRes = ReceiveString( &AcceptedStr , t_socket );


		if ( RecvRes == TRNS_FAILED )
		{
			printf( "Service socket error while reading, closing thread.\n" );
			closesocket( t_socket );
			return 1;
		}
		else if ( RecvRes == TRNS_DISCONNECTED )
		{
			printf( "Connection closed while reading, closing thread.\n" );
			closesocket( t_socket );
			return 1;
		}
		else
		{
			printf("Got string : %s\n", AcceptedStr);
			decoded_message = DecodeMessage(AcceptedStr);
			ActionAndBuildResponse(decoded_message, player_number, t_socket);			
							
			freeDoublePointer(&decoded_message);			
		}
		free( AcceptedStr );		
	}

	printf("Conversation ended.\n");
	closesocket( t_socket );
	return 0;
}
//free double pointer
void freeDoublePointer(char*** array_of_strings)
{
	for (int array_index = 0; array_index < MAXIMUM_MESSAGE_ARGS; array_index++)
		free((*array_of_strings)[array_index]);
	free(*array_of_strings);
}
/*
This function is responsible for the action of the client(the logic) for all kinds of server messages
input: decoded message string array, int thread number, Socket of the connection
logic: allocating all the string, create message_type_in string for message type and finds the recieved message type to call the correct funcion action
*/
void ActionAndBuildResponse(char** decoded_message, int thread_num, SOCKET t_socket)
{
	char message_type_in[MAX_SIZE_OF_ARGUMENTS], message_type_out[MAX_SIZE_OF_ARGUMENTS];
	int number_of_parameters=0;
	char** parameters = NULL;
	char *tmp_string = NULL;
	TransferResult_t SendRes;
	BOOL is_replay = FALSE;
	tmp_string = (char*)malloc(sizeof(char) * SEND_STR_SIZE);
	if (tmp_string == NULL) {
		PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
		return;
	}
	parameters = AllocateArrayOfStrings(MAX_NUM_OF_ARGUMENTS, MAX_SIZE_OF_ARGUMENTS);
	strcpy(message_type_in, decoded_message[0]);
	strcat(message_type_in, "\0");
	
	if (strncmp(message_type_in, "CLIENT_REQUEST", sizeof("CLIENT_REQUEST")) == 0)
	{	
		//initialize player
		ActionRequest(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);

	}
	else if (strncmp(message_type_in, "CLIENT_MAIN_MENU", sizeof("CLIENT_MAIN_MENU")) == 0)
	{		
		ActionMainMenu(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);
	}
	else if (strncmp(message_type_in, "CLIENT_CPU", sizeof("CLIENT_CPU")) == 0 )
	{
		ActionCPU(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);
	}
	else if (strncmp(message_type_in, "CLINET_VERSUS", sizeof("CLINET_VERSUS")) ==0)
	{
		ActionVersus(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);
	}
	else if (strncmp(message_type_in, "CLIENT_LEADEROARD", sizeof("CLIENT_LEADEROARD")) == 0 )
	{
		number_of_parameters = 0;
		(players[thread_num]).is_busy = TRUE;
		strcpy(message_type_out, "SERVER_MAIN_MENU");
		tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
		SendRes = SendString(tmp_string, t_socket);
		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(t_socket);
			return;
		}
	}
	else if (strncmp(message_type_in, "CLIENT_PLAYER_MOVE", sizeof("SERVER_GAME_RESULT")) == 0 )
	{
		ActionPlayerMove(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);
	}
	else if (strncmp(message_type_in, "CLIENT_REPLAY", sizeof("CLIENT_REPLAY")) == 0 )
	{
		ActionReplay(thread_num, decoded_message, message_type_out, tmp_string, parameters, t_socket);
	}	
	else if (strncmp(message_type_in, "CLIENT_DISCONNECT", sizeof("CLIENT_DISCONNECT")) == 0 )
	{

		printf("Client disconnected closing socket.\n");
		closesocket(ThreadInputs[thread_num-1].socket);
		CloseHandle(ThreadHandles[thread_num-1]);
		ThreadHandles[thread_num-1] = NULL;
		closesocket(t_socket);
	}
	else  
		goto not_found_message;	

not_found_message:
	free(tmp_string);
	FreeParams(parameters);	
}

#pragma region  Actions



/*
Client action on serverRequest
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: Create a message SERVER_APPROVED and send to client
*/
void ActionRequest(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string,char** parameters, SOCKET t_socket)
{
	TransferResult_t SendRes;
	int number_of_parameters;
	strcpy((players[thread_num]).name, decoded_message[1]);
	number_of_parameters = 0;
	strcpy(message_type_out, "SERVER_APPROVED");
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
}

/*
Client action on server main menu
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: Create a message SERVER_MAIN_MENU and send to player
*/
void ActionMainMenu(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket)
{
	TransferResult_t SendRes;
	int number_of_parameters = 0;
	(players[thread_num]).is_busy = TRUE;
	(players[thread_num]).opponent = -1;
	strcpy(message_type_out, "SERVER_MAIN_MENU");
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
}

/*
Client action on server CPU
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: random a move for a pc and send SERVER_PLAYER_MOVE_REQUEST to the player
*/
void ActionCPU(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket)
{
	TransferResult_t SendRes;
	time_t t;
	srand((unsigned)time(&t));
	int number_of_parameters = 0;
	(players[thread_num]).is_busy = TRUE;
	(players[thread_num]).opponent = 0;
	strcpy((players[0]).last_move, ChosenMove(rand() % 4 + 1));
	(players[0]).played_move = TRUE;
	strcpy(message_type_out, "SERVER_PLAYER_MOVE_REQUEST");
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
}

/*
Client action on server versus
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: searching for opponent which is in VS mode by sampling function FindOpponent  every 100ms, after 30 sec timeout and send SERVER_NO_OPPONENT
*/
void ActionVersus(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket)
{
	TransferResult_t SendRes;
	int number_of_parameters = 0;
	int opponent;
	BOOL found_opponent = FALSE;
	(players[thread_num]).is_busy = FALSE;
	strcpy(message_type_out, "SERVER_NO_OPPONENT");
	for (size_t i = 0; i < 300; i++) //30 sec to find opponent
	{
		opponent = FindOpponent(thread_num);
		if (opponent != -1) {
			(players[thread_num]).opponent = opponent;
			strcpy(message_type_out, "SERVER_PLAYER_MOVE_REQUEST");
			found_opponent = TRUE;
			break;
		}
		Sleep(100);
	}
	if(!found_opponent)
		(players[thread_num]).is_busy = TRUE;
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
}

/*
Client action on server request move
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: recieve player move and wait for opponents move by waiting in while loop for opponenet played_move==TRUE
After calculating  the result of the game copy all the arguments and send message to the player with opponent name, moves of each player and the winner.
*/
void ActionPlayerMove(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket)
{
	TransferResult_t SendRes;
	int number_of_parameters = 4;
	char* winner = NULL;
	(players[thread_num]).is_busy = TRUE;

	strcpy((players[thread_num]).last_move, decoded_message[1]);
	(players[thread_num]).played_move = TRUE;
	int opponent_num = (players[thread_num]).opponent;
	if (opponent_num != -1) {
		while (players[opponent_num].played_move != TRUE); //wait for both player to choose a move
		winner = PlayGame((players[thread_num]).last_move, players[opponent_num].last_move); //check game result
		strcpy(parameters[0], players[opponent_num].name);
		strcpy(parameters[1], (players[thread_num]).last_move);
		strcpy(parameters[2], players[opponent_num].last_move);
		strcpy(parameters[3], winner);
		free(winner);
	}	
	
	strcpy(message_type_out, "SERVER_GAME_RESULT");
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
	(players[thread_num]).played_move = FALSE;

	strcpy(message_type_out, "SERVER_GAME_OVER_MENU");
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}
}

/*
Client action on server replay
input: thread number, array of string decoded message, pointer to tmp string, pointer to sendres, pointer to array of string parameters, socket
logic: Waiting 30 sec for the opponenet to accept challange, if not send message SERVER_OPPONENT_QUIT, when opponent returnes to main menu send message SERVER_OPPONENT_QUIT
if no opponenet send SERVER_MAIN_MENU, if there is an opponent which means oppoenet accepted challange ask player for next move and send SERVER_PLAYER_MOVE_REQUEST
*/
void ActionReplay(int thread_num, char** decoded_message, char* message_type_out, char* tmp_string, char** parameters, SOCKET t_socket) 
{
	TransferResult_t SendRes;
	BOOL is_replay = FALSE;
	int number_of_parameters = 0;
	strcpy(message_type_out, "SERVER_OPPONENT_QUIT");
	int opponent = (players[thread_num]).opponent;
	if (opponent == 0) {
		strcpy((players[0]).last_move, ChosenMove(rand() % 4 + 1));
		(players[0]).played_move = TRUE;
		strcpy(message_type_out, "SERVER_PLAYER_MOVE_REQUEST");
		is_replay = TRUE;
	}
	 
	else {
		(players[thread_num]).is_busy = FALSE;
		for (size_t i = 0; i < 300; i++) //30 sec to find opponent
		{
			if (((players[opponent]).is_busy == FALSE && (players[opponent]).opponent == thread_num)) {
				strcpy(message_type_out, "SERVER_PLAYER_MOVE_REQUEST");
				is_replay = TRUE;
				break;
			}
			else if ((players[opponent]).opponent != thread_num)
			{
				break;
			}

			Sleep(100);
		}
	}
	(players[thread_num]).is_busy = TRUE;
	tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);

	SendRes = SendString(tmp_string, t_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(t_socket);
		return;
	}

	if (is_replay != TRUE) {
		strcpy(message_type_out, "SERVER_MAIN_MENU");
		(players[thread_num]).opponent = -1;
		tmp_string = EncodeMessage(message_type_out, parameters, number_of_parameters);
		SendRes = SendString(tmp_string, t_socket);
		if (SendRes == TRNS_FAILED)
		{
			printf("Service socket error while writing, closing thread.\n");
			closesocket(t_socket);
			return;
		}
	}
}

#pragma endregion

#pragma region Initialize and free
//free array of char*
void FreeParams(char** parameters)
{
	int i;
	for (i = 0; i < MAX_NUM_OF_ARGUMENTS; i++)
	{
		free((parameters)[i]);
	}
	free((parameters));
}
//initializing array of char*


//Initializing all the players
void InitilaizePlayers()
{
	int i;
	for (i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		(players[i]).name = (char*)malloc(sizeof(char)*MAX_SIZE_OF_ARGUMENTS);
		if ((players[i]).name == NULL) {
			PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
			return;
		}
		(players[i]).last_move = (char*)malloc(sizeof(char)*MAX_SIZE_OF_ARGUMENTS);		
		if ((players[i]).last_move == NULL) {
			PrintError(MALLOC_FAILED, 0, __LINE__, __FILE__);
			return;
		}
		(players[i]).is_busy = TRUE;
		(players[i]).played_move = FALSE;
	}
	
	strcpy(players[0].name, "cpu"); //create pc player
}
//Clearing players
void FreePlayers()
{
	int i;
	for (i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		free((players[i]).name);
		free((players[i]).last_move);
	}
}
#pragma endregion

/*searching for free opponent
input: int number of player searching
output: int number of free opponent, when -1 if not found
*/
int FindOpponent(int my_number)
{
	int i;
	for (i = 1; i < MAX_NUM_PLAYERS; i++) //1 saved for pc
	{
		if (i != my_number) {
			if ((players[i]).is_busy == FALSE)
				return i;
		}
	}
	return -1;
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
