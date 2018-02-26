// SysTick.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  If you are using this module, you may need more
// precise timing, so it is assumed that you are using the PLL to set
// the system clock to 50 MHz.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 2.11, Section 2.6

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
#include "../inc/tm4c123gh6pm.h"
#include "DAC.h"
#include "Music.h"

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

uint16_t sound_flute[32] = {1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,	
  1219,1092,1007,913,890,833,847,810,777,744,674,	
  598,551,509,476,495,533,589,659,758,876};

uint16_t sound_horn[32] = {1063,1082,1119,1275,1678,1748,1275,755,661,661,703,	
  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,	
  1370,1086,708,519,448,490,566,684,802,992};

uint16_t sound_trumpet[32] = {1007,1088,1156,1194,1067,789,303,99,789,1510,1476,	
  1173,1067,1037,1084,1062,1011,1015,1045,1062,1011,1011,	
  1058,1113,1084,1075,1079,1105,1088,1049,1015,1045};


extern uint16_t resetFlag;
extern uint16_t pauseFlag;
extern Note *music;
extern uint16_t musicLength;
extern uint16_t instrumentFlag;
	
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 20 nsec for 50 MHz clock)
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}
// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(500000);  // wait 10ms (assumes 50 MHz clock)
  }
}

void SysTick_Handler(void){
	static uint16_t DAC_index = 0;
	if (pauseFlag==0) {
		if (instrumentFlag == flute)DAC_Out(sound_flute[DAC_index]);
		else if (instrumentFlag == horn) DAC_Out(sound_horn[DAC_index]);
		else if (instrumentFlag == trumpet) DAC_Out(sound_trumpet[DAC_index]);
		DAC_index = (DAC_index+1)%32;
	}
}

void Timer0A_Init(void){
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 4000000;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

void Timer0A_Handler(void){
	static uint16_t note_counter = 0;
	static uint16_t song_counter = 0;
	TIMER0_ICR_R = 0x00000001;
	
	if (resetFlag != 0) {
		resetFlag = 0;
		note_counter = 0;
		song_counter = 0;
	}
	
	if (pauseFlag!=0){
		return;
	}
	
	if (note_counter == 0) {
		if (song_counter == musicLength) {
			song_counter = 0;
		}
		note_counter = (music+song_counter)->period;
		NVIC_ST_RELOAD_R = (music+song_counter)->freq;
		song_counter ++;
	}
	
	else {
		if (note_counter == 1) NVIC_ST_RELOAD_R = 0;
		note_counter --;
	}
}
