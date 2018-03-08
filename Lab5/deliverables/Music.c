#include <stdint.h>
#include "DAC.h"
#include "TimeUtil.h"
#include "Music.h"
#include "..//inc//tm4c123gh6pm.h"

Note Bach[336] = {
	{BF3,s},{EF4,s},{D5,s},{C5,s},{D5,s},{F4,s},{D5,s},{F4,s},{BF3,s},{F4,s},{D5,s},{C5,s},{D5,s},{F4,s},{D5,s},{F4,s},
	{BF3,s},{G4,s},{EF5,s},{D5,s},{EF5,s},{G4,s},{EF5,s},{G4,s},{BF3,s},{G4,s},{EF5,s},{D5,s},{EF5,s},{G4,s},{EF5,s},{GF4,s},
	{BF3,s},{A4,s},{EF5,s},{D5,s},{EF5,s},{C5,s},{EF5,s},{C5,s},{BF3,s},{A4,s},{EF5,s},{D5,s},{EF5,s},{C5,s},{EF5,s},{C5,s},
	{BF3,s},{BF4,s},{D5,s},{C5,s},{D5,s},{BF4,s},{D5,s},{BF4,s},{BF3,s},{BF4,s},{D5,s},{C5,s},{D5,s},{BF4,s},{D5,s},{A4,s},
	{BF3,s},{G4,s},{D5,s},{C5,s},{D5,s},{BF4,s},{A4,s},{BF4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{F4,s},{E4,s},{D4,s},
	{E4,s},{BF4,s},{C5,s},{BF4,s},{C5,s},{BF4,s},{C5,s},{BF4,s},{E4,s},{BF4,s},{C5,s},{BF4,s},{C5,s},{BF4,s},{C5,s},{BF4,s},
	{A4,s},{C5,s},{F5,s},{EF5,s},{F5,s},{C5,s},{BF4,s},{C5,s},{A4,s},{C5,s},{BF4,s},{C5,s},{F4,s},{A4,s},{G4,s},{F4,s},
	{G3,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},{G3,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},
	{G3,s},{E4,s},{F4,s},{G4,s},{F4,s},{E4,s},{D4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F5,s},{E5,s},{D5,s},{C5,s},{BF4,s},
	{A4,s},{G4,s},{F4,s},{F5,s},{C5,s},{F5,s},{A4,s},{C5,s},{F4,s},{G4,s},{A4,s},{C5,s},{BF4,s},{A4,s},{G4,s},{F4,s},
	{B4,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},{D5,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},
	{EF4,s},{G4,s},{C5,s},{D5,s},{EF5,s},{C5,s},{G4,s},{F4,s},{E4,s},{G4,s},{A4,s},{C5,s},{EF5,s},{C5,s},{BF4,s},{G4,s},
	{GF4,s},{A4,s},{GF4,s},{A4,s},{C5,s},{A4,s},{C5,s},{A4,s},{GF4,s},{A4,s},{GF4,s},{A4,s},{C5,s},{A4,s},{C5,s},{A4,s},
	{BF4,s},{A4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{C5,s},{A4,s},{BF4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF3,s},
	{G3,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{G3,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},
	{BF3,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},{BF3,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},
	{BF3,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},{BF3,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},
	{BF3,s},{A4,s},{EF5,s},{D5,s},{EF5,s},{A4,s},{EF5,s},{A4,s},{BF3,s},{A4,s},{EF5,s},{D5,s},{EF5,s},{A4,s},{EF5,s},{A4,s},
	{A3,s},{F4,s},{D5,s},{C5,s},{D5,s},{BF4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF3,s},{A3,s},{G3,s},{F3,s},
	{E3,s},{C4,s},{G4,s},{A4,s},{BF4,s},{G4,s},{A4,s},{BF4,s},{E3,s},{C4,s},{G4,s},{A4,s},{BF4,s},{G4,s},{A4,s},{BF4,s},
	{EF3,s},{C4,s},{F4,s},{G4,s},{A4,s},{F4,s},{G4,s},{A4,s},{EF3,s},{C4,s},{F4,s},{G4,s},{A4,s},{F4,s},{G4,s},{A4,s}
};

Note Bethoveen[66] = {
	{E4,q},{E4,q},{F4,q},{G4,q},
	{G4,q},{F4,q},{E4,q},{D4,q},
	{C4,q},{C4,q},{D4,q},{E4,q},
	{E4,q+e},{D4,e},{D4,h},
	{E4,q},{E4,q},{F4,q},{G4,q},
	{G4,q},{F4,q},{E4,q},{D4,q},
	{C4,q},{C4,q},{D4,q},{E4,q},
	{D4,q+e},{C4,e},{C4,h},
	{D4,q},{D4,q},{E4,q},{C4,q},
	{D4,q},{E4,e},{F4,e},{E4,q},{C4,q},
	{D4,q},{E4,e},{F4,e},{E4,q},{D4,q},
	{C4,q},{D4,q},{G3,h},
	{E4,q},{E4,q},{F4,q},{G4,q},
	{G4,q},{F4,q},{E4,q},{D4,q},
	{C4,q},{C4,q},{D4,q},{E4,q},
	{D4,q},{D4,e},{C4,e},{C4,h}
};

Note Sine_Wave[1] = {
{E4,w}
};

Music song1 = {Bach,336,70};
Music song2 = {Bethoveen,66,70};
Music song3 = {Sine_Wave,1,70};

uint16_t musicLength;
Note *music;
uint16_t resetFlag = 0;
uint16_t pauseFlag = 0;
uint16_t currentSong = 1;
Instruments instrumentFlag = trumpet;
uint16_t tempo_mult = 1;

void playMusic(uint16_t song){
	switch(song) {
		case 1:
			music = song1.song;
			musicLength = song1.song_length;
			TIMER0_TAILR_R = (80000000/song1.tempo)*60/4;
			resetFlag = 1;
			break;
		case 2:
			music = song2.song;
			musicLength = song2.song_length;
			TIMER0_TAILR_R = (80000000/song2.tempo)*60/16;
			resetFlag = 1;
			break;
		case 3:
			music = song3.song;
			musicLength = song3.song_length;
			TIMER0_TAILR_R = (80000000/song3.tempo)*60/4;
			resetFlag = 1;
			break;
	}
}
