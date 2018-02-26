#include <stdint.h>
#include "DAC.h"
#include "TimeUtil.h"
#include "Music.h"
#include "..//inc//tm4c123gh6pm.h"

Note Bach[336] = {
	{BF4,s},{EF4,s},{D4,s},{C4,s},{D4,s},{F4,s},{D4,s},{F4,s},{BF4,s},{F4,s},{D4,s},{C4,s},{D4,s},{F4,s},{D4,s},{F4,s},
	{BF4,s},{G4,s},{EF4,s},{D4,s},{EF4,s},{G4,s},{EF4,s},{G4,s},{BF4,s},{G4,s},{EF4,s},{D4,s},{EF4,s},{G4,s},{EF4,s},{GF4,s},
	{BF4,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{C4,s},{EF4,s},{C4,s},{BF4,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{C4,s},{EF4,s},{C4,s},
	{BF4,s},{BF4,s},{D4,s},{C4,s},{D4,s},{BF4,s},{D4,s},{BF4,s},{BF4,s},{BF4,s},{D4,s},{C4,s},{D4,s},{BF4,s},{D4,s},{A4,s},
	{BF4,s},{G4,s},{D4,s},{C4,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{F4,s},{E4,s},{D4,s},
	{E4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{E4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},
	{A4,s},{C4,s},{F4,s},{EF4,s},{F4,s},{C4,s},{BF4,s},{C4,s},{A4,s},{C4,s},{BF4,s},{C4,s},{F4,s},{A4,s},{G4,s},{F4,s},
	{G4,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},{G4,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},
	{G4,s},{E4,s},{F4,s},{G4,s},{F4,s},{E4,s},{D4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},{E4,s},{D4,s},{C4,s},{BF4,s},
	{A4,s},{G4,s},{F4,s},{F4,s},{C4,s},{F4,s},{A4,s},{C4,s},{F4,s},{G4,s},{A4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},
	{B4,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},{D4,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},
	{EF4,s},{G4,s},{C4,s},{D4,s},{EF4,s},{C4,s},{G4,s},{F4,s},{F4,s},{G4,s},{A4,s},{C4,s},{EF4,s},{C4,s},{BF4,s},{G4,s},
	{GF4,s},{A4,s},{GF4,s},{A4,s},{C4,s},{A4,s},{C4,s},{A4,s},{GF4,s},{A4,s},{GF4,s},{A4,s},{C4,s},{A4,s},{C4,s},{A4,s},
	{BF4,s},{A4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{C4,s},{A4,s},{BF4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF4,s},
	{G4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{G4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},
	{BF4,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},{BF4,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},
	{BF4,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},{BF4,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},
	{BF4,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{A4,s},{EF4,s},{A4,s},{BF4,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{A4,s},{EF4,s},{A4,s},
	{A4,s},{F4,s},{D4,s},{C4,s},{D4,s},{B4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},
	{E4,s},{C4,s},{G4,s},{A4,s},{BF4,s},{G4,s},{A4,s},{BF4,s},{E4,s},{C4,s},{G4,s},{A4,s},{BF4,s},{G4,s},{A4,s},{BF4,s},
	{EF4,s},{C4,s},{F4,s},{G4,s},{A4,s},{F4,s},{G4,s},{A4,s},{EF4,s},{C4,s},{F4,s},{G4,s},{A4,s},{F4,s},{G4,s},{A4,s}
};

Note Bethoveen[66] = {
	{B4,q},{B4,q},{C4,q},{D4,q},
	{D4,q},{C4,q},{B4,q},{A4,q},
	{G4,q},{G4,q},{A4,q},{B4,q},
	{B4,q},{B4,e},{A4,e},{A4,h},
	{B4,q},{B4,q},{C4,q},{D4,q},
	{D4,q},{C4,q},{C4,q},{A4,q},
	{G4,q},{G4,q},{A4,q},{B4,q},
	{A4,q},{A4,e},{G4,e},{G4,h},
	{A4,q},{A4,q},{B4,q},{G4,q},
	{A4,q},{B4,e},{C4,e},{B4,q},{G4,q},
	{A4,q},{B4,e},{C4,e},{B4,q},{A4,q},
	{G4,q},{A4,q},{D4,h},
	{B4,q},{B4,q},{C4,q},{D4,q},
	{D4,q},{C4,q},{B4,q},{A4,q},
	{G4,q},{G4,q},{A4,q},{B4,q},
	{A4,q},{A4,e},{G4,e},{G4,h}
};

Music song1 = {Bach,336,70};
Music song2 = {Bethoveen,66,70};
Music song3;

uint16_t musicLength;
Note *music;
uint16_t resetFlag = 0;
uint16_t pauseFlag = 0;
Instruments instrumentFlag = flute;

void playMusic(uint16_t song){
	switch(song) {
		case 1:
			music = song1.song;
			musicLength = song1.song_length;
			TIMER0_TAILR_R = 8000000*60/song1.tempo/4;
			resetFlag = 1;
			break;
		case 2:
			music = song2.song;
			musicLength = song2.song_length;
			TIMER0_TAILR_R = 8000000*60/song2.tempo/4;
			resetFlag = 1;
			break;
	}
}
