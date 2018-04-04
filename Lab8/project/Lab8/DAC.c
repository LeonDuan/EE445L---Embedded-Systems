// SSI1Clk (SCLK, pin 2) connected to PF2
// SSI1Fss (!CS, pin 3) connected to PF3
// SSI1Tx (DIN, pin 1) connected to PF1
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//void DAC_Init(void){
//  SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI1
//  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
//  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
//  GPIO_PORTF_AFSEL_R |= 0x06;     // enable alt funct on PF1,2 regular GPIO on PF3
//  GPIO_PORTF_DEN_R |= 0x0E;       // configure PF1,2 as SSI, PF3 is regular GPIO
//  GPIO_PORTF_DATA_R |= 0x08;      // PF3 FSS high
//  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000440;
//  GPIO_PORTF_AMSEL_R = 0;         // disable analog functionality on PF
//  SSI1_CR1_R = 0x00000000;        // disable SSI, master mode
//  SSI1_CPSR_R = 0x02;             // 8 MHz SSIClk 
//  SSI1_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
//  SSI1_CR0_R |= 0x0F;             // DSS = 16-bit data
//  SSI1_CR1_R |= 0x00000002;       // enable SSI
//}

void DAC_Init(void){
  SYSCTL_RCGCSSI_R |= 0x02;       // activate SSI1
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};// ready?
  GPIO_PORTF_AFSEL_R |= 0x0E;     // enable alt funct on PF1,2,3
  GPIO_PORTF_DEN_R |= 0x0E;       // configure PF1,2,3
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00002220;
  GPIO_PORTF_AMSEL_R = 0;         // disable analog functionality on PF
  SSI1_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI1_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI1_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI1_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI1_DR_R = 0;									// load 'data' into transmit FIFO
	SSI1_CR1_R |= 0x00000002;       // enable SSI
}


void DAC_Out(uint32_t data){
	GPIO_PORTF_DATA_R &= ~0x08;        // PF3 FSS low
  while((SSI1_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI1_DR_R = data;                  // data out
  GPIO_PORTF_DATA_R |= 0x08;         // PF3 FSS high
}
