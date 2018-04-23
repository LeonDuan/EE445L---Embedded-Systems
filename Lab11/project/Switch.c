#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Switch.h"
#include "Timer.h"

volatile static unsigned long Touch;     // true on touch
volatile static unsigned long Release;   // true on release
volatile static unsigned long Last;      // previous
void (*TouchTask_PE1)(void);    // user function to be executed on touch
void (*TouchTask_PE2)(void);  // user function to be executed on release

void PortE_Arm(void){
	GPIO_PORTE_ICR_R = 0x06;      // (e) clear flag 1,2
  GPIO_PORTE_IM_R |= 0x06;      // (f) arm interrupt on PE1,2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000080; // (g) priority 4
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 4 in NVIC
}

void Switch_Init(void(*touchtask_PE1)(void), void(*touchtask_PE2)(void)){
  // **** general initialization ****
  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
  while((SYSCTL_PRGPIO_R & 0x00000010) == 0){};
  GPIO_PORTE_DIR_R &= ~0x06;    // (c) make PE1,2 in
  GPIO_PORTE_AFSEL_R &= ~0x06;  //     disable alt funct on PE1,2
  GPIO_PORTE_DEN_R |= 0x06;     //     enable digital I/O on PE1,2   
  GPIO_PORTE_PCTL_R &= ~0x00000FF0; // configure PE1,2 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_PUR_R |= 0x06;     //     enable weak pull-up on PE1,2
  GPIO_PORTE_IS_R &= ~0x06;     // (d) PE1,2 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x06;     //    PE1,2 is not both edges
	GPIO_PORTE_IEV_R |= 0x06;			// 		 PE1,2 is rising edge

	PortE_Arm();
		
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate TIMER2
  TouchTask_PE1 = touchtask_PE1;           // user function 
  TouchTask_PE2 = touchtask_PE2;       // user function 
}

void TimerTask_2A (void){
	TIMER2_IMR_R &= ~0x00000001;    // disarm timeout interrupt
  PortE_Arm();   // start GPIO
}

void GPIOPortE_Handler(void){
	int interrupt_trigger = GPIO_PORTE_MIS_R;
	GPIO_PORTE_IM_R &= ~0x06;
	if (interrupt_trigger == 0x02) (*TouchTask_PE1)();
	if (interrupt_trigger == 0x04) (*TouchTask_PE2)();
	Timer2A_Arm(&TimerTask_2A, 800000);
}
