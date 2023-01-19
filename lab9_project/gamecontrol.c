#include "gamecontrol.h"
#include "bird.h"
#include "drivers/buttons.h"
#include "drivers/switches.h"
#include "flappy_bird_config.h"
#include "pipes.h"
#include <math.h>

// Global Variables //

// Bird
static bird_t bird;

// score
static bool keep_score;
static char score[SCORE_TEXT_SIZE];
static uint16_t score_num;
static char High_score[SCORE_TEXT_SIZE];
static uint16_t High_score_num = RESET;

// Pipes
static pipes_t pipes[NUM_PIPES];
static uint16_t past_pipe;
static uint16_t pipe_spacing;
static uint16_t pipe_spacing_count;
static uint16_t pipe_count;
static uint8_t lead_pipe;

// Counters
static uint16_t END_GAME_COUNT;
static uint16_t INSTRUCTIONS_COUNT;

// Game control States
typedef enum { INSTRUCTIONS, RUN, END_GAME, STATS } GAME_CONTROL_STATES_T;

// Current State of game
static GAME_CONTROL_STATES_T currentState;

// Print functions
void print_countdown(uint16_t num);

void erase_countdown(uint16_t num);

// Reset global variables, initialize all pipes as dead
void init_gamecontrol() {
  pipe_spacing_count = RESET;
  pipe_count = RESET;
  lead_pipe = RESET;
  score_num = RESET;
  END_GAME_COUNT = RESET;
  INSTRUCTIONS_COUNT = RESET;
  pipe_spacing = BASE_PIPE_SPACING;
  past_pipe = WAITING_PIPE;
  keep_score = false;

  // Initialize all pipes as dead
  for (uint16_t i = RESET; i < NUM_PIPES; ++i) {
    pipes[i].currentState = STOP;
  }
}

void tick_gamecontrol() {

  // State transitions
  switch (currentState) {

  case (INSTRUCTIONS):

    // Display instructions wait 5 seconds, move to Run init Bird
    if (INSTRUCTIONS_COUNT > TWELVE_SECONDS) {
      currentState = RUN;
      bird_init(&bird);
    }
    break;

  case (RUN):

    // Don't keep score until all pipes are initialized, this is to help with
    // resetting the game else, bird can appear in middle of erased pipe
    if ((pipes[PIPE_ONE].currentState == MOVE) &&
        (pipes[PIPE_TWO].currentState == MOVE) &&
        (pipes[PIPE_THREE].currentState == MOVE)) {
      keep_score = true;
    }

    // Establish which pipe is in the lead to check for impact
    if (keep_score) {
      if (pipes[PIPE_ONE].currentState == STOP) {
        lead_pipe = PIPE_TWO;
      } else if (pipes[PIPE_TWO].currentState == STOP) {
        lead_pipe = PIPE_THREE;
      } else if (pipes[PIPE_THREE].currentState == STOP) {
        lead_pipe = PIPE_ONE;
      }

      // If lead pipe has passed center of bird increase score
      // Set lead pipe = to past pipe to avoid double counting score
      if (pipes[lead_pipe].x_current < (bird.x_center - BIRD_RADIUS) &&
          lead_pipe != past_pipe) {
        score_num++;
        past_pipe = lead_pipe;
      }
    }

    // Verify lead pipe is moving,
    // Check for impact of Bird with lead Pipe or ground
    if (is_pipe_moving(&pipes[lead_pipe])) {

      // Ground Collision Check
      if ((bird.y_center + BIRD_RADIUS) >= DISPLAY_HEIGHT) {
        bird.current_state = DEAD;
        currentState = END_GAME;
      }

      // Check if bird is horizontally alligned with lead pipe
      if ((bird.x_center + BIRD_RADIUS >= pipes[lead_pipe].x_current) &&
          (bird.x_center - BIRD_RADIUS <=
           pipes[lead_pipe].x_current + PIPES_WIDTH)) {

        // If bird is alligned with pipe from above and not inside gap then bird
        // dies, End the game.
        if (((bird.y_center - BIRD_RADIUS) <= pipes[lead_pipe].height_top) ||
            ((bird.y_center + BIRD_RADIUS) >= pipes[lead_pipe].hieght_lower)) {
          bird.current_state = DEAD;
          currentState = END_GAME;
        }
      }
    }

    break;

  case (END_GAME):

    // Pause game for couple second to let user see where they died
    // Then erase Bird and all the pipes, switch to stats
    if (END_GAME_COUNT >= TWO_SECONDS) {
      erase_bird(&bird);

      // Erase Pipes, set to dead
      for (uint16_t i = RESET; i < NUM_PIPES; ++i) {
        erase_pipe(&pipes[i]);
        pipes[i].currentState = DEAD;
      }

      currentState = STATS;
    }
    break;

  case (STATS):

    // If button 0 is pressed erase all of stats and switch to instructions
    if (buttons_read() == BUTTON_0_ON) {

      // Erase score stat on top of screen
      display_setTextSize(SMALL_TEXT);
      display_setCursor(SCORE_MSG_LOCATION, RESET);
      display_setTextColor(DISPLAY_BLACK);
      sprintf(score, SCORE_MSG, score_num);
      display_print(score);

      // Erase large score stat in middle of screen
      display_setCursor(FINAL_SCORE_OFFSET, MIDDLE_HEIGHT);
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(MEDIUM_TEXT);
      display_print(score);

      // Erase High score of the game
      display_print("\n\r\n\r      ");
      display_print(High_score);
      display_setTextSize(SMALL_TEXT);
      display_print("\n\r\n\r\n\r            ");
      display_print("Press Button 0 to restart");

      // Re-Initialize game control
      init_gamecontrol();
      currentState = INSTRUCTIONS;
    }
    break;
  }

  // State Actions
  switch (currentState) {

  // Print instructions to screen
  case (INSTRUCTIONS):

    // If switch 0 is flipped, change speed of game
    if (switches_read() >= SWITCH_0_ON) {
      change_speed(FAST_SPEED);
      pipe_spacing = FAST_PIPE_SPACING;
    }

    else {
      change_speed(BASE_SPEED);
      pipe_spacing = BASE_PIPE_SPACING;
    }

    // Output main instructions screen
    if (INSTRUCTIONS_COUNT == RESET) {
      display_setCursor(MAIN_INSTRUCTIONS_OFFSET, MIDDLE_HEIGHT);
      display_setTextColor(DISPLAY_WHITE);
      display_setTextSize(2);
      display_print("WELCOME TO FLAPPY BIRD\n\r\n\r");
      display_setTextSize(1);
      display_print(
          "               - Tap to Jump and avoid pipes\n\r \n\r          "
          "     - Flip Switch 0 to speed up ");
    }

    // Erase instructions screen after 7 seconds
    else if (INSTRUCTIONS_COUNT == SEVEN_SECONDS) {
      display_setCursor(MAIN_INSTRUCTIONS_OFFSET, MIDDLE_HEIGHT);
      display_setTextColor(DISPLAY_BLACK);
      display_setTextSize(MEDIUM_TEXT);
      display_print("WELCOME TO FLAPPY BIRD\n\r\n\r");
      display_setTextSize(SMALL_TEXT);
      display_print(
          "               - Tap to Jump and avoid pipes\n\r \n\r          "
          "     - Flip Switch 0 to speed up");
    }

    // Start countdown with 3
    else if (INSTRUCTIONS_COUNT == TIME_3_ON) {
      print_countdown(TIME_3);
    }

    // After 1 second erase 3
    else if (INSTRUCTIONS_COUNT == TIME_3_OFF) {
      erase_countdown(TIME_3);
    }

    // Draw 2
    else if (INSTRUCTIONS_COUNT == TIME_2_ON) {
      print_countdown(TIME_2);
    }

    // Erase 2
    else if (INSTRUCTIONS_COUNT == TIME_2_off) {
      erase_countdown(TIME_2);
    }

    // Draw 1
    else if (INSTRUCTIONS_COUNT == TIME_1_ON) {
      print_countdown(TIME_1);
    }

    // Erase 1
    else if (INSTRUCTIONS_COUNT == TIME_1_OFF) {
      erase_countdown(TIME_1);
    }

    // Increment Instructions counter
    INSTRUCTIONS_COUNT++;

    break;

  case (RUN):

    // Print Score
    // Delete old shot message
    display_setTextSize(SMALL_TEXT);
    display_setCursor(SCORE_MSG_LOCATION, RESET);
    display_setTextColor(DISPLAY_BLACK);
    sprintf(score, SCORE_MSG, PST_SCORE_CNT);
    display_print(score);

    // Write updated shot message
    display_setCursor(SCORE_MSG_LOCATION, RESET);
    display_setTextColor(DISPLAY_WHITE);
    sprintf(score, SCORE_MSG, score_num);
    display_print(score);

    pipe_spacing_count++;

    // Activate Pipes, if not moving, one per pipe spacing count
    if (pipe_spacing_count > pipe_spacing) {
      for (int i = 0; i < NUM_PIPES; ++i) {
        if (!is_pipe_moving(&pipes[i])) {
          init_pipe(&pipes[i]);
          break;
        }
      }
      pipe_spacing_count = RESET;
    }

    // Tick the Bird
    bird_tick(&bird);

    // Tick one pipe per tick function call
    if (pipe_count == RESET) {
      tick_pipes(&pipes[PIPE_ONE]);
      pipe_count = PIPE_TWO;
    }

    else if (pipe_count == 1) {
      tick_pipes(&pipes[PIPE_TWO]);
      pipe_count = PIPE_THREE;
    }

    else {
      tick_pipes(&pipes[PIPE_THREE]);
      pipe_count = PIPE_ONE;
    }
    break;

  // INcrease end game counter
  case (END_GAME):
    END_GAME_COUNT++;
    break;

  // Display final stats, with high score
  case (STATS):

    // Set final stats location
    display_setCursor(FINAL_SCORE_OFFSET, MIDDLE_HEIGHT);
    display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(MEDIUM_TEXT);
    display_print(score);

    // Adjust high score with current score if necessary
    if (High_score_num < score_num) {
      sprintf(High_score, "High Score: %d", score_num);
      High_score_num = score_num;
    }

    else {
      sprintf(High_score, "High Score: %d", High_score_num);
    }

    // Print the actual stats
    display_print("\n\r\n\r      ");
    display_print(High_score);
    display_setTextSize(1);
    display_print("\n\r\n\r\n\r            ");
    display_print("Press Button 0 to restart");

    break;
  }
}

// Draw countdown number
void print_countdown(uint16_t num) {
  char text[COUNTDOWN_TEXT_SIZE];
  sprintf(text, "%d", num);
  display_setCursor(MIDDLE_WIDTH, MIDDLE_HEIGHT);
  display_setTextSize(LARGE_TEXT);
  display_setTextColor(DISPLAY_WHITE);
  display_print(text);
}

// Erase countdown number
void erase_countdown(uint16_t num) {
  char text[COUNTDOWN_TEXT_SIZE];
  sprintf(text, "%d", num);
  display_setCursor(MIDDLE_WIDTH, MIDDLE_HEIGHT);
  display_setTextSize(LARGE_TEXT);
  display_setTextColor(DISPLAY_BLACK);
  display_print(text);
}