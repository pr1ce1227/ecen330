#include "interrupts.h"
#include "armInterrupts.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stddef.h>
#include <stdint.h>

// Interrupt controller Base Address
#define INTERRUPT_CONTROLLER XPAR_AXI_INTC_0_BASEADDR

// Offsets
#define MER_OFFSET 0x01C
#define ENABLE_MER 0x03
#define IER_OFFSET 0x08
#define RESET_IER 0x00
#define SIE_OFFSET 0x10
#define CIE_OFFSET 0x14
#define IPR_OFFSET 0x04
#define IAR_OFFSET 0x0C
#define NUM_FUNCTIONS 3
#define BIT_ACTIVE 1
#define START 0

// Holds all interrupt functions
static void (*isrFcnPtrs[NUM_FUNCTIONS])() = {NULL};

// Generic read register
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(INTERRUPT_CONTROLLER + offset);
}

// Generic Write Register
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(INTERRUPT_CONTROLLER + offset, value);
}

// Runs the interupt functions
static void interrupts_isr() {
  // Checks through each function
  for (int i = START; i < NUM_FUNCTIONS; ++i) {

    // Check if inturrupt is active
    if (readRegister(IPR_OFFSET) & (BIT_ACTIVE << i)) {

      // check to see if defined function for inturrupt
      if (isrFcnPtrs[i] != NULL) {
        isrFcnPtrs[i]();
      }

      // acknowledge IAR signal
      writeRegister(IAR_OFFSET, BIT_ACTIVE << i);
    }
  }
}

// Enable interrupt output
// Disable all interrupt input lines.
// Enable the Interrupt system on the ARM processor, and register an ISR
// handler function.
void interrupts_init() {
  writeRegister(MER_OFFSET, ENABLE_MER);
  writeRegister(IER_OFFSET, RESET_IER);

  armInterrupts_init();
  armInterrupts_setupIntc(interrupts_isr);
  armInterrupts_enable();
}

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq) {
  writeRegister(SIE_OFFSET, BIT_ACTIVE << irq);
}

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq) {
  writeRegister(CIE_OFFSET, BIT_ACTIVE << irq);
}