// lab3Main.c
// Runs on LM4F120/TM4C123
// Test the switch initialization functions by setting the LED
// color according to the status of the switches.
// Daniel and Jonathan Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 2.3, Program 2.9, Figure 2.36

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

// negative logic switches connected to PF0 and PF4 on the Launchpad
// red LED connected to PF1 on the Launchpad
// blue LED connected to PF2 on the Launchpad
// green LED connected to PF3 on the Launchpad
// NOTE: The NMI (non-maskable interrupt) is on PF0.  That means that
// the Alternate Function Select, Pull-Up Resistor, Pull-Down Resistor,
// and Digital Enable are all locked for PF0 until a value of 0x4C4F434B
// is written to the Port F GPIO Lock Register.  After Port F is
// unlocked, bit 0 of the Port F GPIO Commit Register must be set to
// allow access to PF0's control registers.  On the LM4F120, the other
// bits of the Port F GPIO Commit Register are hard-wired to 1, meaning
// that the rest of Port F can always be freely re-configured at any
// time.  Requiring this procedure makes it unlikely to accidentally
// re-configure the JTAG pins as GPIO, which can lock the debugger out
// of the processor and make it permanently unable to be debugged or
// re-programmed.

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "Switch.h" // input
#include "Display.h" // graphics
#include "ST7735.h"
#include "timeUtils.h" // time utilities
#include "speaker.h" // speaker




#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define PE0				(*((volatile uint32_t *)0x40024004))
#define PE1				(*((volatile uint32_t *)0x40024008))
#define PE2				(*((volatile uint32_t *)0x40024010))
#define PE3				(*((volatile uint32_t *)0x40024020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board

#define ANALOG_CLOCK		0
#define DIGITAL_CLOCK		1
#define ALARM_CLOCK			2

//GLOBAL VARIABLES//
uint32_t currentMode = 0;
uint32_t prevMode = 0;
uint32_t setTimeFlag = 0;
uint32_t currentHour = 0;
uint32_t currentMinute = 0;
uint32_t currentSeconds = 0;
uint32_t prevHour = 0;
uint32_t prevMinute = 0;
uint32_t prevSeconds = 0;
uint32_t setAlarmFlag = 0;
uint32_t alarmHour = 0;
uint32_t alarmMinute = 0;
uint32_t alarmActive = 0;
uint32_t soundFlag = 0;
uint32_t changeModeFlag = 0;
uint32_t updateTimeFlag = 1;
uint32_t changeThemeFlag = 0;
uint16_t THEME_COLOR = 0x0000;
uint16_t theme_index = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void PortF_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

void PortD_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x01;         // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x01;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0x01;         // 7) enable digital I/O on PD3-0
}

void wait(uint32_t time){
	// PF2 ^= 0x04;
	while (time) {
		time--;
	}
	// PF2 ^= 0x04;
}



void setTime(void) {
	DisableInterrupts();
	draw_DigitalClock(currentHour, currentMinute, THEME_COLOR);

	wait(2000000);
	int newHour1 = currentHour / 10;
	int newHour2 = currentHour % 10;
	int newMinute1 = currentMinute / 10;
	int newMinute2 = currentMinute % 10;
	
	int prevPosition = -1;
	int curPosition = 0;

	while(1){
		
		// change arrow position
		if (prevPosition != curPosition) {
			clear_Arrow_Area();
			switch(curPosition){
				case 0:
					draw_Arrow(7, 120);
					break;
				case 1:
					draw_Arrow(33, 120);
					break;
				case 2:
					draw_Arrow(78, 120);
					break;
				case 3:
					draw_Arrow(101, 120);
					break;
			}
		}
		prevPosition = curPosition;
		
		// increment time
		if(!PE1) {
			switch(curPosition){
				case 0:
					newHour1 = (newHour1 + 1) % 3;
					ST7735_DrawChar(5,80,newHour1 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 1:
					newHour2 = (newHour2 + 1) % 10;
					ST7735_DrawChar(32,80,newHour2 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 2:
					newMinute1 = (newMinute1 + 1) % 7;
					ST7735_DrawChar(76,80,newMinute1 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 3:
					newMinute2 = (newMinute2 + 1) % 10;
					ST7735_DrawChar(100,80,newMinute2 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
			}
		}

		wait(2500000);
		if (!PE2){
			curPosition = (curPosition + 1) % 4;
		}
		if(!PE0) {
			clear_Arrow_Area();
			int newHour = newHour1 * 10 + newHour2;
			int newMinute = newMinute1 * 10 + newMinute2;
			int totalMinute = newHour * 60 + newMinute;
			if (totalMinute >= 1440){
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nWon't be saved.");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nWon't be saved.");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nWon't be saved.");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
			}
			else{
				currentHour = newHour;
				currentMinute = newMinute;
			}
			ST7735_SetCursor(0,0);
			ST7735_OutString("Digital Clock");
			break;
		}
	}
	setTimeFlag = 0;
	EnableInterrupts();
}




void setAlarm(void) {
	GPIO_PORTE_IM_R &= ~0x0F; //disarm PortE interrupts 
	draw_DigitalClock(alarmHour, alarmMinute, THEME_COLOR);
	draw_Alarm_Status(55, 125);
	wait(2000000);

	int alarmHour1 = alarmHour / 10;
	int alarmHour2 = alarmHour % 10;
	int alarmMinute1 = alarmMinute / 10;
	int alarmMinute2 = alarmMinute % 10;
	

	int prevPosition = -1;
	int curPosition = 0;

	while(1){
		
		// change arrow position
		if (prevPosition != curPosition) {
			clear_Arrow_Area();
			switch(curPosition){
				case 0:
					draw_Arrow(7, 120);
					break;
				case 1:
					draw_Arrow(33, 120);
					break;
				case 2:
					draw_Arrow(78, 120);
					break;
				case 3:
					draw_Arrow(101, 120);
					break;
				case 4:
					draw_Alarm_Status_Arrow(37,120);
					break;
			}
		}
		prevPosition = curPosition;
		
		// increment time
		if(!PE1) {
			switch(curPosition){
				case 0:
					alarmHour1 = (alarmHour1 + 1) % 3;
					ST7735_DrawChar(5,80,alarmHour1 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 1:
					alarmHour2 = (alarmHour2 + 1) % 10;
					ST7735_DrawChar(32,80,alarmHour2 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 2:
					alarmMinute1 = (alarmMinute1 + 1) % 7;
					ST7735_DrawChar(76,80,alarmMinute1 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 3:
					alarmMinute2 = (alarmMinute2 + 1) % 10;
					ST7735_DrawChar(100,80,alarmMinute2 + '0',THEME_COLOR,ST7735_Color565(228,228,228),4);
					break;
				case 4:
					alarmActive = !alarmActive;
					draw_Alarm_Status(55, 125);
					break;
			}
		}

		wait(2500000);
		if (!PE2){
			curPosition = (curPosition + 1) % 5;
		}
		if(!PE0) {
			clear_Arrow_Area();
			int newAlarmHour = alarmHour1 * 10 + alarmHour2;
			int newAlarmMinute = alarmMinute1 * 10 + alarmMinute2;
			int totalMinute = newAlarmHour * 60 + newAlarmMinute;
			if (totalMinute >= 1440){
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nAlarm not set");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nAlarm not set");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
				ST7735_SetCursor(0,0);
				ST7735_OutString("Invalid Time. \nAlarm not set");
				wait(16000000);
				ST7735_FillRect(0, 0, 128, 32, 0);
				wait(16000000);
			}
			else{
				alarmHour = alarmHour1 * 10 + alarmHour2;
				alarmMinute = alarmMinute1 * 10 + alarmMinute2;
			}
			ST7735_SetCursor(0,0);
			ST7735_OutString("Set Alarm");
			break;
		}
	}
	setAlarmFlag = 0;	
	GPIO_PORTE_IM_R |= 0x0F;          // 13) arm interrupts
}

int hourHandXCoords[360] = {63, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 80, 81, 81, 81, 82, 82, 82, 82, 83, 83, 83, 83, 84, 84, 84, 84, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 88, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 86, 86, 86, 86, 86, 86, 86, 85, 85, 85, 85, 85, 84, 84, 84, 84, 83, 83, 83, 83, 82, 82, 82, 82, 81, 81, 81, 80, 80, 80, 80, 79, 79, 79, 78, 78, 78, 77, 77, 76, 76, 76, 75, 75, 75, 74, 74, 73, 73, 73, 72, 72, 71, 71, 71, 70, 70, 69, 69, 69, 68, 68, 67, 67, 66, 66, 66, 65, 65, 64, 64, 63, 63, 63, 62, 62, 61, 61, 60, 60, 59, 59, 59, 58, 58, 57, 57, 56, 56, 56, 55, 55, 54, 54, 54, 53, 53, 52, 52, 52, 51, 51, 50, 50, 50, 49, 49, 49, 48, 48, 47, 47, 47, 46, 46, 46, 45, 45, 45, 45, 44, 44, 44, 43, 43, 43, 43, 42, 42, 42, 42, 41, 41, 41, 41, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39, 39, 39, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62};
int hourToXCoord(int hour, int minute){
	return hourHandXCoords[(hour % 12) * 30 + minute / 2];
}

int hourHandYCoords[360] = {77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 78, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 79, 80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 82, 82, 82, 82, 83, 83, 83, 83, 84, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95, 96, 96, 97, 97, 97, 98, 98, 98, 99, 99, 99, 100, 100, 100, 101, 101, 101, 102, 102, 102, 103, 103, 103, 104, 104, 104, 105, 105, 105, 106, 106, 106, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 110, 110, 110, 110, 111, 111, 111, 111, 112, 112, 112, 112, 112, 113, 113, 113, 113, 113, 114, 114, 114, 114, 114, 114, 115, 115, 115, 115, 115, 115, 115, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 117, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 116, 115, 115, 115, 115, 115, 115, 115, 114, 114, 114, 114, 114, 114, 113, 113, 113, 113, 113, 112, 112, 112, 112, 112, 111, 111, 111, 111, 110, 110, 110, 110, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 106, 106, 106, 105, 105, 105, 104, 104, 104, 103, 103, 103, 102, 102, 102, 101, 101, 101, 100, 100, 100, 99, 99, 99, 98, 98, 98, 97, 97, 97, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 91, 90, 90, 90, 89, 89, 89, 88, 88, 88, 87, 87, 87, 87, 86, 86, 86, 85, 85, 85, 84, 84, 84, 84, 83, 83, 83, 83, 82, 82, 82, 82, 81, 81, 81, 81, 81, 80, 80, 80, 80, 80, 79, 79, 79, 79, 79, 79, 78, 78, 78, 78, 78, 78, 78, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77};
int hourToYCoord(int hour, int minute){
	return hourHandYCoords[(hour % 12) * 30 + minute / 2];
}

int main(void){
	PLL_Init(Bus80MHz);   
	PortF_Init(); // initialize port F (LED outputs)
	PortD_Init();	// initialize port D (Speaker output)
	Switch_Init(); // initialize inputs
	SysTick_Init(); // initialize SysTick
	Timer0A_Init();
	init_LCD();	//initialize LCD
	draw_AnalogClock();
	// enableSpeaker(1);
	
	EnableInterrupts();
	
	while(1) {

		if (changeModeFlag) {
			currentMode = (currentMode+1)%3;
			changeModeFlag = 0;
		}
		switch(currentMode) {
			case ANALOG_CLOCK:
				if (prevMode != currentMode || updateTimeFlag || changeThemeFlag) {
					if(prevMode != currentMode) {
						clear_Screen("Analog Clock");
						draw_AnalogClock();
					}
					if(changeThemeFlag){
						theme_index = (theme_index+1)%7;
						THEME_COLOR = Themes[theme_index];
					}
					static int lastHourX = 63;
					static int lastHourY = 63;
//					int prevHourHandx = hourToXCoord(prevHour, prevMinute);
//					int prevHourHandy = hourToYCoord(prevHour, prevMinute);
					int curHourHandX = hourToXCoord(currentHour, currentMinute);
					int curHourHandY = hourToYCoord(currentHour, currentMinute);
					PF2 ^= 0x04;
					PF2 ^= 0x04;
					draw_ClockHand(CLOCK_ORIGIN, Minute_Hand[prevMinute].x,Minute_Hand[prevMinute].y,ST7735_Color565(228,228,228));
					draw_ClockHand(CLOCK_ORIGIN, lastHourX,lastHourY,ST7735_Color565(228,228,228));
					draw_ClockHand(CLOCK_ORIGIN, Minute_Hand[currentMinute].x,Minute_Hand[currentMinute].y,THEME_COLOR);
					prevMinute = currentMinute;
					draw_ClockHand(CLOCK_ORIGIN, curHourHandX,curHourHandY,THEME_COLOR);
					PF2 ^= 0x04;
					prevHour = currentHour;
					updateTimeFlag = 0;
					lastHourX = curHourHandX;
					lastHourY = curHourHandY;
					changeThemeFlag = 0;
				}
				break;
			case DIGITAL_CLOCK:
				
				if (prevMode != currentMode || updateTimeFlag || changeThemeFlag) { 
					if(changeThemeFlag){
						theme_index = (theme_index+1)%7;
						THEME_COLOR = Themes[theme_index];
					}
					if(prevMode != currentMode){
						clear_Screen("Digital Clock");
					}
					draw_DigitalClock(currentHour, currentMinute, THEME_COLOR);
					changeThemeFlag = 0;
					updateTimeFlag = 0;
				}
				if(setTimeFlag){
					setTime();
				}
				break;
			case ALARM_CLOCK:
				if (prevMode != currentMode || changeThemeFlag) {
					if(changeThemeFlag){
						theme_index = (theme_index+1)%7;
						THEME_COLOR = Themes[theme_index];
						changeThemeFlag = 0;
					}
					clear_Screen("Set Alarm");
					draw_DigitalClock(alarmHour, alarmMinute, THEME_COLOR);
					draw_Alarm_Status(55,125);
				}
				if(setAlarmFlag){
					setAlarm();
				}
				break;
		}
		
		prevMode = currentMode;
	}
}
