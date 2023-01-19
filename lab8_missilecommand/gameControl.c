#include "gameControl.h"
#include "config.h"
#include "display.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"
#include <stdint.h>
#include <stdio.h>

// Define different levels and their quantity
#define RESET 0
#define LEVEL_ONE 1
#define LEVEL_ONE_ADVANCE 2
#define LEVEL_TWO 2
#define LEVEL_TWO_ADVANCE 3
#define LEVEL_THREE 3
#define LEVEL_THREE_ADVANCE 5
#define LEVEL_FOUR 4
#define LEVEL_FOUR_ADVANCE 7
#define LEVEL_FIVE 5
#define LEVEL_FIVE_ADVANCE 9
#define LEVEL_SIX 6
#define LEVEL_SIX_ADVANCE 12
#define LEVEL_SEVEN 7

// Cursor Positions, msg size
#define IMPACTED_MSG_LOCATION (DISPLAY_WIDTH * .6)
#define SHOT_MSG_LOCATION (DISPLAY_WIDTH * .15)
#define SHOT_MSG "SHOT: %d"
#define IMPACTED_MSG "IMPACTED: %d"
#define MSG_SIZE 20
#define PST_MISSILE_CNT (missiles_missed - 1)
#define PST_SHOT_CNT (player_missiles_launched - 1)

// Pointer Locations
#define PLAYER_PTR (CONFIG_MAX_ENEMY_MISSILES + 1)
#define ENEMY_PTR 0
#define PLANE_PTR CONFIG_MAX_ENEMY_MISSILES
#define HALF_TOTAL_MISSLES 6

// Plane
#define PLANE_LAUNCH_CNT 300
#define TOTAL_BAD_MISSLES                                                      \
  (CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLANE_MISSILES)
#define PLANE_BOMB_ZONE (DISPLAY_WIDTH / 2)

// Bases
#define LEFT_BASE_ORIGIN (DISPLAY_WIDTH / 6)
#define MIDDLE_BASE_ORIGIN (DISPLAY_WIDTH / 2)
#define RIGHT_BASE_ORIGIN ((DISPLAY_WIDTH / 6) * 5)
#define LARGE_BASE_HEIGHT (DISPLAY_HEIGHT - 5)
#define SMALL_BASE_HEIGHT (LARGE_BASE_HEIGHT - 2)
#define LARGE_BASE_LENGTH 30
#define SMALL_BASE_LENGTH 10
#define LEFT_BASE_X (LEFT_BASE_ORIGIN - 15)
#define LEFT_BASE_X2 (LEFT_BASE_ORIGIN - 5)
#define MIDDLE_BASE_X (MIDDLE_BASE_ORIGIN - 15)
#define MIDDLE_BASE_X2 (MIDDLE_BASE_ORIGIN - 5)
#define RIGHT_BASE_X (RIGHT_BASE_ORIGIN - 15)
#define RIGHT_BASE_X2 (RIGHT_BASE_ORIGIN - 5)
#define GUN_HEIGHT (BASE_HEIGHT - 2)

//////////////////////
// Global Variables //
//////////////////////

// Missiles
static missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];
static missile_t *enemy_missiles = &(missiles[RESET]);
static missile_t *player_missiles = &(missiles[PLAYER_PTR]);

// Counters
static uint16_t enemy_missiles_launched;
static uint16_t player_missiles_launched;
static uint16_t missiles_missed;
static uint16_t level;
static uint16_t plane_count;

// Data changed trackers
static bool move_enemy = false;
static bool player_missiles_changed = true;

// Stat Messages
static char shot_message[MSG_SIZE];
static char impacted_message[MSG_SIZE];

// States from missisle.c here just to be referenced
typedef enum {
  INITIALIZING,
  FLYING,
  EXPLODING_GROWING,
  EXPLODING_SHRINKING,
  DEAD,
} missile_tick_t;

///////////
// STATS //
///////////

void updateStats() {
  // Delete old shot message
  display_setCursor(SHOT_MSG_LOCATION, RESET);
  display_setTextColor(DISPLAY_BLACK);
  sprintf(shot_message, SHOT_MSG, PST_SHOT_CNT);
  display_print(shot_message);

  // Write updated shot message
  display_setCursor(SHOT_MSG_LOCATION, RESET);
  display_setTextColor(DISPLAY_WHITE);
  sprintf(shot_message, SHOT_MSG, player_missiles_launched);
  display_print(shot_message);

  // Erase previous Impacted message
  display_setCursor(IMPACTED_MSG_LOCATION, RESET);
  display_setTextColor(DISPLAY_BLACK);
  sprintf(impacted_message, IMPACTED_MSG, PST_MISSILE_CNT);
  display_print(impacted_message);

  // Write updated impacted message
  display_setCursor(IMPACTED_MSG_LOCATION, RESET);
  display_setTextColor(DISPLAY_WHITE);
  sprintf(impacted_message, IMPACTED_MSG, missiles_missed);
  display_print(impacted_message);
}

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {

  // initialize all planes as dead
  for (uint16_t i = RESET; i < CONFIG_MAX_TOTAL_MISSILES; i++)
    missile_init_dead(&missiles[i]);

  // Initialize variables, screen and update stats
  missiles_missed = RESET;
  player_missiles_launched = RESET;
  enemy_missiles_launched = RESET;
  plane_count = PLANE_LAUNCH_CNT;
  display_fillScreen(DISPLAY_BLACK);
  display_setTextColor(DISPLAY_WHITE);
  updateStats();

  // Draw Bases
  display_fillRect(LEFT_BASE_X, LARGE_BASE_HEIGHT, LARGE_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
  display_fillRect(LEFT_BASE_X2, (SMALL_BASE_HEIGHT), SMALL_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
  display_fillRect(MIDDLE_BASE_X, LARGE_BASE_HEIGHT, LARGE_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
  display_fillRect(MIDDLE_BASE_X2, (SMALL_BASE_HEIGHT), SMALL_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
  display_fillRect(RIGHT_BASE_X, LARGE_BASE_HEIGHT, LARGE_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
  display_fillRect(RIGHT_BASE_X2, (SMALL_BASE_HEIGHT), SMALL_BASE_LENGTH,
                   LARGE_BASE_HEIGHT, DISPLAY_GREEN);
}

// Ticks all missiles, increases difficulty, triggers explosions
void gameControl_tick() {

  ////////////
  // LEVELs //
  ////////////

  // Increase difficulty as missles launch count increases
  if (enemy_missiles_launched < LEVEL_ONE_ADVANCE)
    level = LEVEL_ONE;
  else if (enemy_missiles_launched < LEVEL_TWO_ADVANCE)
    level = LEVEL_TWO;
  else if (enemy_missiles_launched < LEVEL_THREE_ADVANCE)
    level = LEVEL_THREE;
  else if (enemy_missiles_launched < LEVEL_FOUR_ADVANCE)
    level = LEVEL_FOUR;
  else if (enemy_missiles_launched < LEVEL_FIVE_ADVANCE)
    level = LEVEL_FIVE;
  else if (enemy_missiles_launched < LEVEL_SIX_ADVANCE)
    level = LEVEL_SIX;
  else
    level = LEVEL_SEVEN;

  //////////////
  // MISSILES //
  //////////////

  // Initialises enemies based on current level
  for (uint16_t i = RESET; i < level; ++i) {
    // Activate any of current level that are dead
    if (missiles[i].currentState == DEAD) {
      missile_init_enemy(&missiles[i]);
      enemy_missiles_launched++;
    }
  }

  // Check for touch to send player missles, limited to 4 missles
  for (uint16_t i = RESET; i < CONFIG_MAX_PLAYER_MISSILES; ++i) {

    // check if any player missles are dead
    if (player_missiles[i].currentState == DEAD) {

      // check to see if touch occured, if so activate missle
      // Increase Player missile count
      if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
        touchscreen_ack_touch();
        missile_init_player(&player_missiles[i], touchscreen_get_location().x,
                            touchscreen_get_location().y);
        player_missiles_launched++;
        updateStats();
      }
      break;
    }
  }

  // Tick first half of missles, then 2nd half of missles
  if (move_enemy) {
    for (uint16_t i = RESET; i < HALF_TOTAL_MISSLES; ++i) {
      missile_tick(&missiles[i]);
      if ((missiles[i].impacted == true) &&
          (missiles[i].type != MISSILE_TYPE_PLAYER)) {
        missiles_missed++;
        missiles[i].impacted = false;
        // Update Stats
        updateStats();
      }
    }
  }

  else {
    for (uint16_t i = HALF_TOTAL_MISSLES; i < CONFIG_MAX_TOTAL_MISSILES; ++i) {
      missile_tick(&missiles[i]);
      if ((missiles[i].impacted == true) &&
          (missiles[i].type != MISSILE_TYPE_PLAYER)) {
        missiles_missed++;
        missiles[i].impacted = false;
        // Update Stats
        updateStats();
      }
    }
  }

  // ALternate whose turn it is to move
  move_enemy = !move_enemy;

  // Check to see if enemy missle has entered explosion, if so detonate enemy
  for (uint16_t i = RESET; i < TOTAL_BAD_MISSLES; ++i) {
    for (uint16_t j = RESET; j < CONFIG_MAX_TOTAL_MISSILES; ++j) {

      // If enemy missle isn't active then skip
      if (!missile_is_flying(&missiles[i])) {
        continue;
      }
      // If player missle isn't exploding then skip
      if (missiles[j].explode_me == false) {
        continue;
      }

      // If enemy missle is in radius of any explosion, explode enemy
      if (((missiles[i].x_current - missiles[j].x_current) *
           (missiles[i].x_current - missiles[j].x_current)) +
              ((missiles[i].y_current - missiles[j].y_current) *
               (missiles[i].y_current - missiles[j].y_current)) <
          (missiles[j].radius * missiles[j].radius)) {
        missile_trigger_explosion(&missiles[i]);
      }

      // If plane is in radius of explosion, explode plane,
      // check with 1st missile
      if (i == RESET) {
        if (((plane_getXY().x - missiles[j].x_current) *
             (plane_getXY().x - missiles[j].x_current)) +
                ((plane_getXY().y - missiles[j].y_current) *
                 (plane_getXY().y - missiles[j].y_current)) <
            (missiles[j].radius * missiles[j].radius)) {
          plane_explode();
        }
      }
    }
  }

  ///////////
  // PLANE //
  ///////////

  // if plane count is less than start count, increment,
  // else launch plane reset count
  if (plane_count < PLANE_LAUNCH_CNT) {
    plane_count++;
  }

  else {
    plane_init(&missiles[CONFIG_MAX_ENEMY_MISSILES]);
    plane_count = RESET;
  }

  if ((plane_getXY().x < PLANE_BOMB_ZONE) &&
      (missiles[CONFIG_MAX_ENEMY_MISSILES].currentState == DEAD)) {
    missile_init_plane(&missiles[CONFIG_MAX_ENEMY_MISSILES], plane_getXY().x,
                       plane_getXY().y);
  }

  // Tick plane function
  plane_tick();
}
