#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"

uint32_t start;
uint32_t currentPos;

void Proximity_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD0-1
  GPIO_PORTD_AMSEL_R &= ~0x03;      // 3) disable analog functionality on PD0-1
  GPIO_PORTD_PCTL_R &= ~0x000000FF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x01;         // 5a) make PD0 out
  GPIO_PORTD_DIR_R &= ~0x02;        // 5b) make PD1 in
  GPIO_PORTD_AFSEL_R &= ~0x03;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0x03;         // 7) enable digital I/O on PD3-0
	GPIO_PORTD_PUR_R |= 0x02;					// 8) enable weak pull-up on PD1
	GPIO_PORTD_IS_R &= ~0x02;					// 9) PD1 is edge-sensitive
	GPIO_PORTD_IBE_R |= 0x02;    //     PD1 is both edges
//  GPIO_PORTD_IEV_R |= 0x02;    //     PF4 falling edge event
  GPIO_PORTD_ICR_R = 0x02;      // (e) clear flag4
	GPIO_PORTD_IM_R |= 0x02;
	NVIC_PRI0_R = (NVIC_PRI0_R&0x00FFFFFF)|0xA0000000; // (g) priority 5
  NVIC_EN0_R |= 0x08;      // (h) 
}

int count = 0;
void GPIOPortD_Handler(void){
	volatile uint32_t data = GPIO_PORTD_DATA_R;
	if ((data & 0x02) == 0) {
		volatile uint32_t current = NVIC_ST_CURRENT_R;
		uint32_t elapsed = ((start - current) * 125 / 10) / 1000; //
		uint32_t pos = elapsed * 340 / 1000 / 2;
		if (pos > 10 && pos < 300) {
			currentPos = pos;
		}
	}
	else {
		start = NVIC_ST_CURRENT_R;
	}
	GPIO_PORTD_ICR_R |= 0x02;      // acknowledge flag4
}

void requestEcho(void) {
	GPIO_PORTD_DATA_R |= 0x01;			// Trigger Ultrasonic Sensor
	SysTick_Wait(1000);							// 12.5 us (the sensor requires >10 us)
	GPIO_PORTD_DATA_R &= ~0x01;			// Turn off trigger
}

uint32_t getCurrentHandPosition(void) {
	return currentPos;
}
