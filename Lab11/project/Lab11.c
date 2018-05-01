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

#define 			DEFAULT_ENEMY_COUNTER							5
#define				DEFAULT_BOSS_COUNTER							5
#define 			DEFAULT_ADD_ENEMY_BULLET_COUNTER	80
#define 			DEFAULT_ADD_MY_BULLETS_COUNTER    20
#define				DEFAULT_ADD_BOSS_BULLET_COUNTER		80
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
int Update_Enemies_Counter = 1;
int Update_Boss_Counter = 5;
int Update_My_Ship_Counter = 5;
int Add_My_Bullets_Counter = 20;
int Add_Enemy_Bullets_Counter = 80;
int Add_Boss_Bullets_Counter = 80;
int Update_Bullets_Counter = 1;

void Update_Objects(void) {
	if (Update_Enemies_Counter == 0) {
		Update_Enemies_Counter = DEFAULT_ENEMY_COUNTER/enemyMultiplier;
	}		
	if (Update_Boss_Counter == 0) {
		Update_Boss_Counter = DEFAULT_BOSS_COUNTER/enemyMultiplier;
	}
	if (Add_My_Bullets_Counter == 0) {
		Add_My_Bullets_Counter = DEFAULT_ADD_MY_BULLETS_COUNTER;
	}
	if (Add_Enemy_Bullets_Counter == 0) {
		Add_Enemy_Bullets_Counter = DEFAULT_ADD_ENEMY_BULLET_COUNTER/enemyMultiplier;
	}
	if (Add_Boss_Bullets_Counter == 0) {
		Add_Enemy_Bullets_Counter = DEFAULT_ADD_BOSS_BULLET_COUNTER/enemyMultiplier;
	}
	if (Update_Bullets_Counter == 0) {
		Update_Bullets_Counter = 1;
	}
	Update_Bullets_Counter --;
	Update_Enemies_Counter --;
	Update_Boss_Counter --;
	Add_Enemy_Bullets_Counter --;
	Add_Boss_Bullets_Counter --;
	Update_My_Ship_Counter --;
	Add_My_Bullets_Counter --;
}

void simulate(void) {
	Init_Level(1);
	Draw_My_Ship();
	Draw_Enemies();
	while(1){
		if (Update_Enemies_Counter == 0) {
			Update_Enemies();
			Check_Hit();
			Draw_Enemies();
		}
		if (Update_Boss_Counter == 0 && Get_Stage() == 2) {
			Update_Boss();
			Check_Hit();
			Draw_Boss();
		}
		if (Update_My_Ship_Counter == 0) {
			Update_My_Ship(getCurrentHandPosition());
			Check_Hit();
			Draw_My_Ship();
		}
		if (Add_My_Bullets_Counter == 0) {
			Add_Bullets(0);
			Check_Hit();
			Draw_Bullets();
		}
		if (Add_Enemy_Bullets_Counter == 0 && Get_Stage() == 1) {
			Add_Bullets(1);
			Check_Hit();
			Draw_Bullets();
		}
		if (Add_Boss_Bullets_Counter == 0 && Get_Stage() == 2) {
			Add_Bullets(2);
			Check_Hit();
			Draw_Bullets();
		}
		if (Update_Bullets_Counter == 0) {
			Update_My_Bullets();
			Update_Enemy_Bullets();
			if (Check_Hit()) Draw_Enemies();;
			Draw_Bullets();
		}
	}
}

int current_screen = 1; // 1 for home page that lets you choose to calibrate or game, 2 for gaming 
int middle = 200;
void dummy(void){}

int main(void) {
	PLL_Init(Bus80MHz);         // set system clock to 80 MHz
	
	DisableInterrupts();
	PortF_Init();								// heartbeat
	Proximity_Init();
	Init_Graphics();
	SysTick_Init();
	Switch_Init(&dummy, &dummy);
	
	Timer5A_Init(&Update_Objects, 4000000);
	EnableInterrupts();
//	Sound_Play(1);
//	while(1) {
////		if (current_screen == 1) {
////			ST7735_OutString("Yellow to game\n");
////			ST7735_OutString("Green to calibrate");
////			if ((GPIO_PORTE_DATA_R & 0x02) != 0) current_screen = 2;
////			else if ((GPIO_PORTE_DATA_R & 0x04) != 0) {
////				middle = getCurrentHandPosition();
////				ST7735_SetCursor(0,10);
////				ST7735_OutString("Calibrated!");
////				SysTick_Wait(100000000);
////				ST7735_SetCursor(0,10);
////				ST7735_OutString("           ");
////			}
////		}
////		else {
//////			simulate();
////			// Draw_Dummy();
////		}
	simulate();
}

