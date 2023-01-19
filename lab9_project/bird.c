#include "bird.h"
#include "display.h"
#include "flappy_bird_config.h"
#include "touchscreen.h"
#include <stdint.h>
#include <stdio.h>

// Global Variables
static uint16_t gravity;

// Reset Birds position, set state to falling
void bird_init(bird_t *player_bird) {
  player_bird->x_center = BIRD_START_WIDTH;
  player_bird->y_center = BIRD_START_HEIGHT;
  player_bird->flying = true;
  player_bird->current_state = FALLING;
  gravity = RESET;
  display_fillCircle(player_bird->x_center, player_bird->y_center, BIRD_RADIUS,
                     BIRD_COLOR);
}

void bird_tick(bird_t *player_bird) {

  ///////////////////////
  // State transitions //
  ///////////////////////

  switch (player_bird->current_state) {

  // Always falling unless screen is touched
  case (FALLING):
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      player_bird->current_state = JUMP;
    }

    else if (player_bird->flying == false) {
      player_bird->current_state = DEAD;
    }
    break;

  // Jump once then imedietly start falling
  case (JUMP):
    player_bird->current_state = FALLING;
    break;

  // Only enter this state from Game control
  case (DEAD):
    break;
  }

  ///////////////////
  // State Actions //
  ///////////////////

  switch (player_bird->current_state) {

  // Increase falling rate by one per tick
  // Redraw the bird each tick
  case (FALLING):
    gravity++;
    display_fillCircle(player_bird->x_center, player_bird->y_center,
                       BIRD_RADIUS, DISPLAY_BLACK);
    player_bird->y_center += gravity;
    display_fillCircle(player_bird->x_center, player_bird->y_center,
                       BIRD_RADIUS, BIRD_COLOR);
    break;

  // Reset gravity, jump once, redraw the bird each tick
  case (JUMP):
    gravity = RESET_JUMP_GRAVITY;
    touchscreen_ack_touch();
    display_fillCircle(player_bird->x_center, player_bird->y_center,
                       BIRD_RADIUS, DISPLAY_BLACK);
    player_bird->y_center -= JUMP_HEIGHT;
    display_fillCircle(player_bird->x_center, player_bird->y_center,
                       BIRD_RADIUS, BIRD_COLOR);
    break;

  // Pause all movement, set flight to false
  case (DEAD):
    player_bird->flying = false;
    break;
  }
}

// Return bird location
point bird_get_location(bird_t *player_bird) {
  point bird_point = {player_bird->x_center, player_bird->y_center};
  return bird_point;
}

// erase bird from screen
void erase_bird(bird_t *player_bird) {
  display_fillCircle(player_bird->x_center, player_bird->y_center, BIRD_RADIUS,
                     DISPLAY_BLACK);
}