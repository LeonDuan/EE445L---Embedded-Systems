//Sound.h
//Sound module
//Tahir Haideri and Tianyun Duan

#ifndef Sound_H
#define Sound_H

#define B5    	2531   // 987.8 Hz
#define BF5   	2681   // 932.3 Hz
#define A5    	2841   // 880 Hz
#define AF5   	3010   // 830.6 Hz
#define G5    	3189   // 784 Hz
#define GF5  	3378   // 740 Hz
#define F5   	3579   // 698.5 Hz
#define E5   	3792   // 659.3 Hz
#define EF5  	4018   // 622.3 Hz
#define D5   	4257   // 587.3 Hz
#define DF5  	4510   // 554.4 Hz
#define C5  	4778   // 523.3 Hz
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
#define B3  	10124   // 246.9 Hz
#define BF3 	10726   // 233.1 Hz
#define A3  	11364   // 220 Hz
#define AF3 	12039   // 207.7 Hz
#define G3  	12755   // 196 Hz
#define GF3 	13514   // 185 Hz
#define F3  	14317   // 174.6 Hz
#define E3  	15169   // 164.8 Hz
#define EF3 	16071   // 155.6 Hz
#define D3  	17026   // 146.8 Hz
#define DF3 	18039   // 138.6 Hz
#define C3  	19111   // 130.8 Hz

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

void Sound_Init(void);
void Sound_Play(uint16_t song);

#endif
