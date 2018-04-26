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

#include "Game.h"

#define MAIN_SHIP_WIDTH		17
#define MAIN_SHIP_HEIGHT	20
#define ENEMY_SHIP_WIDTH	10
#define ENEMY_SHIP_HEIGHT	15
#define BOSS_WIDTH				54
#define BOSS_HEIGHT				50


int current_stage;

Ship * enemies[NUM_MAX_ENEMIES];
Ship * my_ship;
Ship * boss;

Bullet * enemy_bullets[NUM_ENEMY_BULLETS];
Bullet * my_bullets[NUM_MY_BULLETS];

Explosion * explosions[NUM_MAX_EXPLOSIONS];

int Is_Hit(Ship * ship, Bullet * bullet) {
	// TODO
	return 0;
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

void Add_Bullets(int enemy){
	// TODO
}

// ------------------------ Get Functions ------------------------
Ship ** Get_Enemies(void){
	return enemies;
}

Ship * Get_Boss(void){
	return boss;
}

Ship * Get_My_Ship(void){
	return my_ship;
}

Bullet ** Get_Enemy_Bullets(void){
	return enemy_bullets;
}

Bullet ** Get_My_Bullets(void){
	return my_bullets;
}

Explosion ** Get_Explosions(void){
	return explosions;
}
