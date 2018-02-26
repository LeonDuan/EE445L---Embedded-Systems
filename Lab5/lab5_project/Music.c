#include <stdint.h>
#include "DAC.h"
#include "TimeUtil.h"
#include "Music.h"
#include "..//inc//tm4c123gh6pm.h"

Music song1;
Music song2;
Music song3;

uint16_t musicLength;
Note *music;
uint16_t resetFlag = 0;
uint16_t soundFlag = 0;
uint16_t pauseFlag = 0;

void playMusic(uint16_t song){
	switch(song) {
		case 1:
			music = song1.song;
			musicLength = song1.song_length;
			TIMER0_TAILR_R = 8000000*60/song1.tempo/4;
			resetFlag = 1;
			break;
	}
}
