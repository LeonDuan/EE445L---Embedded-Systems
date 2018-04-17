// PWMtest.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 6.8, section 6.3.2

   "Embedded Systems: Real-Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2015
   Program 8.4, Section 8.3

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
#include <stdint.h>
#include "PLL.h"
#include "PWM.h"
#include "Tach.h"
#include "Switch.h"
#include "fixed.h"
#include "util.h"
#include "SysTick.h"
#include "ST7735.h"

uint32_t Target_Speed = 300;
uint32_t Speed;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void Switch_1(void){ // increase speed
	Target_Speed += 50;
	if (Target_Speed > 600) Target_Speed = 600;
}
void Switch_2(void){ // decrease speed
	Target_Speed -= 50;
	if (Target_Speed < 150) Target_Speed = 150;
}

int main(void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);               // bus clock at 80 MHz
	Tach_Init();
  Motor_Init(40000, 30000);         // initialize Motor, 1000 Hz, 75% duty
	Switch_Init(&Switch_1,&Switch_2);
  ST7735_InitR(INITR_REDTAB);
	ST7735_XYplotInit(0, 128, 0, 800);
	SysTick_Init();
	EnableInterrupts();
  while(1){
		SysTick_Wait(8000000);
		drawNext(Target_Speed, ST7735_BLUE, 0);
		drawNext(Speed, ST7735_BLACK, 1);
		ST7735_SetCursor(0, 0);
		ST7735_OutUDec(Speed/10);
		ST7735_OutString("rps");
  }
}
