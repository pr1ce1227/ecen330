/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "display.h"

#define HALFHEIGHT DISPLAY_HEIGHT / 2
#define HALFWIDTH DISPLAY_WIDTH / 2
#define ORIGIN 0
#define RADIUS 25
#define QUARTERWIDTH DISPLAY_WIDTH * .25
#define THREEQUARTERWIDTH DISPLAY_WIDTH * .75
#define THIRDHEIGHT DISPLAY_HEIGHT * .33
#define TWOTHIRDHEIGHT DISPLAY_HEIGHT * .66
#define THIRDWIDTH DISPLAY_WIDTH * .33
#define TWOTHIRDWIDTH DISPLAY_WIDTH * .66
#define TENTHHEIGHT DISPLAY_HEIGHT * .1
#define NINETENTHHEIGHT DISPLAY_HEIGHT * .9

// Print out "hello world" on both the console and the LCD screen.
int main() {

  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // 2 green crossing lines
  display_drawLine(ORIGIN, ORIGIN, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);
  display_drawLine(ORIGIN, DISPLAY_HEIGHT, DISPLAY_WIDTH, ORIGIN,
                   DISPLAY_GREEN);

  // 2 circles, one empty one filled
  display_drawCircle(QUARTERWIDTH, HALFHEIGHT, RADIUS, DISPLAY_RED);
  display_fillCircle(THREEQUARTERWIDTH, HALFHEIGHT, RADIUS, DISPLAY_RED);

  // 2 triangles, one empty one filled
  display_fillTriangle(HALFWIDTH, THIRDHEIGHT, THIRDWIDTH, TENTHHEIGHT,
                       TWOTHIRDWIDTH, TENTHHEIGHT, DISPLAY_WHITE);
  display_drawTriangle(HALFWIDTH, TWOTHIRDHEIGHT, THIRDWIDTH, NINETENTHHEIGHT,
                       TWOTHIRDWIDTH, NINETENTHHEIGHT, DISPLAY_YELLOW);

  return 0;

  // test
}
