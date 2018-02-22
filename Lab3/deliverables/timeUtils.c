// SysTick.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 4.7
   
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
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
#include "timeUtils.h"
#include "PLL.h"
#include "speaker.h"


#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

extern uint32_t prevSeconds;
extern uint32_t prevHour;
extern uint32_t prevMinute;
extern uint32_t currentMinute;
extern uint32_t currentHour;
extern uint32_t currentSeconds;
extern uint32_t updateTimeFlag;

long StartCritical(void);
void EndCritical(long);

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
	long sr = StartCritical();
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 20000;  // period
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; //Priority 1
  NVIC_ST_CTRL_R = 0x07;
	EndCritical(sr);
}
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 62.5 nsec for 16 MHz clock)
//void SysTick_Wait(uint32_t delay){
//  volatile uint32_t elapsedTime;
//  uint32_t startTime = NVIC_ST_CURRENT_R;
//  do{
//    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
//  }
//  while(elapsedTime <= delay);
//}
//// Time delay using busy wait.
//// This assumes 16 MHz system clock.
//void SysTick_Wait10ms(uint32_t delay){
//  uint32_t i;
//  for(i=0; i<delay; i++){
//    SysTick_Wait(800000);  // wait 10ms (assumes 80 MHz clock)
//  }
//}
#define PF2       (*((volatile uint32_t *)0x40025010))
	
extern uint32_t alarmHour;
extern uint32_t alarmMinute;
extern uint32_t alarmActive;
extern uint32_t soundFlag;
void SysTick_Handler(){
	if(soundFlag && alarmActive){
		GPIO_PORTD_DATA_R = !GPIO_PORTD_DATA_R;
	}
}
	
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function


// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//         period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(void){
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 8000-1;    // 4) reload value
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
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	// PF2 ^= 0x04;
  prevSeconds = currentSeconds;
	currentSeconds += 1;
	
	if (currentSeconds>=60) {
		currentSeconds = 0;
		currentMinute += 1;
		updateTimeFlag = 1;
	}
	
	if (currentMinute>=60) {
		currentMinute = 0;
		currentHour += 1;
		updateTimeFlag = 1;
	}
	
	if (currentHour>=24) {
		currentHour = 0;
	}
	
	if (alarmHour == currentHour && alarmMinute == currentMinute && alarmActive){
		soundFlag = 1;
	}
}

void Timer1_Arm(void){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 80000000-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
  static int snooze_counter = 60;
	if (snooze_counter==0) {
		TIMER0_IMR_R = 0x00000000;    // disarm timeout interrupt
		enableSpeaker(1);
		soundFlag = 1;
		snooze_counter = 60;	
	}
	else snooze_counter -- ;
}
