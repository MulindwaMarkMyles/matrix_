#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 37
#define COLS 148

void set(char **array)
{
	for (int i = 0; i < ROWS; i++)
	{
		array[i] = calloc(COLS, sizeof(char));
		memset(array[i], ' ', COLS);
	}
}
void draw(char **array)
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			printf("\033[1;32m%c\033[1;0m", array[i][j]);
		}
		putchar('\n');
	}
}
void assign(char **array)
{
	int x = rand() % ROWS;
	int y = rand() % COLS;
	int turn = 10;
	int i = rand() % 7;
	char signs[]  = "@$%&+=x";
	if (array[x][y] == ' ')
	{
		array[x][y] = signs[i];
	}
}

int main(void)
{
	char **array = malloc(ROWS + 1);
	srand(time(NULL));
	set(array);
	while (1)
	{
		assign(array);
		draw(array);
	}
	return (0);
}