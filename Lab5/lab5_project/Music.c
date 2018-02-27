#include <stdint.h>
#include "DAC.h"
#include "TimeUtil.h"
#include "Music.h"
#include "..//inc//tm4c123gh6pm.h"

Note Bach[336] = {
	{BF3,s},{EF4,s},{D4,s},{C5,s},{D5,s},{F4,s},{D5,s},{F4,s},{BF3,s},{F4,s},{D5,s},{C5,s},{D5,s},{F4,s},{D5,s},{F4,s},
	{BF3,s},{G4,s},{EF4,s},{D4,s},{EF4,s},{G4,s},{EF4,s},{G4,s},{BF3,s},{G4,s},{EF4,s},{D4,s},{EF4,s},{G4,s},{EF4,s},{GF4,s},
	{BF3,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{C4,s},{EF4,s},{C4,s},{BF3,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{C4,s},{EF4,s},{C4,s},
	{BF3,s},{BF3,s},{D4,s},{C4,s},{D4,s},{BF4,s},{D4,s},{BF4,s},{BF3,s},{BF4,s},{D4,s},{C4,s},{D4,s},{BF4,s},{D4,s},{A4,s},
	{BF3,s},{G4,s},{D4,s},{C4,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{F4,s},{E4,s},{D4,s},
	{E4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{E4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},{C4,s},{BF4,s},
	{A4,s},{C4,s},{F4,s},{EF4,s},{F4,s},{C4,s},{BF4,s},{C4,s},{A4,s},{C4,s},{BF4,s},{C4,s},{F4,s},{A4,s},{G4,s},{F4,s},
	{G3,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},{G3,s},{D4,s},{BF4,s},{A4,s},{BF4,s},{D4,s},{BF4,s},{D4,s},
	{G3,s},{E4,s},{F4,s},{G4,s},{F4,s},{E4,s},{D4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},{E4,s},{D4,s},{C4,s},{BF4,s},
	{A4,s},{G4,s},{F4,s},{F4,s},{C4,s},{F4,s},{A4,s},{C4,s},{F4,s},{G4,s},{A4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},
	{B4,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},{D4,s},{F4,s},{AF4,s},{G4,s},{AF4,s},{F4,s},{B4,s},{F4,s},
	{EF4,s},{G4,s},{C4,s},{D4,s},{EF4,s},{C4,s},{G4,s},{F4,s},{F4,s},{G4,s},{A4,s},{C4,s},{EF4,s},{C4,s},{BF4,s},{G4,s},
	{GF4,s},{A4,s},{GF4,s},{A4,s},{C4,s},{A4,s},{C4,s},{A4,s},{GF4,s},{A4,s},{GF4,s},{A4,s},{C4,s},{A4,s},{C4,s},{A4,s},
	{BF4,s},{A4,s},{G4,s},{BF4,s},{A4,s},{BF4,s},{C4,s},{A4,s},{BF4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF3,s},
	{G3,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{G3,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},{F4,s},{EF4,s},
	{BF3,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},{BF3,s},{D4,s},{AF4,s},{G4,s},{AF4,s},{D4,s},{AF4,s},{D4,s},
	{BF3,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},{BF4,s},{EF4,s},{G4,s},{F4,s},{G4,s},{EF4,s},{G4,s},{EF4,s},
	{BF3,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{A4,s},{EF4,s},{A4,s},{BF4,s},{A4,s},{EF4,s},{D4,s},{EF4,s},{A4,s},{EF4,s},{A4,s},
	{A3,s},{F4,s},{D4,s},{C4,s},{D4,s},{B4,s},{A4,s},{G4,s},{F4,s},{EF4,s},{D4,s},{C4,s},{BF4,s},{A4,s},{G4,s},{F4,s},
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

Music song1 = {Bach,336,70};
Music song2 = {Bethoveen,66,70};
Music song3;

uint16_t musicLength;
Note *music;
uint16_t resetFlag = 0;
uint16_t pauseFlag = 0;
Instruments instrumentFlag = trumpet;

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
	}
}
