#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "DAC.h"
#include "Sound.h"
#include "Timer.h"

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

uint16_t sound_flute[32] = {1007,1252,1374,1548,1698,1797,1825,1797,1675,1562,1383,	
  1219,1092,1007,913,890,833,847,810,777,744,674,	
  598,551,509,476,495,533,589,659,758,876};

uint16_t sound_horn[32] = {1063,1082,1119,1275,1678,1748,1275,755,661,661,703,	
  731,769,845,1039,1134,1209,1332,1465,1545,1427,1588,	
  1370,1086,708,519,448,490,566,684,802,992};

uint16_t sound_trumpet[32] = {1007,1088,1156,1194,1067,789,303,99,789,1510,1476,	
  1173,1067,1037,1084,1062,1011,1015,1045,1062,1011,1011,	
  1058,1113,1084,1075,1079,1105,1088,1049,1015,1045};

uint16_t musicLength;
Note *music;
uint16_t resetFlag = 0;
uint16_t pauseFlag = 0;
uint16_t currentSong = 1;
Instruments instrumentFlag = trumpet;
uint16_t tempo_mult = 1;

Music bgm = {Bethoveen,66,70};

void play_Music(void){
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
		TIMER1_TAILR_R = music[song_counter].freq;
		song_counter ++;
	}
	
	else {
		note_counter --;
	}
}

void out_Sound(){
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

void Sound_Init(){
	Timer0A_Init(&play_Music,200000);
	Timer1A_Init(&out_Sound,4000000);
	DAC_Init();
}

void Sound_Play(uint16_t song){
	switch(song) {
		case 1:
			music = bgm.song;
			musicLength = bgm.song_length;
			TIMER0_TAILR_R = (80000000/bgm.tempo)*60/16;
			resetFlag = 1;
			break;
	}
}
