#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Timer.h"
#include "Sound.h"
#include "Game.h"
#include "Proximity.h"
#include "Switch.h"
#include "Graphics.h"

void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void DisableInterrupts(void); // Disable interrupts

uint16_t cur_stage = 1;
void test(void) {
	GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R^0x04; // toggle PF2
	cur_stage ++;
	if (cur_stage == 4) cur_stage = 1;
	draw_Explosion(cur_stage, 50,50);
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
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	PortF_Init();
	DisableInterrupts();
	Timer0A_Init(&test, 40000000);
	Init_Graphics();
	draw_Main_Ship(20, 20);
	draw_Enemy_Ship(1, 80, 50);
	draw_Enemy_Ship(2, 50, 80);
	draw_Enemy_Ship(3, 110, 100);
	draw_Boss(1, 80, 60);
	draw_Boss(2, 160, 60);
	
	EnableInterrupts();
  while(1){
    WaitForInterrupt();
  }
}
