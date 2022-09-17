#include "gpioTest.h"
#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "switches.h"
#include <stdio.h>

// Colored Boxes Position
#define BOX_LENGTH DISPLAY_WIDTH / 4
#define BOX_HEIGHT DISPLAY_HEIGHT / 2
#define BOX_Y 0
#define BOX3_X 0
#define BOX2_X BOX_LENGTH
#define BOX1_X BOX_LENGTH * 2
#define BOX0_X BOX_LENGTH * 3

// Driver States
#define START_STATE 0x00
#define FINISH_STATE 0x0F

// Cursor text location
#define CURSOR_HEIGHT BOX_HEIGHT / 2
#define BOX3_X_CURSOR BOX3_X + (BOX2_X / 4)
#define BOX2_X_CURSOR BOX2_X + (BOX2_X / 4)
#define BOX1_X_CURSOR BOX1_X + (BOX2_X / 4)
#define BOX0_X_CURSOR BOX0_X + (BOX2_X / 4)

// Colored boxes text
#define BUTTON0_TEXT "BTN0"
#define BUTTON1_TEXT "BTN1"
#define BUTTON2_TEXT "BTN2"
#define BUTTON3_TEXT "BTN3"

// Function Displays a colored box with button name when button is pressed,
// erased when released
void gpioTest_buttons() {

  // Initialize Drivers, display, states and text settings
  display_init();
  buttons_init();
  uint8_t prev_state = START_STATE;
  uint8_t current_state = START_STATE;
  display_fillScreen(DISPLAY_BLACK);
  display_setTextColor(DISPLAY_RED);
  display_setTextSize(2);
  display_setTextWrap(false);

  // Loops until all buttons are pressed, then function exits
  // current and previous state keep track of wether box has been erased or
  // drawn.
  while (1) {

    current_state = buttons_read();

    // If the buttons have entered a new state then apply the changes
    if (current_state != prev_state) {
      prev_state = current_state;

      // If Button 0 is pressed display colored box and name, else erase box
      if ((buttons_read() & BUTTONS_BTN0_MASK) == BUTTONS_BTN0_MASK) {
        display_fillRect(BOX0_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_WHITE);
        display_setCursor(BOX0_X_CURSOR, CURSOR_HEIGHT);
        display_print(BUTTON0_TEXT);
      }

      else {
        display_fillRect(BOX0_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_BLACK);
      }

      // If Button 1 is pressed display colored box and name, else erase box
      if ((buttons_read() & BUTTONS_BTN1_MASK) == BUTTONS_BTN1_MASK) {
        display_fillRect(BOX1_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_GREEN);
        display_setCursor(BOX1_X_CURSOR, CURSOR_HEIGHT);
        display_print(BUTTON1_TEXT);
      }

      else {
        display_fillRect(BOX1_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_BLACK);
      }

      // If Button 2 is pressed display colored box and name, else erase box
      if ((buttons_read() & BUTTONS_BTN2_MASK) == BUTTONS_BTN2_MASK) {
        display_fillRect(BOX2_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_YELLOW);
        display_setCursor(BOX2_X_CURSOR, CURSOR_HEIGHT);
        display_print(BUTTON2_TEXT);
      }

      else {
        display_fillRect(BOX2_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_BLACK);
      }

      // If Button 3 is pressed display colored box and name, else erase box
      if ((buttons_read() & BUTTONS_BTN3_MASK) == BUTTONS_BTN3_MASK) {
        display_fillRect(BOX3_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_BLUE);
        display_setCursor(BOX3_X_CURSOR, CURSOR_HEIGHT);
        display_print(BUTTON3_TEXT);
      }

      else {
        display_fillRect(BOX3_X, BOX_Y, BOX_LENGTH, BOX_HEIGHT, DISPLAY_BLACK);
      }

      // If all buttons are pressed exit Funciton
      if (buttons_read() == FINISH_STATE) {
        break;
      }
    }
  }
}

// Funciton turns on or off corresponding LED when switch is flipped on or off
void gpioTest_switches() {

  // Initialized LED's and switches
  leds_init();
  switches_init();

  // Loops until all switches are flipped on, then exits
  while (1) {

    /* CHecks to see if all switches are on, if so it exits, else it displays
       the LED's corresponding to the switches */
    if (switches_read() == FINISH_STATE) {
      leds_write(START_STATE);
      break;
    }

    else {
      leds_write(switches_read());
    }
  }
}