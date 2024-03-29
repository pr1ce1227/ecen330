#include <stdio.h>

#define MISSILE_COMMAND_PART2

#include "flappy_bird_config.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "pipes.h"
#include "touchscreen.h"

#define RUNTIME_S 30
#define RUNTIME_TICKS ((int)(RUNTIME_S / CONFIG_GAME_TIMER_PERIOD))

volatile bool interrupt_flag;
bool tick = true;

uint32_t isr_triggered_count;
uint32_t isr_handled_count;

// Interrupt handler for game - use flag method so that it can be interrupted by
// the touchscreen tick while running.
void game_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  interrupt_flag = true;
  isr_triggered_count++;
}

// Interrupt handler for touchscreen - tick directly
void touchscreen_isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);
  touchscreen_tick();
}

// Milestone 2 test application
int main() {
  interrupt_flag = false;
  isr_triggered_count = 0;
  isr_handled_count = 0;

  display_init();
  touchscreen_init(CONFIG_TOUCHSCREEN_TIMER_PERIOD);
  display_fillScreen(DISPLAY_BLACK);
  pipes_t pipe;
  init_pipe(&pipe);

  // Initialize timer interrupts
  interrupts_init();
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, game_isr);
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, touchscreen_isr);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);

  intervalTimer_initCountDown(INTERVAL_TIMER_0, CONFIG_GAME_TIMER_PERIOD);
  intervalTimer_initCountDown(INTERVAL_TIMER_1,
                              CONFIG_TOUCHSCREEN_TIMER_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);

  // Main game loop
  while (isr_triggered_count < RUNTIME_TICKS) {
    while (!interrupt_flag)
      ;
    interrupt_flag = false;
    isr_handled_count++;
    if (tick) {
      tick_pipes(&pipe);
    }
    tick = !tick;
  }
  printf("Handled %d of %d interrupts\n", isr_handled_count,
         isr_triggered_count);
}