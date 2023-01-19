#ifndef BIRD
#define BIRD

#include "display.h"
#include <stdint.h>
#include <stdio.h>

// Bird States
typedef enum { FALLING, JUMP, DEAD } BIRD_STATES;

// Bird Struct
typedef struct bird {
  uint16_t x_center;
  uint16_t y_center;
  bool flying;
  BIRD_STATES current_state;
} bird_t;

// (X,Y) point used to return bird location
typedef struct point {
  uint16_t x;
  uint16_t y;
} point;

// Reset birds location to middle left screen
void bird_init(bird_t *player_bird);

// Controls the birds falling and jumping actions
void bird_tick(bird_t *player_bird);

// Returns birds (X,Y) location
point bird_get_location(bird_t *player_bird);

// Returns boolean of wether or not bird is dead
bool is_flying(bird_t *player_bird);

// Erases birds graphics
void erase_bird(bird_t *bird);

#endif /* BIRD */