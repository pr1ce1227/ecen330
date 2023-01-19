#include "switches.h"
#include "xil_io.h"
#include "xparameters.h"

#define GPIO_SWITCH_TRI 0x0F
#define GPIO_SWITCH_TRI_OFFSET 0x04
#define GPIO_SWITCH_DATA 0x00

static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

void switches_init() { writeRegister(GPIO_SWITCH_TRI_OFFSET, GPIO_SWITCH_TRI); }

uint8_t switches_read() { return readRegister(GPIO_SWITCH_DATA); }