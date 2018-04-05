// 1) There are three ways we can use to initiate an ADC conversion process
// SIGMA DELTA: There are Vin and V0, where V0 is presented in a digital fashion,
// meaning its information is encoded in its duty cycle. We then get V2, the average difference
// between Vin and V0, and then we drive V2 to 0 using a feedback loop. Then with some digital
// logic, the digital signal will be produced
// SUCCESSIVE APPROXIMATION: With a clock. We need a DAC of the precision. we do something like 
// a binary search to "guess" the real value. We use it when we are interested in accuracy, but it
// is typically slow.
// FLASH: pretty much a decoder. Vin is compared to all possible values in parallel. Therefore, this
// type of ADC doesn't scale with speed, only in cost.

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
	UART_OutString("JA");
  while(1){
    // GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED
  }
}
