#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "PWM.h"
#include "UART.h"

#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define TIMER_TAMR_TACMR        0x00000004  // GPTM TimerA Capture Mode
#define TIMER_TAMR_TAMR_CAP     0x00000003  // Capture mode
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_CTL_TAEVENT_POS   0x00000000  // Positive edge
#define TIMER_CTL_TAEVENT_NEG   0x00000004  // Negative edge
#define TIMER_CTL_TAEVENT_BOTH  0x0000000C  // Both edges
#define TIMER_IMR_CAEIM         0x00000004  // GPTM CaptureA Event Interrupt Mask
#define TIMER_ICR_CAECINT       0x00000004  // GPTM CaptureA Event Interrupt Clear
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load Register Low
#define STABILITY_K							20

uint32_t Period;              // (1/clock) units
uint32_t First;               // Timer0A first edge
int32_t Done;                 // set each rising
extern uint32_t Speed;				// motor speed in 0.1 rps
int32_t E;          			 	  // speed error in 0.1 rps
int32_t U;         				    // duty cycle 40 to 39960
uint32_t PID_delay = 2;

extern uint32_t Target_Speed;

uint32_t Tach_Read(){
	return Period;
}

uint32_t Tach_Speed(){
	return 200000000/Period;
}

void Tach_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x01;// activate timer0    
  SYSCTL_RCGCGPIO_R |= 0x02;       // activate port B and port F
	while((SYSCTL_PRGPIO_R&0x02) == 0){};// ready?

                                   // allow time to finish activating
  First = 0;                       // first will be wrong
  GPIO_PORTB_DIR_R &= ~0x40;       // make PB6 in
  GPIO_PORTB_AFSEL_R |= 0x40;      // enable alt funct on PB6/T0CCP0
  GPIO_PORTB_DEN_R |= 0x40;        // enable digital I/O on PB6
                                   // configure PB6 as T0CCP0
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x07000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;     // disable analog functionality on PB6
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = TIMER_CFG_16_BIT; // configure for 16-bit timer mode
                                   // configure for 24-bit capture mode
  TIMER0_TAMR_R = (TIMER_TAMR_TACMR|TIMER_TAMR_TAMR_CAP);
                                   // configure for rising edge event
  TIMER0_CTL_R &= ~(TIMER_CTL_TAEVENT_POS|0x0C);
  TIMER0_TAILR_R = TIMER_TAILR_TAILRL_M;// start value
  TIMER0_TAPR_R = 0xFF;            // activate prescale, creating 24-bit
  TIMER0_IMR_R |= TIMER_IMR_CAEIM; // enable capture match interrupt
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// clear timer0A capture match flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 16-b, +edge timing, interrupts
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = NVIC_EN0_INT19;     // enable interrupt 19 in NVIC
}

int record[1000];
int idx = 0;
void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_CAECINT;// acknowledge timer0A capture match
	uint32_t temp = TIMER0_TAR_R &0x00FFFFFF;
  Period = First - temp; // 24 bits, 12.5ns resolution
	First = TIMER0_TAR_R;            // setup for next

	uint32_t tempSpeed = 200000000/Period;
	if ((tempSpeed >= Speed + 100 || tempSpeed <= Speed - 100) && Speed != 0) return;
	Speed = 200000000/Period;
	E = Target_Speed - Speed;   // 0.1 rps
	U += (STABILITY_K * E) / 64;  // discrete integral
	if(U < 15000) U = 15000;        // Constrain output
	if(U > 39900) U = 39900;      // 20000 to 39900
	Motor_SetDuty(U);            // output
	if (idx < 1000) record[idx++] = E;
}
