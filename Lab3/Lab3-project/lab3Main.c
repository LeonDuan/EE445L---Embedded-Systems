// lab3Main.c
// Runs on LM4F120/TM4C123
// Test the switch initialization functions by setting the LED
// color according to the status of the switches.
// Daniel and Jonathan Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 2.3, Program 2.9, Figure 2.36

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

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG pins as GPIO, which can lock the debugger out
// of the processor and make it permanently unable to be debugged or
// re-programmed.

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Switch.h" // input
#include "Display.h" // graphics
#include "ST7735.h"
#include "timeUtils.h" // time utilities
#include "speaker.h" // speaker




#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board

//GLOBAL VARIABLES//
uint32_t currentMode = 1;
uint32_t setTimeFlag = 0;
uint32_t currentHour = 0;
uint32_t currentMinute = 0;
uint32_t currentSeconds = 0;
uint32_t prevHour;
uint32_t prevMinute;
uint32_t prevSeconds;
uint32_t setAlarmFlag = 0;
uint32_t alarmHour = 0;
uint32_t alarmMinute = 0;
uint32_t alarmActive = 0;
uint32_t soundFlag = 0;
uint32_t updateMinuteFlag = 0;
uint32_t updateHourFlag = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

int main(void){
	PLL_Init(Bus80MHz);   
	PortF_Init(); // initialize port F (LED outputs)
	Switch_Init(); // initialize inputs
	SysTick_Init(); // initialize SysTick
	init_LCD();	//initialize LCD
	enableSpeaker(1);
	
	EnableInterrupts();
	
	draw_AnalogClock();
	for (int i=0; i<60; i++) {
		draw_ClockHand(63, 95, Minute_Hand[i].x,Minute_Hand[i].y,0);
	}
	init_LCD();
	draw_DigitalClock(12,35);
	while(1) {
		switch(currentMode) {
			case 1:
				draw_AnalogClock();
				if (updateMinuteFlag) {
					draw_ClockHand(CLOCK_ORIGIN, Minute_Hand[prevMinute].x,Minute_Hand[prevMinute].y,ST7735_Color565(228,228,228));
					draw_ClockHand(CLOCK_ORIGIN, Minute_Hand[currentMinute].x,Minute_Hand[currentMinute].y,0);
					updateMinuteFlag = 0;
				}
				if (updateHourFlag) {
					draw_ClockHand(CLOCK_ORIGIN, Hour_Hand[prevHour].x,Minute_Hand[prevHour].y,0);
					draw_ClockHand(CLOCK_ORIGIN, Hour_Hand[currentHour].x,Minute_Hand[currentHour].y,0);
					updateMinuteFlag = 0;
				}
				break;
			case 2:
				break;
			case 3:
				break;
		}
	}
}
