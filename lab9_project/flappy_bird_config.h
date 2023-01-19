#ifndef CONFIG
#define CONFIG

#include "display.h"

// Timing
#define CONFIG_TOUCHSCREEN_TIMER_PERIOD 10.0E-3
#define CONFIG_GAME_TIMER_PERIOD 50.0E-3
#define RESET 0
#define ONE_SECOND 20
#define TWO_SECONDS 40
#define TWELVE_SECONDS 240
#define SEVEN_SECONDS 150
#define TIME_3_ON 151
#define TIME_3_OFF 170
#define TIME_2_ON 171
#define TIME_2_off 190
#define TIME_1_ON 191
#define TIME_1_OFF 210
#define TIME_3 3
#define TIME_2 2
#define TIME_1 1

// Bird
#define BIRD_COLOR DISPLAY_BLUE
#define BIRD_RADIUS 10
#define BIRD_START_HEIGHT (DISPLAY_HEIGHT / 2)
#define BIRD_START_WIDTH (DISPLAY_WIDTH / 10)
#define JUMP_HEIGHT 1
#define RESET_JUMP_GRAVITY -7

// Pipes
#define PIPES_GAP 70
#define PIPES_WIDTH 20
#define BASE_SPEED 5
#define FAST_SPEED 12
#define NUM_PIPES 3
#define BASE_PIPE_SPACING 75
#define FAST_PIPE_SPACING 35
#define WAITING_PIPE -1
#define PIPE_ONE 0
#define PIPE_TWO 1
#define PIPE_THREE 2

// In game score text
#define SCORE_MSG_LOCATION (DISPLAY_WIDTH * .2)
#define SCORE_MSG "SCORE: %d"
#define PST_SCORE_CNT (score_num - 1)
#define RESET 0
#define SCORE_TEXT_SIZE 20
#define SMALL_TEXT 1
#define MEDIUM_TEXT 2
#define LARGE_TEXT 3
#define MIDDLE_HEIGHT (DISPLAY_HEIGHT / 2)
#define MIDDLE_WIDTH (DISPLAY_WIDTH / 2)
#define FINAL_SCORE_OFFSET (DISPLAY_WIDTH * .22)
#define MAIN_INSTRUCTIONS_OFFSET (DISPLAY_WIDTH * .1)
#define COUNTDOWN_TEXT_SIZE 5

// Buttons and Switches
#define SWITCH_0_ON 0x01
#define BUTTON_0_ON 0x01

#endif /* CONFIG */