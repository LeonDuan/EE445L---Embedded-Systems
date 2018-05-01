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

int enemyMultiplier = 1;
int Update_Enemies_Counter = 10;
int Update_Boss_Counter = 5;
int Update_My_Ship_Counter = 5;
int Add_My_Bullets_Counter = 40;
int Add_Enemy_Bullets_Counter = 60;
void Update_Objects(void) {
	if (Update_Enemies_Counter == 0) {
		Update_Enemies_Counter = DEFAULT_ENEMY_COUNTER/enemyMultiplier;
	}		
	if (Update_Boss_Counter == 0) {
		Update_Boss_Counter = DEFAULT_BOSS_COUNTER/enemyMultiplier;
	}
	if (Update_Ene

	Update_Enemies_Counter --;
	Update_Boss_Counter--;
	Update_My_Ship_Counter--;
	Update_My_Ship_Counter--;
	Add_Enemy_Bullets_Counter--;
}

int main(void) {
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	
	DisableInterrupts();
	PortF_Init();								// heartbeat
	Proximity_Init();
	Init_Graphics();
	SysTick_Init();
	
	Timer5A_Init(&Update_Objects, 4000000);
	EnableInterrupts();
//	Sound_Play(1);
	while(!Should_Terminate()) {
		Update_Screen();
	}
}
