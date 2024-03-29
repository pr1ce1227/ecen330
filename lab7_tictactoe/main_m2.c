/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "interrupts.h"
#include "intervalTimer.h"
#include "ticTacToeControl.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"
#include "utils.h"

#define TICK_PERIOD 50E-3
#define TOTAL_SECONDS 30
#define MAX_INTERRUPT_COUNT (TOTAL_SECONDS / TICK_PERIOD)

#define MILESTONE1_MESSAGE "Running testBoards()\n"
#define MILESTONE2_MESSAGE "Running tic-tac-toe game\n"

// Keep track of how many times interrupt routing is called,
// and how many times ISR is run
volatile uint32_t interrupt_count;
uint32_t isr_run_count;

// Interrupt flag method
volatile bool interrupt_flag;

// Interrupt Service Routing to run tick functions using flag method
static void isr();

int main() {
  printf(MILESTONE2_MESSAGE);

  // Initialize Modules
  ticTacToeControl_init(TICK_PERIOD);
  touchscreen_init(TICK_PERIOD);

  interrupts_init();
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
  intervalTimer_initCountDown(INTERVAL_TIMER_0, TICK_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);

  interrupt_count = 0;
  isr_run_count = 0;
  interrupt_flag = false;

  intervalTimer_start(INTERVAL_TIMER_0);

  while (1) {
    // Wait for interrupt flag
    while (!interrupt_flag) {
      utils_sleep();
    }
    interrupt_flag = false;

    // Increment counter
    isr_run_count++;

    // Run tick functions
    ticTacToeControl_tick();
    touchscreen_tick();

    // Stop after predetermined amount of ticks
    // if (interrupt_count >= MAX_INTERRUPT_COUNT)
    //   break;
  }

  // Stop interrupts and print counts
  intervalTimer_stop(INTERVAL_TIMER_0);
  printf("interrupt count: %d\n", interrupt_count);
  printf("isr invocation count: %d\n", isr_run_count);
  return 0;
}

// Interrupt routine
static void isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  interrupt_count++;
  interrupt_flag = true;
}