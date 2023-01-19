#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

// Base addresses of timers
#define TIMER_0 XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_1 XPAR_AXI_TIMER_1_BASEADDR
#define TIMER_2 XPAR_AXI_TIMER_2_BASEADDR

// Timer Numbers
#define TIMER_NUMBER_0 0
#define TIMER_NUMBER_1 1
#define TIMER_NUMBER_2 2

// Register Offsets for timers
#define TCSRO 0x00
#define TCSR1 0x010
#define TLRO 0x04
#define TLR1 0x014
#define TCRO 0x08
#define TCR1 0x018

// Timer settings adjustments
#define RESET 0x00
#define CASCADE_ON 0x0800
#define COUNT_DOWN 0x002
#define AUTO_RELOAD 0x010
#define EN_TIMER 0x080
#define EN_LOAD 0x020
#define ENITO 0x040
#define TOINT 0x00100

// Variables for calculating time in seconds
#define HALF_SHIFT 32
#define FREQUENCY XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ

// Reads given register, bridge from harware to software.
static uint32_t readRegister(uint32_t timerNumber, uint32_t offset) {

  // If statement checks which timer is being read
  if (timerNumber == TIMER_NUMBER_0) {
    return Xil_In32(TIMER_0 + offset);
  }

  else if (timerNumber == TIMER_NUMBER_1) {
    return Xil_In32(TIMER_1 + offset);
  }

  else if (timerNumber == TIMER_NUMBER_2) {
    return Xil_In32(TIMER_2 + offset);
  }
}

// Writes to given register, bridge from hardware to software
static void writeRegister(uint32_t timerNumber, uint32_t offset,
                          uint32_t value) {

  // if Statement checks which timer is being written
  if (timerNumber == TIMER_NUMBER_0) {
    Xil_Out32(TIMER_0 + offset, value);
  }

  else if (timerNumber == TIMER_NUMBER_1) {
    Xil_Out32(TIMER_1 + offset, value);
  }

  else if (timerNumber == TIMER_NUMBER_2) {
    Xil_Out32(TIMER_2 + offset, value);
  }
}

// Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts up
// Initialize both LOAD registers with zeros
// Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountUp(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO, RESET);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | CASCADE_ON);
  writeRegister(timerNumber, TLRO, RESET);
  writeRegister(timerNumber, TLR1, RESET);
  intervalTimer_reload(timerNumber);
}

// Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// Initialize LOAD registers with appropriate values, given the `period`.
// Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  uint64_t time = period * FREQUENCY;
  uint32_t time_lower = time;
  uint32_t time_upper = time >> HALF_SHIFT;

  writeRegister(timerNumber, TCSRO, RESET);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | CASCADE_ON);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | COUNT_DOWN);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | AUTO_RELOAD);

  writeRegister(timerNumber, TLRO, time_lower);
  writeRegister(timerNumber, TLR1, time_upper);

  intervalTimer_reload(timerNumber);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | EN_TIMER);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) & ~EN_TIMER);
}

// Turns on reload bit then imediatl shuts off the bit to lock in the values
void intervalTimer_reload(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) | EN_LOAD);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) & ~EN_LOAD);

  writeRegister(timerNumber, TCSR1, readRegister(timerNumber, TCSR1) | EN_LOAD);
  writeRegister(timerNumber, TCSR1,
                readRegister(timerNumber, TCSR1) & ~EN_LOAD);
  uint64_t time = intervalTimer_getTotalDurationInSeconds(0);
}

// Converts the time intos seconds to be printed to the monitor
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t time = readRegister(timerNumber, TCR1);
  time = time << HALF_SHIFT;
  time = time | readRegister(timerNumber, TCRO);
  double time_seconds = (double)time / FREQUENCY;
  return time_seconds;
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) | ENITO);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) & ~ENITO);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) | TOINT);
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) & ~TOINT);
}