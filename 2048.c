#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

#define BEEPFREQ 150
#define BEEPDUR 200

#define SIZE 4 // change this to make the board 4x4, 5x5, 6x6 max is 7
// if you change the SIZE then delete the save.dat  file if any

int board[SIZE][SIZE] = {0};
int prevboard[SIZE][SIZE] = {0};
int score = 0;
int prevscore = 0;
int sound = 0; // 0 for unmuted and 1 for muted
int undo = 0; // 0 for not able to undo and 1 for able to do undo
int saveavailable = 0; // 0 for not available and 1 for available
char m[50] = " "; //will be used for messaging throughout the game

//this variable will saves the current color attributes of the console

WORD saved_attributes;

//functions

void setupboard(void);
void printboard(void);
void stack(void);
void combine(void);
void reverse(void);
void transpose(void);
void left(void);
void right(void);
void up(void);
void down(void);
void action(char op);
void copy_board_to_prevboard(void);
int check_if_board_changed(void);
void addnewvalue(void);
int checkgamestatus(void);
int horizontalmovecheck(void);
int verticalmovecheck(void);
int zerotilecheck(void);
void setcolor(int tile);
void savecurrentcolorattributes(void);
void gamereset(void);
void gameundo(void);
void gamesave(void);
void gameload(void);
void deletegamesave(void);
void beepsound(int freq, int dur);

int main()
{
	FILE *savefile = fopen("save.dat","r");
	if (savefile == NULL)
	{
		saveavailable = 0;
	}
	else {
		fclose(savefile);
		saveavailable = 1;
	}
	system("color 0f");
	savecurrentcolorattributes();
	char op;
	setupboard();
	int running = 0;
	system("title 2048");
	printboard();
	while(running == 0)
	{
		op = getch();
		if (op == 'q' || op == 'Q')
		{
			system("color 07");
			return 0;
		}
		action(op);
		running = checkgamestatus();
	}
	if (running == 1)
	{
		printf("\n\t\tYou won!");
		getch();
		system("color 07");
	}
	else if (running == 2)
	{
		system("color 07");
		printf("\n\t\tYou lost!");
	}
	return 0;
}

void setupboard(void)
{
	int i, j, k, random;
	srand(time(NULL));
	for(k = 1; k < 3; k++)
	{
		do
		{
			i = rand() % SIZE;
			j = rand() % SIZE;
		} while(board[i][j] != 0);
		random = rand();
		if (random % 10 == 0) // 0.9 probability for a 2 and 0.1 probability for a 4
		{
			board[i][j] = 4;	
		}
		else
		{
			board[i][j] = 2;	
		}
	}
}

void addnewvalue(void)
{
	int i, j, add, temp;
	srand(time(NULL));
	do {
		i = rand() % SIZE;
		j = rand() % SIZE;
	} while(board[i][j] != 0);
	temp = rand();
	if ((temp % 10) == 0) // 0.1 prbability for a 4 and 0.9 probability for a 2
	{
		add = 4;
	}
	else
	{
		add = 2;
	}
	board[i][j] = add;
}

void printboard(void)
{
	system("cls");
	int i, j,k;
	printf("\n\n\n\t\t2048 by hamzacyberpatcher\n\n\n\t\t\t");
	
	printf("\n");
	
	for(i = 0; i < SIZE; i++)
	{
		printf("\t");
		for (j = 0; j < SIZE; j++)
		{
			setcolor(board[i][j]);
			if (board[i][j] != 0)
			printf("  %4d",board[i][j]);
			else
			printf("     .");
			setcolor(0);
		}
		if(i == 2)
		{
			printf("\t\tScore : %d",score);		
		}
		printf("\n");
	}
	printf("\n\t\t");
	if (undo == 0)
	{
		setcolor(2);
		printf("U-Undo");
		setcolor(0);
	}
	else
	{
		setcolor(4);
		printf("U-Undo");
		setcolor(0);
	}
	if (sound == 0)
	{
		printf("\t\tM-Mute  ");
	}
	else
	{
		printf("\t\tM-Unmute");
	}
	printf("\tR-Reset");
	printf("\n\t\tS-Save");
	if (saveavailable == 0)
	{
		setcolor(2);
		printf("\t\tL-Load");
		setcolor(0);
	}
	else
	{
		setcolor(4);
		printf("\t\tL-Load");
		setcolor(0);
	}
	printf("\t\tQ-Quit");
	printf("\n\t\tD-Delete Save File");
	if (strlen(m) != 1)
	{
		printf("\n\t\t%s",m);
		strcpy(m," ");
	}
}

void stack(void)
{
	/*
	This function will move everything on the board towards left
	Before:			After:
	[0,2,2,0]		[2,2,0,0]
	[2,0,2,2]		[2,2,2,0]
	[2,0,2,0]		[2,2,0,0]
	[0,0,2,2]		[2,2,0,0]
	*/
	int i, j, stackedboard[SIZE][SIZE] = {0}, fillposition;
	for(i = 0; i < SIZE; i++)
	{
		fillposition = 0;
		for(j = 0; j < SIZE; j++)
		{
			if (board[i][j] != 0)
			{
				stackedboard[i][fillposition] = board[i][j];
				fillposition++;
			}
		}
	}
	
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			board[i][j] = stackedboard[i][j];
		}
	}
}

void combine(void)
{
	//this function will add the horizontally adjacent values from
	//left to right in the board
	int i ,j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE - 1; j++)
		{
			if(board[i][j] == board[i][j+1])
			{
				board[i][j] *= 2;
				score += board[i][j];
				board[i][j+1] = 0;
			}
		}
	}
}

void reverse(void)
{
	// this function will reverse every row in the board
	// Before [2,4,8,8] After [8,8,4,2]
	int i, j, reversedboard[SIZE][SIZE] = {0};
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			reversedboard[i][j] = board[i][SIZE-j-1];
		}
	}
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			board[i][j] = reversedboard[i][j];
		}
	}
}

void transpose(void)
{
	// will take the transpose of the board
	//Before:
	//[2,2,4,8]
	//[32,16,8,8]
	//[2,4,8,16]
	//[4,8,16,64]
	//After:
	//[2,32,2,4]
	//[2,16,4,8]
	//[4,8,8,16]
	//[8,8,16,64]
	int i, j, transposedboard[SIZE][SIZE] = {0};
	
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			transposedboard[i][j] = board[j][i];
		}
	}
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			board[i][j] = transposedboard[i][j];
		}
	}
}

void left(void)
{
	copy_board_to_prevboard();
	stack();
	combine();
	stack();
	int n = check_if_board_changed();
	if (n == 0)
	{
		undo = 1;
		addnewvalue();
		printboard();
		beepsound(BEEPFREQ,BEEPDUR);
	}
	else
	{
		undo = 0;
	}
}

void right(void)
{
	copy_board_to_prevboard();
	reverse();
	stack();
	combine();
	stack();
	reverse();
	int n = check_if_board_changed();
	if (n == 0)
	{
		undo = 1;
		addnewvalue();
		printboard();
		beepsound(BEEPFREQ,BEEPDUR);
	}
	else {
		undo = 0;
	}
}

void up(void)
{
	copy_board_to_prevboard();
	transpose();
	stack();
	combine();
	stack();
	transpose();
	int n = check_if_board_changed();
	if (n == 0)
	{
		undo = 1;
		addnewvalue();
		printboard();
		beepsound(BEEPFREQ,BEEPDUR);
	}
	else {
		undo = 0;
	}
}

void down(void)
{
	copy_board_to_prevboard();
	transpose();
	reverse();
	stack();
	combine();
	stack();
	reverse();
	transpose();
	int n = check_if_board_changed();
	if (n == 0)
	{
		undo = 1;
		addnewvalue();
		printboard();
		beepsound(BEEPFREQ,BEEPDUR);
	}
	else {
		undo = 0;
	}
}

void action(char op)
{
	if (op == UP)
	{
		up();
	}
	else if (op == DOWN)
	{
		down();
	}
	else if ( op == LEFT)
	{
		left();
	}
	else if ( op == RIGHT)
	{
		right();
	}
	else if (op == 'r' || op == 'R')
	{
		gamereset();
		setupboard();
		printboard();
	}
	else if (op == 'm')
	{
		if (sound == 1)
		{
			sound = 0;
		}
		else if (sound == 0)
		{
			sound = 1;
		}
		printboard();
	}
	else if (op == 'u' || op == 'U')
	{
		gameundo();
		printboard();
	}
	else if (op == 's' || op == 'S')
	{
		gamesave();
		printboard();
	}
	else if (op == 'l' || op == 'L')
	{
		if (saveavailable == 0)
		{
			beepsound(BEEPFREQ+100,BEEPDUR);
			strcpy(m,"Save file not available");
			printboard();
			return;
		}
		else {
			gameload();
			printboard();
		}
	}
	else if (op == 'd' || op == 'D')
	{
		deletegamesave();
		printboard();
	}
}

void copy_board_to_prevboard(void)
{
	int i , j;
	for ( i = 0; i < SIZE; i++)
	{
		for( j = 0; j < SIZE; j++)
		{
			prevboard[i][j] = board[i][j];
		}
	}
	prevscore = score;
}

int check_if_board_changed(void)
{
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			if(board[i][j] != prevboard[i][j])
			{
				return 0; // means that board has changed
			}
		}
	}
	return 1; // means that the board has not changed
}

int checkgamestatus(void)
{
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			if (board[i][j] == 2048)
			{
				return 1; //indicates that the user has won
			}
		}
	}
	int hm, vm, zt;
	hm = horizontalmovecheck();
	vm = verticalmovecheck();
	zt = zerotilecheck();
	if (zt == 1 && hm == 1 && vm == 1)
	{
		return 2; // indicates that the user has lost
	}
	return 0;
}

int zerotilecheck(void)
{
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			if (board[i][j] == 0)
			{
				return 0; // means that a zero tile exists
			}
		}
	}
	return 1; // means that a zero tile doesnot exists
}

int horizontalmovecheck(void)
{
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE - 1; j++)
		{
			if(board[i][j] == board[i][j+1])
			{
				return 0; // means that a horizonatal move exists
			}
		}
	}
	return 1; // means that a horizontal move doesnot exists
}

int verticalmovecheck(void)
{
	int i, j;
	for(i = 0; i < SIZE-1; i++)
	{
		for(j = 0; j < SIZE; j++)
		{
			if(board[i][j] == board[i+1][j])
			{
				return 0; // means that a vertical move exists
			}
		}
	}
	return 1; // means that a vertical move doesnot exists
}

void setcolor(int tile)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (tile == 0)
	{
		SetConsoleTextAttribute(hConsole, saved_attributes);
	}
	else if (tile == 2)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_RED);
	}
	else if (tile == 4)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN);
	}
	else if (tile == 8)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_BLUE);
	}
	else if (tile == 16)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (tile == 32)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else if (tile == 64)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (tile == 128)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_BLUE | BACKGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (tile == 256)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN | BACKGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (tile == 512)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (tile == 1024)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (tile == 2048)
	{
		SetConsoleTextAttribute(hConsole,FOREGROUND_RED | BACKGROUND_GREEN) | FOREGROUND_INTENSITY;
	}
	else
	{
		SetConsoleTextAttribute(hConsole, saved_attributes);
	}
}

void savecurrentcolorattributes(void){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
}

void gamereset(void)
{
	int i, j;
	for ( i = 0; i < SIZE; i++)
	{
		for ( j = 0; j < SIZE; j++)
		{
			board[i][j] = 0;
			prevboard[i][j] = 0;
		}
	}
	prevscore = 0;
	score = 0;
	undo = 0;
	beepsound(BEEPFREQ+100,BEEPDUR);
	strcpy(m,"Game reseted");
}

void gameundo(void)
{
	int i, j;
	if (undo == 1)
	{
		for(i = 0; i < SIZE; i++)
		{
			for(j = 0; j < SIZE; j++)
			{
				board[i][j] = prevboard[i][j];
			}
		}
		undo = 0;
		score = prevscore;
		beepsound(BEEPFREQ+50,BEEPDUR);
	}
	else if ( undo == 0)
	{
		beepsound(BEEPFREQ+100,BEEPDUR);
		return;
	}
	return;
}

void gamesave(void)
{
	char consent;
	beepsound(BEEPFREQ+100,BEEPDUR);
	printf("\n\t\tDo you want to save the game (y/n): ");
	consent = getche();
	if (consent == 'n' || consent == 'N')
	{
		beepsound(BEEPFREQ+100,BEEPDUR);
		return;
	}
	else if (consent == 'y' || consent == 'Y')
	{
		FILE *savefile;
	savefile = fopen("save.dat","w");
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE ; j++)
		{
			fprintf(savefile,"%d\n",board[i][j]);
		}
	}
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE ; j++)
		{
			fprintf(savefile,"%d\n",prevboard[i][j]);
		}
	}
	fprintf(savefile,"%d\n%d\n%d",score,prevscore,undo);
	fclose(savefile);
	saveavailable = 1;
	strcpy(m,"Game saved successfully");
	beepsound(BEEPFREQ,BEEPDUR);
	}
	else
	{
		beepsound(BEEPFREQ+100,BEEPDUR);
		strcpy(m,"Invalid Choice");
	}
}

void gameload(void)
{
	char consent;
	beepsound(BEEPFREQ+100,BEEPDUR);
	printf("\n\t\tDo you want to load the saved game (y/n) : ");
	consent = getche();
	if (consent == 'y' || consent == 'Y')
	{
		FILE *savefile;
	savefile = fopen("save.dat","r");
	int i, j;
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE ; j++)
		{
			fscanf(savefile,"%d\n",&board[i][j]);
		}
	}
	for(i = 0; i < SIZE; i++)
	{
		for(j = 0; j < SIZE ; j++)
		{
			fscanf(savefile,"%d\n",&prevboard[i][j]);
		}
	}
	fscanf(savefile,"%d\n%d\n%d",&score,&prevscore,&undo);
	fclose(savefile);
	strcpy(m,"Game loaded successfully");
	beepsound(BEEPFREQ,BEEPDUR);	
	}
	else if (consent == 'n' || consent == 'N')	
	{
		beepsound(BEEPFREQ+100,BEEPDUR);
		return;
	}
	else {
		beepsound(BEEPFREQ+100,BEEPDUR);
		strcpy(m,"Invalid Choice");
	}
}

void deletegamesave(void)
{
	char consent;
	beepsound(BEEPFREQ+100,BEEPDUR);
	printf("\n\t\tDo you want to delete the save file (y/n) : ");
	consent = getche();
	if (consent == 'y' || consent == 'Y')
	{
		if (saveavailable == 0)
	{
		strcpy(m,"Save file not available");
		beepsound(BEEPFREQ+100,BEEPDUR);
		return;
	}
	else
	{
		if(remove("save.dat") == 0)
		{
			strcpy(m,"Save file deleted successfully");
			saveavailable = 0;
			beepsound(BEEPFREQ,BEEPDUR);
		}
		else
		{
			strcpy(m,"Unable to delete the save file");
			beepsound(BEEPFREQ+100,BEEPDUR);
		}
	}	
	}
	else if (consent == 'n' || consent == 'N')
	{
		beepsound(BEEPFREQ+100,BEEPDUR);
		return;
	}
	else {
		beepsound(BEEPFREQ+100,BEEPDUR);
		strcpy(m,"Invalid Choice");
	}
}

void beepsound(int freq,int dur)
{
	if (sound == 1)
	{
		return;
	}
	Beep(freq,dur);
}
