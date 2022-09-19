#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define TIMER_0 XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_1 XPAR_AXI_TIMER_1_BASEADDR
#define TIMER_2 XPAR_AXI_TIMER_2_BASEADDR
#define TCSRO 0x00
#define TCSR1 0x010
#define TLRO 0x04
#define TLR1 0x014
#define TCRO 0x08
#define TCR1 0x018
#define RESET 0x00
#define CASCADE_ON 0x0800
#define COUNT_DOWN 0x002
#define AUTO_RELOAD 0x010
#define EN_TIMER 0x080
#define EN_LOAD 0x020
#define FREQUENCY 100000000
#define ENITO 0x040
#define TOINT 0x00100

static uint32_t readRegister(uint32_t timerNumber, uint32_t offset) {
  if (timerNumber == 0) {
    return Xil_In32(TIMER_0 + offset);
  }

  else if (timerNumber == 1) {
    return Xil_In32(TIMER_1 + offset);
  }

  else if (timerNumber == 2) {
    return Xil_In32(TIMER_2 + offset);
  }
}

static void writeRegister(uint32_t timerNumber, uint32_t offset,
                          uint32_t value) {
  if (timerNumber == 0) {
    Xil_Out32(TIMER_0 + offset, value);
  }

  else if (timerNumber == 1) {
    Xil_Out32(TIMER_1 + offset, value);
  }

  else if (timerNumber == 2) {
    Xil_Out32(TIMER_2 + offset, value);
  }
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts up
// 2. Initialize both LOAD registers with zeros
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountUp(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO, RESET);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | CASCADE_ON);
  writeRegister(timerNumber, TLRO, RESET);
  writeRegister(timerNumber, TLR1, RESET);
  intervalTimer_reload(timerNumber);
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// 2. Initialize LOAD registers with appropriate values, given the `period`.
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | CASCADE_ON);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) & CASCADE_ON);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | (COUNT_DOWN | AUTO_RELOAD));
  writeRegister(timerNumber, TLRO, period);
  writeRegister(timerNumber, TLR1, period);
  intervalTimer_reload(timerNumber);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_start(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) | EN_TIMER);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_stop(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) & ~EN_TIMER);
}

// This function is called whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.  The load registers should have already
// been set in the appropriate `init` function, so there is no need to set
// them here.  You just need to enable the load (and remember to disable it
// immediately after otherwise you will be loading indefinitely).
void intervalTimer_reload(uint32_t timerNumber) {
  writeRegister(timerNumber, TCSRO, readRegister(timerNumber, TCSRO) | EN_LOAD);
  writeRegister(timerNumber, TCSRO,
                readRegister(timerNumber, TCSRO) & ~EN_LOAD);

  writeRegister(timerNumber, TCSR1, readRegister(timerNumber, TCSR1) | EN_LOAD);
  writeRegister(timerNumber, TCSR1,
                readRegister(timerNumber, TCSR1) & ~EN_LOAD);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t time = readRegister(timerNumber, TCRO);
  time = time << 32;
  time = time & readRegister(timerNumber, TCR1);
  double time_seconds = time / FREQUENCY;
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