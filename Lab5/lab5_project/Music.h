//Music.h
//Music module for Lab5
//Tahir Haideri and Tianyun Duan

#ifndef Music_H
#define Music_H

#define B4  	5062   // 493.9 Hz
#define BF4 	5363   // 466.2 Hz
#define A4  	5682   // 440 Hz
#define AF4 	6020   // 415.3 Hz
#define G4  	6378   // 392 Hz
#define GF4 	6757   // 370 Hz
#define F4  	7159   // 349.2 Hz
#define E4  	7584   // 329.6 Hz
#define EF4 	8035   // 311.1 Hz
#define D4  	8513   // 293.7 Hz
#define DF4 	9019   // 277.2 Hz
#define C4		9556	 // 261.6 Hz

#define s       1
#define e  			4
#define q				8
#define h				16
#define w				32

typedef enum {
	flute,
	horn,
	trumpet,
	bassoon,
	oboe,
	guitar
} Instruments;

typedef struct {
	uint32_t freq;
	uint16_t period; 
} Note;

typedef struct {
	Note* song; //pointer to array of notes in the song 
	uint16_t song_length;
	uint16_t tempo;
} Music;

void playMusic(uint16_t song);
#endif
