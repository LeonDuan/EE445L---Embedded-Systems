#include "./speaker.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void enableSpeaker(int enable) {
	if(enable) PWM0_ENABLE_R |= 0x01;
	else PWM0_ENABLE_R &= ~0x01;
}
