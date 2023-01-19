#include "clockControl.h"
#include "clockDisplay.h"
#include "stdint.h"
#include "touchscreen.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define PRESS_DELAY .5
#define QUICK_DELAY .1
#define RESET 0
typedef enum {
  INC_DEC,
  WAITING,
  LONG_PRESS_DELAY,
  FAST_UPDATE
} clockControl_state_t;

// Global Variables
static clockControl_state_t currentState;
static uint16_t delay_num_ticks;
static uint16_t delay_cnt;
static uint16_t update_cnt;
static uint16_t update_num_ticks;

// Initialize the clock control state machine, with a given period in seconds.
void clockControl_init(double period_s) {
  currentState = WAITING;
  delay_num_ticks = ceil(PRESS_DELAY / period_s);
  delay_cnt = RESET;
  update_cnt = RESET;
  update_num_ticks = ceil(QUICK_DELAY / period_s);
}

// Tick the clock control state machine
void clockControl_tick() {

  // Control State Transitions
  switch (currentState) {

  case WAITING:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      currentState = INC_DEC;
    }

    else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      currentState = LONG_PRESS_DELAY;
      delay_cnt = 0;
    }

    else {
      currentState == WAITING;
    }
    break;

  case INC_DEC:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED)
      currentState = WAITING;
    touchscreen_ack_touch();
    break;

  case LONG_PRESS_DELAY:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      currentState = INC_DEC;
    }

    else if (delay_cnt >= delay_num_ticks) {
      currentState = FAST_UPDATE;
      update_cnt = 0;
    } else {
      currentState = LONG_PRESS_DELAY;
      delay_cnt++;
    }
    break;

  case FAST_UPDATE:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      currentState = WAITING;
      touchscreen_ack_touch();
    }

    else if (touchscreen_get_status() != TOUCHSCREEN_RELEASED &&
             update_cnt == update_num_ticks) {
      update_cnt = 0;
      clockDisplay_performIncDec(touchscreen_get_location());
    }

    else {
      currentState = FAST_UPDATE;
      update_cnt++;
    }
    break;

  default:
    currentState = WAITING;
    break;
  }

  // Control state Actions
  switch (currentState) {

  case WAITING:
    break;

  case INC_DEC:
    clockDisplay_performIncDec(touchscreen_get_location());
    break;

  case LONG_PRESS_DELAY:
    delay_cnt++;
    break;

  case FAST_UPDATE:
    break;

  default:
    break;
  }
}