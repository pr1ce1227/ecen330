#ifndef PIPES
#define PIPES

#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Pipe states
typedef enum { INIT, MOVE, STOP } PIPES_STATE_t;

// Pipe Struc
typedef struct pipe {
  uint16_t width;
  uint16_t height_top;
  uint16_t hieght_lower;
  int16_t x_current;
  PIPES_STATE_t currentState;
  bool is_moving;
} pipes_t;

// Reset to left side of the screen, set to move
void init_pipe(pipes_t *pipe);

// Control Pipe movement
void tick_pipes(pipes_t *pipe);

// Returns wether or not pipe is dead
bool is_pipe_moving(pipes_t *pipe);

// Remove pipe from screen
void erase_pipe(pipes_t *pipe);

// Adjust movement per tick
void change_speed(uint16_t speed);

#endif