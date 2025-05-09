#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/select.h>

// Terminal dimensions
int ROWS = 0;
int COLS = 0;

// Car position
int car_x = 0;
int car_y = 0;

// Car state
int direction = 1; // 1 for right, -1 for left
int speed = 1;

// Car design (multiple strings for easier visual representation)
const char *car_design[] = {
    "    ____",
    " __/  |_ \\_",
    "|  _     _``-.",
    "'-(_)---(_)--'"
};
#define CAR_HEIGHT 4
#define CAR_WIDTH 13

// Road variables
char **road = NULL;
char road_symbols[] = " .:|";
int road_bumps[20]; // Positions of road bumps
int num_bumps = 20;

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

void initialize_road() {
    // Allocate memory for the road
    road = malloc(ROWS * sizeof(char*));
    for (int i = 0; i < ROWS; i++) {
        road[i] = calloc(COLS + 1, sizeof(char));
    }
    
    // Generate random bumps
    for (int i = 0; i < num_bumps; i++) {
        road_bumps[i] = rand() % COLS;
    }
    
    // Set initial car position
    car_x = 0;
    car_y = ROWS - CAR_HEIGHT - 1;
}

void clear_screen() {
    printf("\033[H\033[J"); // ANSI escape sequence to clear the screen
}

void update_road() {
    // Clear the road (fill with spaces)
    for (int i = 0; i < ROWS; i++) {
        memset(road[i], ' ', COLS);
        road[i][COLS] = '\0';
    }
    
    // Draw road surface (bottom row)
    for (int j = 0; j < COLS; j++) {
        road[ROWS-1][j] = '=';
    }
    
    // Draw road markings
    for (int j = 0; j < COLS; j++) {
        if (j % 6 == 0) {
            road[ROWS-3][j] = '-';
        }
    }
    
    // Draw bumps
    for (int i = 0; i < num_bumps; i++) {
        int bump_pos = road_bumps[i];
        if (bump_pos >= 0 && bump_pos < COLS) {
            road[ROWS-2][bump_pos] = '^';
        }
        
        // Move bumps
        road_bumps[i] -= speed;
        if (road_bumps[i] < 0) {
            road_bumps[i] = COLS + (rand() % COLS);
        }
    }
    
    // Draw car
    for (int i = 0; i < CAR_HEIGHT; i++) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            if (car_x + j >= 0 && car_x + j < COLS && car_y + i >= 0 && car_y + i < ROWS) {
                if (j < strlen(car_design[i]) && car_design[i][j] != ' ') {
                    road[car_y + i][car_x + j] = car_design[i][j];
                }
            }
        }
    }
    
    // Move car
    if (direction > 0) {
        car_x += speed;
        if (car_x > COLS) {
            car_x = -CAR_WIDTH;
        }
    } else {
        car_x -= speed;
        if (car_x < -CAR_WIDTH) {
            car_x = COLS;
        }
    }
    
    // Make car bounce a bit when hitting a bump
    for (int i = 0; i < num_bumps; i++) {
        if (road_bumps[i] >= car_x && road_bumps[i] <= car_x + CAR_WIDTH) {
            car_y = ROWS - CAR_HEIGHT - 1 - (rand() % 2);
            return;
        }
    }
    
    // Normal car height
    car_y = ROWS - CAR_HEIGHT - 1;
}

void draw_road() {
    clear_screen();
    
    // Add a sky with stars
    for (int i = 0; i < ROWS - CAR_HEIGHT - 2; i++) {
        for (int j = 0; j < COLS; j++) {
            if (rand() % 100 < 1) {
                printf("\033[1;33m*\033[0m"); // Yellow star
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    
    // Draw the road with car
    for (int i = ROWS - CAR_HEIGHT - 2; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            char c = road[i][j];
            
            // Color coding
            if (c == '=' || c == '-' || c == '^') {
                printf("\033[1;37m%c\033[0m", c); // White for road markings
            } else if (c != ' ' && i >= car_y && i < car_y + CAR_HEIGHT && 
                      j >= car_x && j < car_x + CAR_WIDTH) {
                printf("\033[1;31m%c\033[0m", c); // Red for car
            } else {
                printf("%c", c);
            }
        }
        printf("\n");
    }
    
    // Instructions
    printf("\033[1;36mPress Ctrl+C to exit | Use space bar to change direction\033[0m");
}

void free_memory() {
    if (road) {
        for (int i = 0; i < ROWS; i++) {
            if (road[i]) {
                free(road[i]);
            }
        }
        free(road);
    }
}

int main() {
    srand(time(NULL));
    
    // Get terminal size
    get_terminal_size();
    
    // Initialize the road
    initialize_road();
    
    // Main loop
    while (1) {
        // Update car position and road
        update_road();
        
        // Draw everything
        draw_road();
        
        // Check for input (non-blocking)
        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        
        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0) {
            char c = getchar();
            if (c == ' ') {
                direction *= -1; // Change direction
            } else if (c == '+') {
                speed++; // Increase speed
                if (speed > 5) speed = 5;
            } else if (c == '-') {
                speed--; // Decrease speed
                if (speed < 1) speed = 1;
            }
        }
        
        // Sleep to control animation speed
        usleep(100000); // 100ms
    }
    
    free_memory();
    return 0;
}
