#include "buttons.h"
#include "xil_io.h"
#include "xparameters.h"

#define TRI_STATE_INPUT 0X0F
#define GPIO_BUTTONS_TRI_OFFSET 0x04
#define GPIO_DATA_OFFSET 0x00

static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

void buttons_init() { writeRegister(GPIO_BUTTONS_TRI_OFFSET, TRI_STATE_INPUT); }

uint8_t buttons_read() { return readRegister(GPIO_DATA_OFFSET); }
