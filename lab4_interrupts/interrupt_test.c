#include "interrupt_test.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include <stdio.h>

// LEDS
#define LED_0 0x01
#define LED_1 0x02
#define LED_2 0x04

// TIMERS
#define TIMER_0 0
#define TIMER_1 1
#define TIMER_2 2

// INTTURUPTS
#define INTURRUPT_0 0
#define INTURRUPT_1 1
#define INTURRUPT_2 2

// PERIODS
#define PERIOD_ONE 1
#define PERIOD_TEN 10
#define PERIOD_TENTH .1

// Inturrupt function to turn on and off LED 0
static void timer0_isr() {
  intervalTimer_ackInterrupt(TIMER_0);
  leds_write(leds_read() ^ LED_0);
}

// Inturrupt function to turn on and off LED 1
static void timer1_isr() {
  intervalTimer_ackInterrupt(TIMER_1);
  leds_write(leds_read() ^ LED_1);
}

// Inturrupt function to turn on and off LED 2
static void timer2_isr() {
  intervalTimer_ackInterrupt(TIMER_2);
  leds_write(leds_read() ^ LED_2);
}

void interrupt_test_run() {
  // initialize inturrupts and leds
  interrupts_init();
  leds_init();

  // initialize timers as countdown with differing periods
  intervalTimer_initCountDown(TIMER_0, PERIOD_TENTH);
  intervalTimer_initCountDown(TIMER_1, PERIOD_ONE);
  intervalTimer_initCountDown(TIMER_2, PERIOD_TEN);

  // enable inturrupts on all 3 timers
  intervalTimer_enableInterrupt(TIMER_0);
  intervalTimer_enableInterrupt(TIMER_1);
  intervalTimer_enableInterrupt(TIMER_2);

  // Enable 3 intturupts input swithcs on the intturrupt controler
  interrupts_irq_enable(INTURRUPT_0);
  interrupts_irq_enable(INTURRUPT_1);
  interrupts_irq_enable(INTURRUPT_2);

  // Pass in inturupt functions to inturrupt driver
  interrupts_register(INTURRUPT_0, timer0_isr);
  interrupts_register(INTURRUPT_1, timer1_isr);
  interrupts_register(INTURRUPT_2, timer2_isr);

  // Start all the timers
  intervalTimer_start(TIMER_0);
  intervalTimer_start(TIMER_1);
  intervalTimer_start(TIMER_2);

  // Loop through time, intturupts will trigger leds
  while (1) {
  }
}