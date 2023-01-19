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

#define halfHeight DISPLAY_HEIGHT / 2
#define halfWidth DISPLAY_WIDTH / 2
#define origin 0
#define radius 25
#define quarterWidth DISPLAY_WIDTH * .25
#define threeQuarterWidth DISPLAY_WIDTH * .75
#define thirdHeight DISPLAY_HEIGHT * .33
#define twoThirdHeight DISPLAY_HEIGHT * .66
#define thirdWidth DISPLAY_WIDTH * .33
#define twoThirdWidth DISPLAY_WIDTH * .66
#define tenthHeight DISPLAY_HEIGHT * .1
#define nineTenthHeight DISPLAY_HEIGHT * .9

// Print out "hello world" on both the console and the LCD screen.
int main() {

  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // 2 green crossing lines
  display_drawLine(origin, origin, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);
  display_drawLine(origin, DISPLAY_HEIGHT, DISPLAY_WIDTH, origin,
                   DISPLAY_GREEN);

  // 2 circles, one empty one filled
  display_drawCircle(quarterWidth, halfHeight, radius, DISPLAY_RED);
  display_fillCircle(threeQuarterWidth, halfHeight, radius, DISPLAY_RED);

  // 2 triangles, one empty one filled
  display_fillTriangle(halfWidth, thirdHeight, thirdWidth, tenthHeight,
                       twoThirdWidth, tenthHeight, DISPLAY_WHITE);
  display_drawTriangle(halfWidth, twoThirdHeight, thirdWidth, nineTenthHeight,
                       twoThirdWidth, nineTenthHeight, DISPLAY_YELLOW);
  return 0;
}
