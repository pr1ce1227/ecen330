#include "switches.h"
#include "xil_io.h"
#include "xparameters.h"

#define GPIO_SWITCH_TRI 0x0F
#define GPIO_SWITCH_TRI_OFFSET 0x04
#define GPIO_SWITCH_DATA 0x00

// reutrns 32 bit hardware ouput of the switches, bits 3:0 hold state of
// switches
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// Writes to the Tri-State driver, requires the address offset and value of pins
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Sets the tri-state driver off which sets switches as input Reads
void switches_init() { writeRegister(GPIO_SWITCH_TRI_OFFSET, GPIO_SWITCH_TRI); }

// Returns 32 bit read of sitches, bit 3:0 hold state of switches
uint8_t switches_read() { return readRegister(GPIO_SWITCH_DATA); }