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
