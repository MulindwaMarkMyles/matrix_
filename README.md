# Matrix Rain Animation

A terminal-based implementation of the iconic "Matrix digital rain" animation effect, written in C.

![Matrix Rain Animation](matrix_preview.gif)

## Features

- Terminal-based animation that adapts to your terminal size
- Dynamic resizing - automatically adjusts if you resize your terminal
- Customizable animation speed and character set
- Memory-efficient implementation
- Pure C implementation with minimal dependencies

## Requirements

- C compiler (gcc, clang)
- POSIX-compliant system (Linux, macOS, or other Unix-like OS)
- Terminal that supports ANSI escape sequences

## Installation

Clone the repository and compile the source code:

```bash
git clone https://github.com/MulindwaMarkMyles/matrix-rain.git
cd matrix-rain
gcc -o matrix matrix.c
```

## Usage

Simply run the compiled executable:

```bash
./matrix
```

To exit, press `Ctrl+C`.

## Customization

You can modify the following constants in `matrix.c` to customize the animation:

- `DROPS`: Adjust the density of the rain effect
- Change the `signs[]` array to use different characters
- Modify the `usleep()` value to change animation speed

## Terminal Car Animation

This repository also includes a terminal-based car animation (`car.c`) that displays an ASCII car driving across your terminal screen.

![Car Animation](car_preview.gif)

### Features

- Animated car with road and sky
- Dynamic resizing based on terminal dimensions
- Obstacles (bumps) that make the car bounce
- Interactive controls
- Starry night sky background

### Compiling and Running the Car Animation

```bash
gcc -o car car.c
./car
```

### Controls

- **Space bar**: Change the car's direction
- **+**: Increase speed
- **-**: Decrease speed
- **Ctrl+C**: Exit the program

## How It Works

### Matrix Animation

The program uses a 2D character array to represent the terminal screen. Each animation frame:

1. Updates character positions (moving older characters down)
2. Generates new characters at the top row
3. Creates trails by randomly removing characters
4. Draws the updated state to the terminal

The program continuously checks for terminal size changes and adjusts accordingly.

### Car Animation

The car animation uses similar rendering techniques but focuses on horizontal movement:

1. Clears the screen and draws a starry background
2. Updates the road and obstacles (bumps)
3. Draws the ASCII car at its current position
4. Handles user input for changing direction and speed
5. Simulates car bouncing when hitting road bumps
