#include "buttons.h"
#include "xil_io.h"
#include "xparameters.h"


#define TRISTATEINPUT 0XF
#define GPIO_TRI_OFFSET 0x04 
#define GPIO_DATA_OFFSET 0x00

static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

void buttons_init(){
   writeRegister(GPIO_TRI_OFFSET,TRISTATEINPUT);
}

uint8_t buttons_read(){
   readRegister(GPIO_DATA_OFFSET); 
}

