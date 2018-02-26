//Music.h
//Music module for Lab5
//Tahir Haideri and Tianyun Duan

#ifndef Music_H
#define Music_H

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
