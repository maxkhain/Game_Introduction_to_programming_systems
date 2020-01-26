#define _CRT_SECURE_NO_WARNINGS
#include "ReadWrite.h"

static HANDLE mutex;
static HANDLE turnstile;
static HANDLE semaphor_readers; //for semaphor gun
static int readers_inside = 0;
char FileName[] = "LeaderBoard.csv";

static void ReportErrorAndEndProgram();

/*Initilize internal mutexes and semaphor
input:
output:
Logic:

*/
void Initialize()
{
	BOOL an_error_occured = FALSE;	

	semaphor_readers = CreateSemaphore(
		NULL,	                /* Default security attributes */
		0,		                /* Initial Count - 0 */
		1,		/* Maximum Count */
		NULL);                  /* un-named */
	if (semaphor_readers == NULL) {
		an_error_occured = TRUE;
		goto Main_Cleanup_0;
	}
	turnstile = CreateSemaphore(
		NULL,	                /* Default security attributes */
		0,		                /* Initial Count - 0 */
		1,		/* Maximum Count */
		NULL);                  /* un-named */
	if (turnstile == NULL) {
		an_error_occured = TRUE;
		goto Main_Cleanup_1;
	}
	mutex = CreateMutex(
		NULL,               /* default security attributes */
		FALSE,	            /* don't lock mutex immediately */
		NULL);              /* un-named */

	if (mutex == NULL) {
		an_error_occured = TRUE;
		goto Main_Cleanup_2;
	}
	return;

Main_Cleanup_2:
	CloseHandle(mutex);
Main_Cleanup_1:
	CloseHandle(mutex);
Main_Cleanup_0:
	if (an_error_occured) ReportErrorAndEndProgram();
}

static void ReportErrorAndEndProgram()
{
	printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}

/*Read from CSV 
input:
output:
Logic: 

*/
char** ReadScoreTable()
{
	DWORD wait_res;
	BOOL release_res;
	LONG previous_count;
	char** ret_string_array = NULL;

	wait_res = WaitForSingleObject(turnstile, INFINITE); //check if no writer inside
	if (wait_res != WAIT_OBJECT_0)
		ReportErrorAndEndProgram();

	release_res = ReleaseSemaphore(
		turnstile,
		1, 		/* let othe readers enter */
		&previous_count);
	if (release_res == FALSE)
		ReportErrorAndEndProgram();

	readers_inside++; //increase readers in room

	if (readers_inside == 1)
	{
		wait_res = WaitForSingleObject(semaphor_readers, INFINITE); //Room not empty
		if (wait_res != WAIT_OBJECT_0)
			ReportErrorAndEndProgram();
	}
	wait_res = WaitForSingleObject(mutex, INFINITE); //check if no writer inside
	if (wait_res != WAIT_OBJECT_0)
		ReportErrorAndEndProgram();
	//read file


	//finished reading
	readers_inside--;
	if (readers_inside == 0)
	{
		release_res = ReleaseSemaphore(
			semaphor_readers,
			1, 		/* room empty */
			&previous_count);
		if (release_res == FALSE)
			ReportErrorAndEndProgram();
	}

	release_res = ReleaseMutex(mutex);
	if (release_res == FALSE)
		ReportErrorAndEndProgram();
	return ret_string_array;
}
/*Read CSV file
output: char* to string of table with \n between lines

*/
char* ReadCSV()
{
	char* data = NULL;
	FILE *fp = NULL;
	fp = fopen(FileName, "r");
	data = inputString(fp, START_SIZE_OF_STRING);
	fclose(fp);
	return data;
}

/*Function taken from Stack Overflow https://stackoverflow.com/questions/16870485/how-can-i-read-an-input-string-of-unknown-length 
function reads CSV file and returns char* to the string of data
input: file pointer, starting size(first line) of table
output: string of the data in the table

*/
char *inputString(FILE* fp, size_t size) {
	//The size is extended by the input with the value of the provisional
	char *str;
	int ch;
	size_t len = 0;
	str = realloc(NULL, sizeof(char)*size);//size is start size
	if (!str)return str;
	while (EOF != (ch = fgetc(fp))) { // && ch != '\n'
		str[len++] = ch;
		if (len == size) {
			str = realloc(str, sizeof(char)*(size += 16));
			if (!str)return str;
		}
	}
	str[len++] = '\0';

	return realloc(str, sizeof(char)*len);
}


/*Read from CSV
input:
output:
Logic:

*/
void WriteToScoreTable(char* player_one_name, char* player_two_name, int winner)
{
	DWORD wait_res;
	BOOL release_res;
	LONG previous_count;
	wait_res = WaitForSingleObject(turnstile, INFINITE); //check if no writer inside
	if (wait_res != WAIT_OBJECT_0)
		ReportErrorAndEndProgram();
	wait_res = WaitForSingleObject(semaphor_readers, INFINITE); //check if no writer inside
	if (wait_res != WAIT_OBJECT_0)
		ReportErrorAndEndProgram();
	//writer inside critical section



	//writer left
	release_res = ReleaseSemaphore(
		turnstile,
		1, 		/* let othe readers enter */
		&previous_count);
	if (release_res == FALSE)
		ReportErrorAndEndProgram();

	release_res = ReleaseSemaphore(
		semaphor_readers,
		1, 		/* let othe readers enter */
		&previous_count);
	if (release_res == FALSE)
		ReportErrorAndEndProgram();
}
/* 
writes to CSV file and updates the standings
first we read the data
than calculate new stats
update table
input: file pointer, player1 name, player2 name, who won

*/
void WriteToCSV(FILE* fp, char* player_one_name, char* player_two_name, int winner) {

}