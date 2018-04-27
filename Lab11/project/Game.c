// refer to the following for coordinates (instead of the actual coordinates on the LCD)
//(0,128)                        (160,128)
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
//(0,0)                           (160,0)

#include "Game.h"

#define MAIN_SHIP_WIDTH		17
#define MAIN_SHIP_HEIGHT	20
#define ENEMY_SHIP_WIDTH	10
#define ENEMY_SHIP_HEIGHT	15
#define BOSS_WIDTH				54
#define BOSS_HEIGHT				50
#define UP								1
#define	LEFT							2
#define DOWN							3
#define RIGHT							4

static int current_stage;
static int enemy_direction = RIGHT;
static int flag_GameOver = 0;
static int remaining_Enemies;

Ship * enemies[NUM_MAX_ENEMIES_COLUMN][NUM_MAX_ENEMIES_ROW];
Ship * my_ship;
Ship * boss;

Bullet * enemy_bullets[NUM_ENEMY_BULLETS];
Bullet * my_bullets[NUM_MY_BULLETS];

Explosion * explosions[NUM_MAX_EXPLOSIONS];

void Add_Explosion(int x, int y) {
	// TODO
}

int Is_Hit(Ship * ship, Bullet * bullet) {
	if (ship->x >= (bullet->x-bullet->width) &&
		ship->y <= (bullet->y-bullet->height) && (ship->y-ship->height) <= bullet->y) return 1;
	return 0;
}

void Update_Enemies(void){
	switch (enemy_direction) {
		case (UP):
			for (int i = 0; i < NUM_MAX_ENEMIES_COLUMN; i++) {
				for (int j = 0; i < NUM_MAX_ENEMIES_ROW; j++) {
					if (enemies[i][j]->hp != 0) {
						if (enemies[i][j]->y >= 128) {
							enemy_direction = LEFT;
							Update_Enemies();
						}
						enemies[i][j]->y += 1;
					}
				}
			}
			break;
			
		case (LEFT):
			for (int i = NUM_MAX_ENEMIES_ROW; i >= 0; i--) {
				for (int j = 0; i < NUM_MAX_ENEMIES_COLUMN; j++) {
					if (enemies[j][i]->hp != 0) {
						if (enemies[j][i]->x <= 30) {
							enemy_direction = DOWN;
							Update_Enemies();
						}
						enemies[j][i]->x -= 1;
					}
				}
			}
			break;
			
		case (DOWN):
			for (int i = NUM_MAX_ENEMIES_COLUMN; i >= 0; i--) {
				for (int j = 0; i < NUM_MAX_ENEMIES_ROW; j++) {
					if (enemies[i][j]->hp != 0) {
						if (enemies[i][j]->y <= 0) {
							enemy_direction = RIGHT;
							Update_Enemies();
						}
						enemies[i][j]->y -= 1;
					}
				}
			}
			break;
			
		case (RIGHT):
			for (int i = 0; i < NUM_MAX_ENEMIES_ROW; i++) {
				for (int j = 0; i < NUM_MAX_ENEMIES_COLUMN; j++) {
					if (enemies[j][i]->hp != 0) {
						if (enemies[j][i]->y >= 128) {
							enemy_direction = DOWN;
							Update_Enemies();
						}
						enemies[j][i]->y += 1;
					}
				}
			}
	}
}

void Update_Boss(void){
	// TODO
}

void Update_Enemy_Bullets(void){
	for (int i = 0; i < NUM_ENEMY_BULLETS; i++) {
		if (enemy_bullets[i]->valid) {
			if ((enemy_bullets[i]->x-enemy_bullets[i]->width) > 0) {
				if (Is_Hit(my_ship,enemy_bullets[i]))	{
					my_ship->hp -= 1;
					if (my_ship->hp == 0) {
						flag_GameOver = 1;
					}
				}
				else enemy_bullets[i]->x -= 1;
			}
		}
	}	
}

void Update_My_Ship(void){
	// TODO
}

void Update_My_Bullets(void){
	for (int i = 0; i < NUM_MY_BULLETS; i++) {
		if (my_bullets[i]->valid) {
			for (int j = 0; i < NUM_MAX_ENEMIES_COLUMN; j++) {
				for (int k = 0; i < NUM_MAX_ENEMIES_ROW; k++) {
					if (Is_Hit(enemies[j][k],my_bullets[i])) {
						enemies[j][k]->hp -= 1;
						if (!enemies[j][k]->hp) {
							remaining_Enemies -= 1;
							Add_Explosion(enemies[j][k]->x, enemies[j][k]->y);
						}
					}
					else my_bullets[i]->x += 1;
				}
			}
		}
	}
}

void Init_Level(int stage){
	// TODO
}

void Add_Bullets(int enemy){
	// TODO
}

// ------------------------ Get Functions ------------------------
Ship ** Get_Enemies(void){
	return *enemies;
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

int Is_GameOver(void) {
	return flag_GameOver;
}

int Is_LevelWon(void) {
	return (!remaining_Enemies);
}