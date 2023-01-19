#include "missile.h"
#include "config.h"
#include "display.h"
#include "plane.h"
#include "touchscreen.h"
#include <math.h>
#include <stdio.h>
////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function, our missile
// will have different initializers depending on the missile type.

#define RESET 0
#define ENEMY_START_HEIGHT (DISPLAY_HEIGHT / 4)
#define SQUARED 2
#define LEFT_RANGE DISPLAY_WIDTH / 3
#define MIDDLE_RANGE (DISPLAY_WIDTH / 3) * 2
#define LEFT_BASE DISPLAY_WIDTH / 6
#define MIDDLE_BASE DISPLAY_WIDTH / 2
#define RIGHT_BASE (DISPLAY_WIDTH / 6) * 5
#define BASE_HEIGHT DISPLAY_HEIGHT - 7
#define MAX_RADIUS 15
#define INITIALIZING_ST_MSG "INITIALIZE STATE\n\r"
#define FLYING_ST_MSG "FLYING STATE\n\r"
#define EXPLODING_GROWING_ST_MSG "EPLODING_GROWING STATE\n\r"
#define EXPLODING_SHRINKING_ST_MSG "EXPLODING_SHRINKING STATE\n\r"
#define DEAD_ST_MSG "DEAD STATE\n\r"
#define DOUBLE_SPEED 2

// missle_tick states
typedef enum {
  INITIALIZING,
  FLYING,
  EXPLODING_GROWING,
  EXPLODING_SHRINKING,
  DEAD,
} missile_tick_t;

// Resets all exploding parameters, current locations and solves for lengths
void missile_init_general(missile_t *missile) {
  // Total flight lengthe base on start and end point
  missile->total_length = sqrt(((missile->x_dest - missile->x_origin) *
                                (missile->x_dest - missile->x_origin)) +
                               ((missile->y_dest - missile->y_origin) *
                                (missile->y_dest - missile->y_origin)));

  // Reset current locations to new origins and Reset exploding parameters
  missile->x_current = missile->x_origin;
  missile->y_current = missile->y_origin;
  missile->length = RESET;
  missile->radius = RESET;
  missile->explode_me = false;
  missile->impacted = false;
}

void missile_flight_advance(missile_t *missile) {
  if (missile->type == MISSILE_TYPE_ENEMY ||
      missile->type == MISSILE_TYPE_PLANE) {
    missile->length = missile->length +
                      (CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK * DOUBLE_SPEED);
  }

  else if (missile->type == MISSILE_TYPE_PLAYER) {
    missile->length = missile->length +
                      (CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK * DOUBLE_SPEED);
  }

  missile->x_current =
      missile->x_origin + ((missile->length / missile->total_length) *
                           (missile->x_dest - missile->x_origin));

  missile->y_current =
      missile->y_origin + ((missile->length / missile->total_length) *
                           (missile->y_dest - missile->y_origin));
}

/// Initialize each missle as dead
void missile_init_dead(missile_t *missile) { missile->currentState = DEAD; }

// Initialize enemy missles, travel from top quarter to the very bottom
void missile_init_enemy(missile_t *missile) {

  // Type, state, start and end locations
  missile->type = MISSILE_TYPE_ENEMY;
  missile->currentState = INITIALIZING;
  missile->x_origin = rand() % DISPLAY_WIDTH;
  missile->y_origin = rand() % ENEMY_START_HEIGHT;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;

  // sets other general parameters
  missile_init_general(missile);
}

// Based on players tough, select closest base as origin and touch as final
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {
  missile->type = MISSILE_TYPE_PLAYER;
  missile->currentState = INITIALIZING;

  // Find closest base to players touch to select missile start point
  if (x_dest < LEFT_RANGE) {
    missile->x_origin = LEFT_BASE;
  }

  else if (x_dest < MIDDLE_RANGE) {
    missile->x_origin = MIDDLE_BASE;
  }

  else {
    missile->x_origin = RIGHT_BASE;
  }
  missile->y_origin = BASE_HEIGHT;
  missile->x_dest = x_dest;
  missile->y_dest = y_dest;

  // sets other general parameters
  missile_init_general(missile);
}

// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {
  missile->type = MISSILE_TYPE_PLANE;
  missile->currentState = INITIALIZING;
  missile->x_origin = plane_x;
  missile->y_origin = plane_y;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;
  missile_init_general(missile);
}

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile) {
  // Print debugging message
  // debugStatePrint(missile);

  // State Transitiosn
  switch (missile->currentState) {

  // Imedietly transitions to flying
  case (INITIALIZING):
    missile->currentState = FLYING;
    break;

  // Checks to see if Missles have completed their flight, if so erase flight
  // Enemy Missles simply dissapear and enter dead state,
  // player missles enter explode_growing state
  case (FLYING):
    // Erase flight path
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, DISPLAY_BLACK);

    // Check for completed flight
    if (missile->length >= missile->total_length) {

      // Player missles ener Exploding state, enemies enter dead state
      if (missile->type == MISSILE_TYPE_PLAYER) {
        missile->currentState = EXPLODING_GROWING;
      }

      else {
        missile->currentState = DEAD;
        missile->impacted = true;
      }
    }

    // Remain in flight state if end of flight not achieved
    else {
      missile->currentState = FLYING;
    }
    break;

  // Continue to explode until max radius achieved then switch to shrinking
  case (EXPLODING_GROWING):
    if (missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS)
      missile->currentState = EXPLODING_SHRINKING;
    else
      missile->currentState = EXPLODING_GROWING;
    break;

  // Continue to shrink until less than 0, then enter Dead state
  case (EXPLODING_SHRINKING):
    if (missile->radius <= RESET) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_BLACK);
      missile->currentState = DEAD;
      missile->explode_me = false;
    }

    else {
      missile->currentState = EXPLODING_SHRINKING;
    }
    break;

  // Do nothing, remain in this state until re-anitialize
  case (DEAD):
    break;
  }

  // State Actions
  switch (missile->currentState) {
  // In this lab main is initializing but in the future will use this state
  case (INITIALIZING):

    break;

  // Draw out missle path
  case (FLYING):
    // Advances missel based on CONFIG Tick speed
    missile_flight_advance(missile);

    // Draw the line of flight, red for enemy, green for player
    if (missile->type == MISSILE_TYPE_ENEMY) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_RED);
    }

    else if (missile->type == MISSILE_TYPE_PLAYER) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_GREEN);
    }

    else if (missile->type == MISSILE_TYPE_PLANE) {
      display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                       missile->y_current, DISPLAY_BLUE);
    }
    break;

  // Increase explosion by increasing radius of circle each tick
  case (EXPLODING_GROWING):
    missile->explode_me = true;
    missile->radius = missile->radius +
                      (CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK * DOUBLE_SPEED);
    if (missile->type == MISSILE_TYPE_ENEMY) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_RED);
    }

    else if (missile->type == MISSILE_TYPE_PLAYER) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_GREEN);
    }

    else if (missile->type == MISSILE_TYPE_PLANE) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_BLUE);
    }
    break;

  // Decrease explosion be erasing old circle and redrawing smaller circle
  case (EXPLODING_SHRINKING):
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       DISPLAY_BLACK);
    missile->radius = missile->radius -
                      (CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK * DOUBLE_SPEED);
    if (missile->type == MISSILE_TYPE_ENEMY) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_RED);
    }

    else if (missile->type == MISSILE_TYPE_PLAYER) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_GREEN);
    }

    else if (missile->type == MISSILE_TYPE_PLANE) {
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_BLUE);
    }
    break;

  // Do nothing
  case (DEAD):
    break;
  }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  if (missile->currentState == DEAD)
    return true;
  else
    return false;
}

// Return whether the given missile is exploding.
bool missile_is_exploding(missile_t *missile) {
  if (missile->explode_me)
    return true;
  else
    return false;
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {
  if (missile->currentState == FLYING)
    return true;
  else
    return false;
}

// Used to indicate that a flying missile should be detonated.
void missile_trigger_explosion(missile_t *missile) {
  display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                   missile->y_current, DISPLAY_BLACK);
  missile->currentState = EXPLODING_GROWING;
}
