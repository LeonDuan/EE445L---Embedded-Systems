#include "Game.h"

#define MAIN_SHIP_WIDTH		20
#define MAIN_SHIP_HEIGHT	17
#define ENEMY_SHIP_WIDTH	15
#define ENEMY_SHIP_HEIGHT	10
#define BOSS_WIDTH				50
#define BOSS_HEIGHT				54



// refer to the following for coordinates (instead of the actual coordinates on the LCD)
//(128, 0)                        (160,128)
//		|------------------------------|
//		|															 |
//		|															 |
//		|															 |
//		|															 |
//		|					         						 |
//		|															 |
//		|															 |
//		|															 |
//		|------------------------------|
//(0, 0)                           (160,0)


int Is_Hit(Ship ship, Bullet bullet) {
	return (ship.x <= bullet.x && ship.x + ship.width >= bullet.x && ship.y >= bullet.y && ship.y - ship.height <= bullet.y);
}


void Update_Enemies(void){
	// TODO
}

void Update_Boss(void){
	// TODO
}

void Update_Enemy_Bullets(void){
	// TODO
}

void Update_My_Ship(void){
	// TODO
}

void Update_My_Bullets(void){
	// TODO
}

void Init_Level(int stage){
	// TODO
}

void Update_Every_Object(void){
	// TODO
}