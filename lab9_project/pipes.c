#include "pipes.h"
#include "display.h"
#include "flappy_bird_config.h"

// Global Variables
static uint16_t pipe_speed = 5;

// Location of top right corner of pipe
typedef struct point {
  uint16_t x;
  uint16_t y;
} point_t;

// Reset pipe to left half of screen
void init_pipe(pipes_t *pipe) {
  pipe->currentState = MOVE;
  pipe->height_top =
      ((DISPLAY_HEIGHT - (rand() % (DISPLAY_HEIGHT - PIPES_GAP))) - PIPES_GAP);
  pipe->hieght_lower = pipe->height_top + PIPES_GAP;
  pipe->width = PIPES_WIDTH;
  pipe->x_current = DISPLAY_WIDTH;
  pipe->is_moving = true;
  pipe_speed = pipe_speed;
}

void tick_pipes(pipes_t *pipe) {

  ///////////////////////
  // State transitions //
  ///////////////////////

  switch (pipe->currentState) {
  // Imedietly switch to moving state
  case (INIT):
    pipe->currentState = MOVE;
    break;

  // If pipe is off left side of screen swithc to stop
  case (MOVE):
    if (pipe->x_current <= RESET - PIPES_WIDTH) {
      pipe->currentState = STOP;
    }
    break;

  // Do nothing
  case (STOP):
    break;
  }

  ///////////////////
  // State actions //
  ///////////////////

  switch (pipe->currentState) {

  // Init the pipes
  case (INIT):
    init_pipe(pipe);
    break;

  // Erase and draw shifted pipe
  case (MOVE):
    display_fillRect(pipe->x_current, RESET, PIPES_WIDTH, pipe->height_top,
                     DISPLAY_BLACK);
    display_fillRect(pipe->x_current, pipe->hieght_lower, PIPES_WIDTH,
                     DISPLAY_HEIGHT - pipe->hieght_lower, DISPLAY_BLACK);

    pipe->x_current -= pipe_speed;

    display_fillRect(pipe->x_current, RESET, PIPES_WIDTH, pipe->height_top,
                     DISPLAY_GREEN);
    display_fillRect(pipe->x_current, pipe->hieght_lower, PIPES_WIDTH,
                     DISPLAY_HEIGHT - pipe->hieght_lower, DISPLAY_GREEN);
    break;

  // Stop movement of pipe
  case (STOP):
    pipe->is_moving = false;
    break;
  }
}

// Return bool if pipe is moving
bool is_pipe_moving(pipes_t *pipe) { return pipe->is_moving; }

// Erase pipe from screen
void erase_pipe(pipes_t *pipe) {
  display_fillRect(pipe->x_current, RESET, PIPES_WIDTH, pipe->height_top,
                   DISPLAY_BLACK);
  display_fillRect(pipe->x_current, pipe->hieght_lower, PIPES_WIDTH,
                   DISPLAY_HEIGHT - pipe->hieght_lower, DISPLAY_BLACK);
}

// Change rate at which pipes move across the screen
void change_speed(uint16_t speed) { pipe_speed = speed; }
