#include "input.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Switch.h"



#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PF4       (*((volatile uint32_t *)0x40025040))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define SWITCHES  (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board

void input_init(void){ 
//	uint32_t status;
//  Switch_Init();           // PA5 is input
//  status = Switch_Input(); // 0x00 or 0x20
//  status = Switch_Input(); // 0x00 or 0x20
  
  Board_Init();             // initialize PF0 and PF4 and make them inputs
                            // make PF3-1 out (PF3-1 built-in LEDs)
  GPIO_PORTF_DIR_R |= (RED|BLUE|GREEN); // disable alt funct on PF3-1
  GPIO_PORTF_AFSEL_R &= ~(RED|BLUE|GREEN); // enable digital I/O on PF3-1
  GPIO_PORTF_DEN_R |= (RED|BLUE|GREEN); // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;     // disable analog functionality on PF
}

uint32_t getInput(void){
	return Board_Input();
}

void testRun(void){
	uint32_t status;
  while(1){
    status = Board_Input();
    switch(status){                    // switches are negative logic on PF0 and PF4
      case 0x01: LEDS = BLUE; break;   // SW1 pressed
      case 0x10: LEDS = RED; break;    // SW2 pressed
      case 0x00: LEDS = GREEN; break;  // both switches pressed
      case 0x11: LEDS = 0; break;      // neither switch pressed
      default: LEDS = (RED|GREEN|BLUE);// unexpected return value
    }
  }
}
