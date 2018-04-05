// 1) 1. Rising/Falling 2. signal period trigger 3. signal pulse trigger
// 2) if We set the IE0 bit, the INR3 flag in the ADC0_RIS_R register will be set when the ADC conversion is complete

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "UART.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


int main(void){
  PLL_Init(Bus80MHz);                      // 80 MHz system clock
  UART_Init();
	ADC0_InitTimer0ATriggerSeq3(0,80000);
  EnableInterrupts();
  while(1){
    GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED
  }
}
