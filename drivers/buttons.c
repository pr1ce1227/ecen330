#include "buttons.h"
#include "xil_io.h"
#include "xparameters.h"

#define TRI_STATE_INPUT 0X0F
#define GPIO_BUTTONS_TRI_OFFSET 0x04
#define GPIO_DATA_OFFSET 0x00

// returns 32 bit read of buttons, bits 3:0 hold buttons state
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// sets Tri-State driver state, requires address offset and value
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// intitializes tri-state driver as off, setting buttons as input read only
void buttons_init() { writeRegister(GPIO_BUTTONS_TRI_OFFSET, TRI_STATE_INPUT); }

// returns 8 bit state of buttons, bit 3:0 correspond to buttons on board.
uint8_t buttons_read() { return readRegister(GPIO_DATA_OFFSET); }
