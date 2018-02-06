// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

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
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include <stdint.h>
#include "ST7735.h"
#include <stdlib.h>

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void (*PeriodicTask)(void);   // user function
void Timer1_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  PeriodicTask = task;          // user function
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
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
  (*PeriodicTask)();                // execute user task
}

void Timer2_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  PeriodicTask = task;          // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
  (*PeriodicTask)();                // execute user task
}

void Timer3_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate TIMER3
  PeriodicTask = task;          // user function
  TIMER3_CTL_R = 0x00000000;    // 1) disable TIMER3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear TIMER3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 51, interrupt number 35
  NVIC_EN1_R = 1<<(35-32);      // 9) enable IRQ 35 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable TIMER3A
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER3A timeout
  (*PeriodicTask)();                // execute user task
}


volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  // DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

uint32_t ADC_buff[1000];
uint32_t time_buff[1000];
int idx = 0;
uint32_t hist[4096] = {0};

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
	if (idx < 1000){
		 ADCvalue = ADC0_InSeq3();
		 ADC_buff[idx] = ADCvalue;
		time_buff[idx] = TIMER1_TAR_R;
		idx ++;
	}
  PF2 ^= 0x04;                   // profile
}

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int dy,dx,m,i;
	uint16_t y,x,y_end,x_end;
	if (abs(y1-y2)<abs(x1-x2)) {
		if (x2>x1) {
			dx = x2-x1;
			dy = y2-y1;
			x = x1;
			x_end = x2;
			y = y1;
			y_end = y2;
			i = 1;
			if (dy<0) {
				dy = -dy;
				i = -1;
			}
			m = 2*dy-dx;
			while (x<x_end) {
				ST7735_DrawPixel(x,y,color);
				if (m>0) {
					x = x + i;
					m = m -2*dx;
				}
				m = m + 2*dy;
				x += 1;
			}
		}
		else {
			dx = x1-x2;
			dy = y1-y2;
			x = x2;
			x_end = x1;
			y = y2;
			y_end = y1;
			i = 1;
			if (dy<0) {
				dy = -dy;
				i = -1;
			}
			m = 2*dy-dx;
			while (x<x_end) {
				ST7735_DrawPixel(x,y,color);
				if (m>0) {
					x = x + i;
					m = m -2*dx;
				}
				m = m + 2*dy;
				x += 1;
			}
		}
	}
	else {
		if (y2>y1) {
			dx = x2-x1;
			dy = y2-y1;
			x = x1;
			x_end = x2;
			y = y1;
			y_end = y2;
			i = 1;
			if (dx<0) {
				dx = -dx;
				i = -1;
			}
			m = 2*dx-dy;
			while (y<y_end) {
				ST7735_DrawPixel(x,y,color);
				if (m>0) {
					x = x + i;
					m = m -2*dy;
				}
				m = m + 2*dx;
				y += 1;
			}
		}
		else {
			dx = x1-x2;
			dy = y1-y2;
			x = x2;
			x_end = x1;
			y = y2;
			y_end = y1;
			i = 1;
			if (dx<0) {
				dx = -dx;
				i = -1;
			}
			m = 2*dx-dy;
			while (x<x_end) {
				ST7735_DrawPixel(x,y,color);
				if (m>0) {
					x = x + i;
					m = m - 2*dy;
				}
				m = m + 2*dx;
				y += 1;
			}
		}
	}
}

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  ADC0_SAC_R |= 0x0;
	Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  Timer1_Init(Timer1A_Handler, 0xFFFFFFFF);
	Timer2_Init(Timer2A_Handler, 7920);
	Timer3_Init(Timer3A_Handler, 7920);
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
  while(1){
		PF1 = (PF1*12345678)/1234567+0x02;  // this line causes jitter
		if(idx >= 1000){
			break;
		}
    PF1 ^= 0x02;  // toggles when running in main
  }
	DisableInterrupts	();

	uint32_t greatestDiff = 0;
	uint32_t smallestDiff = 0xFFFFFFFF;
	for(int i = 0; i <999; i++){
		int diff = time_buff[i] - time_buff[i+1];
		if (diff < smallestDiff) smallestDiff = diff;
		if (diff > greatestDiff) greatestDiff = diff;
	}
	
	volatile int Jitter = greatestDiff - smallestDiff;
	for(int i = 0; i < 4096; i++){
		hist[i] = 0;
	}

	for (int i = 0; i < 1000; i++) {
		hist[ADC_buff[i]] += 1;
	}
	
	for (int i = 0; i < 4096; i++) {
		if(hist[i] != 0){
			int a = 3;
		}
	}	
	return 0;
}

