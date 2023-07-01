#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <conio.h>
#include <setjmp.h>

#define ROWS 30
#define COLS 130

jmp_buf saveBuff;

/*
xmalloc - a better malloc
@size: the size of bytes required
*/
void *xmalloc(size_t size)
{
        void *pointer = malloc(size);
        if (pointer == NULL)
        {
                printf("memory allocation failed.\n");
                exit(EXIT_FAILURE);
        }
        return (pointer);
}

/*
the snake node
*/
typedef struct node
{
        int x;
        int y;
        int xPrev;
        int yPrev;
        struct node *next;
        struct node *prev;
} node_t;

/*
setChar - initialises the board for the columns
@array: the board
*/
void setChar(char **array)
{
        for (int i = 0; i < ROWS; i++)
        {
                array[i] = calloc(COLS, sizeof(char));
                if (i != 0 && i != (ROWS - 1))
                        memset(array[i], ' ', COLS);
                else
                        memset(array[i], '=',COLS);
        }
}

/*
direction - tells whether it is moving up or down
@key: the pressed key
Return: char
*/
char direction(int key)
{
        if (key == 105)
                return ('u');
        else if (key == 106)
                return ('l');
        else if (key == 107)
                return ('d');
        else if (key == 108)
                return ('r');
        return ('r');
}

/*
isEating - determines if the apple is to be eaten
@array: the board
@y: the y position of the head
@x: the x position of the head
Return: bool
*/
bool isEating(char **array, int y, int x)
{
        if (array[y][x] == 'a')
                return (true);
        return (false);
}

/*
check - checks if it is eating itself or colliding with  a wall
@head: the snake's head
@array: the board
*/
void check(node_t *head, char **array)
{
        if (head->y == 0)
                head->y = (ROWS - 2);
        else if (head->y == (ROWS - 1))
                head->y = 1;
        else if (head->x == 0)
                head->x = (COLS - 1);
        else if (head->x == COLS)
                head->x = 1;
        else
        {
                node_t *holder = head->next;
                while (holder != NULL)
                {
                        if ((holder->x == head->x && holder->y == head->y) && head->next != holder)
                        {
                                printf("Game Over, you can't eat yourself!\n");
                                exit(EXIT_FAILURE);
                        }
                        holder  = holder->next;
                }
        }
}

/*
move - moves the head
@head: the snake's head
@key: the key pressed
*/
void move(char **array, node_t *head, int key)
{
        node_t *temp = head;
        int newYup = head->y++;
        int newYdown = head->y--;
        int newXleft = head->x++;
        int newXright = head->x--;

        if (direction(key) == 'u')
        {
                head->xPrev = head->x;
                head->yPrev = head->y;
                if (isEating(array, newYup, head->x))
                        array[newYup][head->x] = ' ';
                head->y--;
        }
        else if (direction(key) == 'd')
        {
                head->yPrev = head->y;
                head->xPrev = head->x;
                if (isEating(array, newYdown, head->x))
                        array[newYdown][head->x] = ' ';
                head->y = newYdown;
        }
        else if (direction(key) == 'l')
        {
                head->xPrev = head->x;
                head->yPrev = head->y;
                if (isEating(array, head->y, newXleft))
                        array[head->y][newXleft] = ' ';
                head->x--;
        }
        else if (direction(key) == 'r')
        {
                head->xPrev = head->x;
                head->yPrev = head->y;
                if (isEating(array, head->y, newXright))
                        array[head->y][newXright] = ' ';
                head->x++;
        }
        check(head, array);

        while (temp != NULL)
        {
                if (temp != head)
                {
                        temp->xPrev = temp->x;
                        temp->yPrev = temp->y;
                        temp->x = temp->prev->xPrev;
                        temp->y = temp->prev->yPrev;
                }
                temp = temp->next;
        }
}

/*
addToSnake - adds new tails to the snake
@head: the snake's head
*/
void addToSnake(node_t *head)
{
        node_t *temp = xmalloc(sizeof(node_t));
        temp->next = NULL;
        node_t *ptr = head;
        while (ptr->next != NULL)
                ptr = ptr->next;
        ptr->next = temp;
        temp->x = ptr->x;
        temp->y = ptr->y;
        temp->prev = ptr;
}

/*
addToEmpty - initialises the snake head
Return: node_t(the snake object)
*/
node_t *addToEmpty()
{
        node_t *temp = xmalloc(sizeof(node_t));
        srand(time(NULL));
        int x = rand() % COLS;
        int y = rand() % ROWS;
        temp->next = NULL;
        temp->prev = NULL;
        temp->y = y;
        temp->x = x;
        if (temp->y == 0)
                temp->y++;
        if (temp->x == 0)
                temp->x++;
        return (temp);
}

/*
clearScreen - clears the screen for different os
*/
void clearScreen()
{
        int clear = system("cls 2> nul");
        if (clear != 1)
                longjmp(saveBuff, 1);
}

/*
drawSurface - draws the board
@array: the board
@score: the score
*/
void drawSurface(char **array, int score)
{
        int i, j;

        printf("  SCORE: %d\n\n", score);
        for (i = 0; i < ROWS; i++)
        {
                printf("I");
                for (j = 0; j < COLS; j++)
                {
                        if (array[i][j] == '*')
                                printf("\033[1;31m%c\033[1;0m", array[i][j]);
                        else if (array[i][j] == 'a')
                                printf("\033[1;32m%c\033[1;0m", array[i][j]);
                        else
                                printf("%c",array[i][j]);
                }
                printf("I\n");
        }
        sleep(1);
        if (setjmp(saveBuff) == 0)
                clearScreen();
        else
                system("clear 2> nul");
}

/*
setPlace - sets the new positions for the snake nodes
@array: the board
@link: the head
*/
void setPlace(char **array, node_t *link)
{
        node_t *temp = link;
        while (temp != NULL)
        {
                array[temp->y][temp->x] = '*';
                temp = temp->next;
        }
}

/*
randomApple - places a random apple on the board
@array: the board
*/
void randomApple(char **array)
{
        srand(time(NULL));
        int x = rand() % (ROWS - 2);
        int y = rand() % (COLS - 2);
        if (x < 2)
                x++;
        if (y < 2)
                y++;
        while (array[x][y] == '*')
        {
                x = rand() % ROWS;
                y = rand() % COLS;
        }
        array[x][y] = 'a';
}

/*
undoWrite - undoes the write on the board
@array: the board
@head: the snake's head
*/
void undoWrite(char **array, node_t *head)
{
        node_t *temp = head;
        while (temp != NULL)
        {
                array[temp->y][temp->x] = ' ';
                temp = temp->next;
        }
}

/*
isEaten - determines if the apple has been eaten
@array: the board
*/
int isEaten(char **array)
{
        int i, j;
        for (i = 0; i < ROWS; i++)
        {
                for (j = 0; j < COLS; j++)
                {
                        if (array[i][j] == 'a')
                                return (0);
                }
        }
        return (1);
}

int main(void)
{
        char **array = xmalloc((ROWS * COLS) + 1);
        int score = 0;
        int key = 0;
       int pressedKey = 0;

        node_t *head = addToEmpty();

        srand(time(NULL));
        
        key = 108;

        setChar(array);
        randomApple(array);

        //initialising the game
        printf("\nThis is the snake game, \nUse i ,j ,k ,l for navigating the snake,\ni - UP, j - LEFT, k - DOWN, l - RIGHT, \nu can use p to pause and e to exit.\nThe red '*' is the snake and the green 'a' is the apple.\n\n");
        printf("Press enter to continue...\n");
        getchar();

        while (1)
        {
                if (isEaten(array))
                { 
                        addToSnake(head);
                        setPlace(array, head);
                        randomApple(array);
                        score++;
                }
                move(array, head, key);
                setPlace(array, head);
                drawSurface(array, score);
                undoWrite(array, head);

                // condtions for  the keys pressed
                if (kbhit())
                {
                        pressedKey = getch();
                        if (key != 107 && pressedKey == 105)
                                key = 105;
                        else if (key != 105 && pressedKey == 107)
                                key = 107;
                        else if (key != 108 && pressedKey == 106)
                                key = 106;
                        else if (key != 106 && pressedKey == 108)
                                key = 108;
                        else if (pressedKey == 112)
                        {
                                printf("\nThe game has been paused press enter to continue....\n");
                                getchar();
                        }
                        else if (pressedKey == 101)
                        {
                                usleep(500);
                                printf("\nAre u sure u want to exit? (y/n): ");
                                pressedKey = getchar();
                                if (pressedKey == 121)
                                        exit(-1);
                        }
                }
        }

        return (0);
}