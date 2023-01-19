#include "plane.h"
#include "config.h"
#include "display.h"
#include "missile.h"
#include <stdint.h>
#include <stdio.h>

// Plane Structure
#define PLANE_HEIGHT DISPLAY_HEIGHT / 6
#define PLANE_LENGTH 20
#define WING_WIDTH 6

#define RESET 0

typedef enum {
  INITIALIZING,
  FLYING,
  ERASE,
  DEAD,
} plane_tick_t;

// Structure of plane
typedef struct {
  plane_tick_t currentState;
  int16_t origin_x;
  int16_t origin_y;
  int16_t current_x;
  int16_t current_y;
  int16_t current_back;
  int16_t destination_x;
  int16_t destintation_y;
  int16_t back_right;
  int16_t back_left;
  int16_t back;
  double total_length;
  double length;
} plane_t;

// Global Variables
plane_t plane;
bool erased = false;

// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {
  plane.origin_x = DISPLAY_WIDTH;
  plane.origin_y = PLANE_HEIGHT;
  plane.current_x = plane.origin_x;
  plane.current_y = plane.origin_y;
  plane.back = plane.origin_x + PLANE_LENGTH;
  plane.back_right = plane.origin_y + WING_WIDTH;
  plane.back_left = plane.origin_y - WING_WIDTH;
  plane.current_back = plane.back;
  plane.destintation_y = PLANE_HEIGHT;
  plane.destination_x = -PLANE_LENGTH;
  plane.total_length = DISPLAY_WIDTH + PLANE_LENGTH;
  plane.length = RESET;
  plane.currentState = INITIALIZING;
}

// Erase old plane, advances parameters, draws new plane
void plane_advance() {
  display_fillTriangle(plane.current_x, plane.current_y, plane.current_back,
                       plane.back_right, plane.current_back, plane.back_left,
                       DISPLAY_BLACK);

  // plane.length = plane.length + CONFIG_PLANE_DISTANCE_PER_TICK;

  plane.current_x -= CONFIG_PLANE_DISTANCE_PER_TICK;

  plane.current_back -= CONFIG_PLANE_DISTANCE_PER_TICK;

  display_fillTriangle(plane.current_x, plane.current_y, plane.current_back,
                       plane.back_right, plane.current_back, plane.back_left,
                       DISPLAY_WHITE);
}

// State machine tick function
void plane_tick() {

  // State Transitions
  switch (plane.currentState) {

  // Jump straight to flying
  case (INITIALIZING):
    plane.currentState = FLYING;
    break;

  // Fly until length = total length, else advance
  case (FLYING):
    if (plane.current_back < RESET) {
      plane.currentState = ERASE;
    }
    break;

  // Once erase enter dead state
  case (ERASE):
    if (erased) {
      plane.currentState = DEAD;
    }
    break;

  // Do nothing
  case (DEAD):
    break;
  }

  // State Actions
  switch (plane.currentState) {

  // Do nothing (initialized in controls)
  case (INITIALIZING):
    break;

  // Advance plane, if past half way drop bomb
  case (FLYING):
    plane_advance(plane);
    break;

  // Erase triangle set erased to ture
  case (ERASE):
    display_fillTriangle(plane.current_x, plane.current_y, plane.current_back,
                         plane.back_right, plane.current_back, plane.back_left,
                         DISPLAY_BLACK);
    erased = true;
    break;

  // Reset boolean values
  case (DEAD):
    erased = false;
    plane.current_x = plane.origin_x;
    plane.current_y = plane.origin_y;
    break;
  }
}

// Trigger the plane to explode
void plane_explode() { plane.currentState = ERASE; }

// Get the XY location of the plane
display_point_t plane_getXY() {
  static display_point_t point;
  point.x = plane.current_x;
  point.y = plane.current_y;
  return point;
}