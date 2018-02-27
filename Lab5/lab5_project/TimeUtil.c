#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "DAC.h"
#include "Music.h"

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count flag
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt enable
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Counter mode
#define NVIC_ST_RELOAD_M        0x00FFFFFF  // Counter load value

#define PF2             (*((volatile uint32_t *)0x40025010))
#define PF1             (*((volatile uint32_t *)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value

uint16_t sound_flute[32] = {1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,	
  1219,1092,1007,913,890,833,847,810,777,744,674,	
  598,551,509,476,495,533,589,659,758,876};

uint16_t sound_horn[32] = {1063,1082,1119,1275,1678,1748,1275,755,661,661,703,	
  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,	
  1370,1086,708,519,448,490,566,684,802,992};

uint16_t sound_trumpet[32] = {1007,1088,1156,1194,1067,789,303,99,789,1510,1476,	
  1173,1067,1037,1084,1062,1011,1015,1045,1062,1011,1011,	
  1058,1113,1084,1075,1079,1105,1088,1049,1015,1045};


extern uint16_t resetFlag;
extern uint16_t pauseFlag;
extern Note *music;
extern uint16_t musicLength;
extern uint16_t instrumentFlag;
	
// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1A_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = 200000;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00002000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
	PF2 ^= 0x04;
	volatile int t1_reload = TIMER1_TAILR_R;
	volatile int t0_reload = TIMER0_TAILR_R;
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	
	static uint16_t DAC_index = 0;
	if (pauseFlag==0) {
		if (instrumentFlag == flute)DAC_Out(sound_flute[DAC_index]);
		else if (instrumentFlag == horn) DAC_Out(sound_horn[DAC_index]);
		else if (instrumentFlag == trumpet) DAC_Out(sound_trumpet[DAC_index]);
		DAC_index = (DAC_index+1)%32;
	}
}

void Timer0A_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = 4000000;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}

void Timer0A_Handler(void){
	PF1 ^= 0x02;
	volatile int t1_reload = TIMER1_TAILR_R;
	volatile int t0_reload = TIMER0_TAILR_R;
	
	static uint16_t note_counter = 0;
	static uint16_t song_counter = 0;
	TIMER0_ICR_R = 0x00000001;
	
	if (resetFlag != 0) {
		resetFlag = 0;
		note_counter = 0;
		song_counter = 0;
	}
	
	if (pauseFlag!=0){
		return;
	}
	
	if (note_counter == 0) {
		if (song_counter == musicLength-1) {
			song_counter = 0;
		}
		
		note_counter = music[song_counter].period;
		TIMER1_TAILR_R = (music+song_counter)->freq;
		song_counter ++;
	}
	
	else {
		note_counter --;
	}
}
