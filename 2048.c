//#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#define SIZE 4

long int score=0;

void setBufferedInput(bool enable)
{
	static bool enabled = true;
	static struct termios old;
	struct termios new;

	if (enable && !enabled) 
	{
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) 
	{
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&new);
		// we want to keep the old setting to restore them at the end
		old = new;
		// disable canonical mode (buffered i/o) and local echo
		new.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		// set the new state
		enabled = false;
	}
}


void drawBoard(int board[SIZE][SIZE])
{
	int x,y;
	system("clear");
	printf("-------------------------------------------------\n");
	printf("\tLeft- a or left arrow\n\tRight- d or right arrow\n\tUp- w or up arrow\n\tDown- s or down arrow\n");
	printf("-------------------------------------------------\n\n");

	for(y=0;y<SIZE;y++)
	{
		for(x=0;x<SIZE;x++)
		{
			if (board[x][y]!=0) 
			{
				char s[8];
				snprintf(s,8,"%u",board[x][y]);
				int t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} 
			else
				printf("   -   ");
		}
		printf("\n\n");
	}
	printf("-------------------------------------------------\n");
	printf("\tRdrocks09 %ld pts\n",score);
	printf("-------------------------------------------------\n");
}

int findTarget(int array[SIZE],int x,int stop) {
	int t;
	// if the position is already on the first, don't evaluate
	if (x==0) {
		return x;
	}
	for(t=x-1;t>=0;t--) {
		if (array[t]!=0) {
			if (array[t]!=array[x]) {
				// merge is not possible, take next position
				return t+1;
			}
			return t;
		} else {
			// we should not slide further, return this one
			if (t==stop) {
				return t;
			}
		}
	}
	// we did not find a
	return x;
}

bool slideArray(int array[SIZE]) {
	bool success = false;
	int x,t,stop=0;

	for (x=0;x<SIZE;x++) {
		if (array[x]!=0) {
			t = findTarget(array,x,stop);
			// if target is not original position, then move or merge
			if (t!=x) {
				// if target is not zero, set stop to avoid double merge
				if (array[t]!=0) {
					score+=array[t]+array[x];
					stop = t+1;
				}
				array[t]+=array[x];
				array[x]=0;
				success = true;
			}
		}
	}
	return success;
}

void rotateBoard(int board[SIZE][SIZE]) {
	int i,j,n=SIZE;
	int tmp;
	for (i=0; i<n/2; i++){
		for (j=i; j<n-i-1; j++){
			tmp = board[i][j];
			board[i][j] = board[j][n-i-1];
			board[j][n-i-1] = board[n-i-1][n-j-1];
			board[n-i-1][n-j-1] = board[n-j-1][i];
			board[n-j-1][i] = tmp;
		}
	}
}

bool moveUp(int board[SIZE][SIZE]) {
	bool success = false;
	int x;
	for (x=0;x<SIZE;x++) {
		success |= slideArray(board[x]);
	}
	return success;
}

bool moveLeft(int board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveDown(int board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveRight(int board[SIZE][SIZE]) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	return success;
}

bool findPairDown(int board[SIZE][SIZE]) {
	bool success = false;
	int x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE-1;y++) {
			if (board[x][y]==board[x][y+1]) return true;
		}
	}
	return success;
}

int countEmpty(int board[SIZE][SIZE]) {
	int x,y;
	int count=0;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				count++;
			}
		}
	}
	return count;
}

bool gameEnded(int board[SIZE][SIZE]) {
	bool ended = true;
	if (countEmpty(board)>0) return false;
	if (findPairDown(board)) return false;
	rotateBoard(board);
	if (findPairDown(board)) ended = false;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return ended;
}

void addRandom(int board[SIZE][SIZE]) {
	static bool initialized = false;
	int x,y;
	int r,len=0;
	int n,list[SIZE*SIZE][2];

	if (!initialized) {
		srand(time(NULL));
		initialized = true;
	}

	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				list[len][0]=x;
				list[len][1]=y;
				len++;
			}
		}
	}

	if (len>0) {
		r = rand()%len;
		x = list[r][0];
		y = list[r][1];
		n = ((rand()%10)/9+1)*2;
		board[x][y]=n;
	}
}

int main() {
	int board[SIZE][SIZE];
	char c;
	bool success;

	system("clear");
	memset(board,0,sizeof(board));
	addRandom(board);
	addRandom(board);
	drawBoard(board);
	setBufferedInput(false);
	while (true) {
		c=getchar();
		switch(c) {
			case 97:	// 'a' key
			case 68:	// left arrow
				success = moveLeft(board);  break;
			case 100:	// 'd' key
			case 67:	// right arrow
				success = moveRight(board); break;
			case 119:	// 'w' key
			case 65:	// up arrow
				success = moveUp(board);    break;
			case 115:	// 's' key
			case 66:	// down arrow
				success = moveDown(board);  break;
			default: success = false;
		}
		if (success) {
			drawBoard(board);
			addRandom(board);
			drawBoard(board);
			if (gameEnded(board)) {
				printf("         GAME OVER          \n");
				break;
			}
		}
	}
	setBufferedInput(true);
	return 0;
}

