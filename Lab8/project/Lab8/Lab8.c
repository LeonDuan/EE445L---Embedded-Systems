#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer.h"
#include "Sound.h"
#include "Game.h"
#include "Proximity.h"
#include "Switch.h"

void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

void test(void) {
	GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
}

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

int main(void) {
	PLL_Init(Bus80MHz);         // set system clock to 50 MHz
	PortF_Init();
	Timer3A_Init(&test, 50000000/16);
	EnableInterrupts();
  while(1){
    WaitForInterrupt();
  }
}
