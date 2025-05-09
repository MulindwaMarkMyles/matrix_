#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DROPS 100  // Number of raindrops

int ROWS = 0;
int COLS = 0;

void get_terminal_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    ROWS = w.ws_row;
    COLS = w.ws_col;
    
    // Fallback to default values if we can't get terminal size
    if (ROWS <= 0) ROWS = 24;
    if (COLS <= 0) COLS = 80;
    
    // Subtract 1 from rows to prevent scrolling
    ROWS -= 1;
}

void set(char **array)
{
	for (int i = 0; i < ROWS; i++)
	{
		array[i] = calloc(COLS, sizeof(char));
		memset(array[i], ' ', COLS);
	}
}

void clear_screen()
{
	printf("\033[H\033[J"); // ANSI escape sequence to clear the screen
}

void draw(char **array)
{
	clear_screen();
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			printf("\033[1;32m%c\033[1;0m", array[i][j]);
		}
		putchar('\n');
	}
}

void update_matrix(char **array)
{
	// Move existing characters down
	for (int i = ROWS - 1; i > 0; i--) {
		for (int j = 0; j < COLS; j++) {
			if (array[i-1][j] != ' ' && rand() % 3 > 0) {
				array[i][j] = array[i-1][j];
			}
		}
	}
	
	// Create new drops at the top
	for (int j = 0; j < COLS; j++) {
		if (rand() % 20 == 0) {
			int i = rand() % 7;
			char signs[] = "@$%&+=x";
			array[0][j] = signs[i];
		} else {
			array[0][j] = ' ';
		}
	}
	
	// Randomly remove some characters to create trails
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (array[i][j] != ' ' && rand() % 10 == 0) {
				array[i][j] = ' ';
			}
		}
	}
}

int main(void)
{
    // Get terminal dimensions
    get_terminal_size();
    
    char **array = malloc(ROWS * sizeof(char*));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    srand(time(NULL));
    set(array);
    
    while (1)
    {
        // Check if terminal size changed
        int old_rows = ROWS;
        int old_cols = COLS;
        get_terminal_size();
        
        // Reallocate if size changed
        if (old_rows != ROWS || old_cols != COLS) {
            // Free old matrix
            for (int i = 0; i < old_rows; i++) {
                free(array[i]);
            }
            
            // Reallocate for new size
            array = realloc(array, ROWS * sizeof(char*));
            if (array == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                return 1;
            }
            
            // Initialize the new matrix
            set(array);
        }
        
        update_matrix(array);
        draw(array);
        usleep(100000);  // Sleep for 100ms between frames
    }
    
    // Free memory (never reached in this infinite loop, but good practice)
    for (int i = 0; i < ROWS; i++) {
        free(array[i]);
    }
    free(array);
    
    return (0);
}