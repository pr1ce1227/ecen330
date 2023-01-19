#include "touchscreen.h"
#include "display.h"
#include "stdint.h"
#include <math.h>
#include <stdio.h>

// Inital Reset value, and period needed for touch to be sensed in seconds
#define WAIT_PERIOD .05
#define RESET 0

typedef enum {
  WAITING,      // Touchscreen is idle (not pressed)
  ADC_SETTLING, // Touchscreen is actively being pressed
  PRESSED_ST    // Touchscreen has been released, but not acknowledged
} touchscreen_state_t;

// Global Variabls
static uint16_t adc_settle_ticks;
static touchscreen_state_t currentState;
static touchscreen_status_t currentStatus;
static bool pressed;
static display_point_t point1;
static uint16_t adc_timer;
static uint16_t *x;
static uint16_t *y;
static uint8_t *z;

// Initialize the touchscreen driver state machine, with a given tick period
// (in seconds).
void touchscreen_init(double period_seconds) {
  adc_settle_ticks = ceil(WAIT_PERIOD / period_seconds);
  currentState = WAITING;
  currentStatus = TOUCHSCREEN_IDLE;
  pressed = false;
  point1.x = RESET;
  point1.y = RESET;
  adc_timer = RESET;
}

// Prints Current state Each time the state is changed
// void debugStatePrint() {
//   static touchscreen_state_t previousState;
//   static bool firstPass = true;
//   // Only print the message if:
//   // 1. This the first pass and the value for previousState is unknown.
//   // 2. previousState != currentState - this prevents reprinting the same
//   // state
//   // name over and over.
//   if (previousState != currentState || firstPass) {
//     firstPass = false; // previousState will be defined, firstPass is false.
//     previousState =
//         currentState;       // keep track of the last state that you were in.
//     switch (currentState) { // This prints messages based upon the state that
//                             // you were in.
//     case WAITING:
//       printf("WAITING\n");
//       break;
//     case ADC_SETTLING:
//       printf("ADC_SETTLING\n");
//       break;
//     case PRESSED_ST:
//       printf("PRESSED_ST\n");
//       break;
//     }
//   }
// }

// Tick the touchscreen driver state machine
void touchscreen_tick() {
  // Debugging, Print state function
  // debugStatePrint();

  // State Transitions *********************
  switch (currentState) {

  case WAITING:
    // Transition to ADC_SETTLING, Set Status to IDLE
    if (display_isTouched()) {
      currentState = ADC_SETTLING;
      display_clearOldTouchData();
      currentStatus = TOUCHSCREEN_IDLE;
    }

    // Remain in current state
    else {
      currentState = WAITING;
    }
    break;

  case ADC_SETTLING:
    // Transition to Waiting, set status to IDLE
    if (!display_isTouched()) {
      currentState = WAITING;
      currentStatus = TOUCHSCREEN_IDLE;
    }

    // Transition to PRESSED_ST, Set status to Pressed
    else if (display_isTouched() && (adc_timer >= adc_settle_ticks)) {
      static uint16_t x_val;
      static uint16_t y_val;
      static uint8_t z_val;
      x = &x_val;
      y = &y_val;
      z = &z_val;
      display_getTouchedPoint(x, y, z);
      point1.x = x_val;
      point1.y = y_val;
      currentState = PRESSED_ST;
      currentStatus = TOUCHSCREEN_PRESSED;
    }

    // Remain in Current State
    else {
      currentState = ADC_SETTLING;
    }
    break;

  case PRESSED_ST:
    // Transistion to waiting, Set status to Released
    if (!display_isTouched()) {
      currentState = WAITING;
      currentStatus = TOUCHSCREEN_RELEASED;
    }

    // Remain in current state
    else {
      currentState = PRESSED_ST;
    }
    break;
  }

  // State Actions **********************
  switch (currentState) {
  // set adc_timer to 0
  case WAITING:
    adc_timer = RESET;
    break;

  // start adc_timer counter
  case ADC_SETTLING:
    adc_timer++;
    break;

  // set press boolean to true
  case PRESSED_ST:
    pressed = true;
    break;
  }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status() { return currentStatus; }

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch() { currentStatus = TOUCHSCREEN_IDLE; }

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location() { return point1; }