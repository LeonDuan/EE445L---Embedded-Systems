#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Timer.h"
#include "Sound.h"
#include "Game.h"
#include "Proximity.h"
#include "Switch.h"
#include "Graphics.h"
#include "SysTick.h"
#include "UART.h"

void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void DisableInterrupts(void); // Disable interrupts


// for heartbeat
void PortF_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;  // activate port F
	while ((SYSCTL_RCGCGPIO_R & 0x20) == 0){}
  GPIO_PORTF_DIR_R |= 0x04;   // make PF2 out (built-in blue LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;// disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;   // enable digital I/O on PF2
                              // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF 
}

int Should_Terminate() {
		if (Is_GameOver()) {
			// TODO
			return 1;
		}
		else if (Is_LevelWon()) {
			// TODO
			return 1;
		}
		else if (Is_GameWon()) {
			// TODO
			return 1;
		}
		return 0;
}

int main(void) {
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	
	DisableInterrupts();
	PortF_Init();								// heartbeat
	Proximity_Init();
	Init_Graphics();
	
	SysTick_Init();
	EnableInterrupts();
//	Sound_Play(1);
	while(!Should_Terminate()) {
		
	}
}
