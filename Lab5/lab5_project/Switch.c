//switch.c
//drivers for switch interfacing

#include <stdint.h>
#include "TimeUtil.h"
#include "Music.h"
#include "DAC.h"
#include "..//inc//tm4c123gh6pm.h"

#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define PA5                     (*((volatile uint32_t *)0x40004080))
#define PE0											(*((volatile uint32_t *)0x40024004))
#define PE1											(*((volatile uint32_t *)0x40024008))
#define PE2											(*((volatile uint32_t *)0x40024010))
#define PE3											(*((volatile uint32_t *)0x40024020))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control

extern uint16_t soundFlag;
extern uint16_t resetFlag;
extern uint16_t pauseFlag;
extern uint16_t instrumentFlag;

void Switch_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x00000010;     // 1) activate clock for Port A
  while((SYSCTL_PRGPIO_R&0x10) == 0){};// ready?
  GPIO_PORTE_DIR_R &= ~0x0F;        // 3) direction PA5 input
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // 4) PA5 regular port function
  GPIO_PORTE_DEN_R |= 0x0F;         // 5) enable PA5 digital port
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 6) disable analog on PA5
  GPIO_PORTE_PCTL_R &= ~0x000F0F00; // 7) PCTL GPIO on PA5
  GPIO_PORTE_PUR_R |= 0x0F;         // 8) Enable weak pull up
  GPIO_PORTE_IS_R &= ~0x0F;         // 9) PE0-PE2 is edge sensitive
  GPIO_PORTE_IBE_R &= ~0x0F;        // 10) PE0-PE2 is not both edges
  GPIO_PORTE_IEV_R &= ~0x0F;        // 11) PE0-PE2 falling edge
  GPIO_PORTE_ICR_R = 0x0F;          // 12) clear flag
  GPIO_PORTE_IM_R |= 0x0F;          // 13) arm interrupts
  NVIC_PRI1_R = (NVIC_PRI1_R&0XFFFFFF00)|0x00000040; // 14) priority 2
  NVIC_EN0_R = 0x0000010;           // 15) Enable interurpt 30 in NVIC
}

void GPIOPortE_Handler() {
	int interrupt_trigger = GPIO_PORTE_MIS_R;
	int delay = 5000;
	while (delay) {
		delay--;
	}
	if(interrupt_trigger&0x01) {
		if (!PE0) {
			GPIO_PORTE_ICR_R = 0x01;
			if (pauseFlag==0) pauseFlag = 1;
			else pauseFlag = 0;
		}
	}
	if (interrupt_trigger&0x02) {
		if (!PE1) {
			GPIO_PORTE_ICR_R = 0x02;
			resetFlag = 1;
			pauseFlag = 1;
		}
	}
	if (interrupt_trigger&0x03) {
		if (!PE2) {
			instrumentFlag = (instrumentFlag + 1)%3;
		}
	}
}
