// Lab9.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V through X/10-ohm ohm resistor
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"
#include "UART.h"
#include "FIFO.h"
#include "util.h"
#include "fixed.h"
#include "ST7735.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

//debug code
//
// This program periodically samples ADC0 channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.


int main(void){
  PLL_Init(Bus80MHz);                      // 80 MHz system clock
	DisableInterrupts();
  ADC0_InitTimer0ATriggerSeq3(0, 4000000); // ADC channel 0, 20 Hz sampling
	TxFifo_Init();
  ST7735_InitR(INITR_REDTAB);
	ST7735_XYplotInit(0, 128, 0, 4000);
  EnableInterrupts();
	ST7735_OutString("Temperature: \nADC sample: ");
	int lastReadInvalid = 0;
	calibrate();
  while(1){
		uint32_t ADC_sample;
		if(TxFifo_Size() != 0){
			TxFifo_Get(&ADC_sample);
			uint32_t toPrint = ADC_to_Fahrenheit2Dec(ADC_sample);
			ST7735_SetCursor(0,0);
			if (toPrint == ADC_VAL_OUT_OF_RANGE_ERROR) {
				ST7735_SetCursor(12, 0);
				ST7735_OutString("Invalid");
				ST7735_SetCursor(11, 1);
				ST7735_OutString("Invalid");
				lastReadInvalid = 1;
			}
			else {
				ST7735_SetCursor(12, 0);
				if (lastReadInvalid) ST7735_OutString("       ");
				ST7735_SetCursor(12, 0);
				ST7735_sDecOut2(toPrint);
				ST7735_SetCursor(11, 1);
				if (lastReadInvalid) ST7735_OutString("       ");
				ST7735_SetCursor(11, 1);
				ST7735_OutUDec(ADC_sample);
				lastReadInvalid = 0;
				drawNextTemp(toPrint);
			}
		}
	}
}

