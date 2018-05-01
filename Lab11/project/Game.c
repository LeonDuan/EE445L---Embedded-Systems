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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "Game.h"
#include "Proximity.h"


#define MAIN_SHIP_WIDTH		17
#define MAIN_SHIP_HEIGHT	20
#define ENEMY_SHIP_WIDTH	10
#define ENEMY_SHIP_HEIGHT	15
#define BOSS_WIDTH				54
#define BOSS_HEIGHT				50
#define BULLET_WIDTH			5
#define BULLET_HEIGHT			2
#define UP								1
#define	LEFT							2
#define DOWN							3
#define RIGHT							4

const Coordinate enemyGrid[20] = {
	{140,90},{125,90},{110,90},{95,90},{80,90},
	{140,70},{125,70},{110,70},{95,70},{80,70},
	{140,50},{125,50},{110,50},{95,50},{80,50},
	{140,30},{125,30},{110,30},{95,30},{80,30},
};

const Ship Enemy[3] = {
	{NULL,NULL,-100,-100,ENEMY_SHIP_WIDTH,ENEMY_SHIP_HEIGHT,2,ENEMY1,1},
	{NULL,NULL,-100,-100,ENEMY_SHIP_WIDTH,ENEMY_SHIP_HEIGHT,4,ENEMY2,1},
	{NULL,NULL,-100,-100,ENEMY_SHIP_WIDTH,ENEMY_SHIP_HEIGHT,6,ENEMY3,1}
};

const Ship Boss[2] = {
	{140,90,-100,-100,BOSS_WIDTH,BOSS_HEIGHT,20,BOSS1,1},
	{140,90,-100,-100,BOSS_WIDTH,BOSS_HEIGHT,20,BOSS2,1}
};

const Ship Player = {10,70,-100,-100,MAIN_SHIP_WIDTH,MAIN_SHIP_HEIGHT,10,MY_SHIP,1};

extern int enemyMultiplier;
static int current_stage;
static int enemy_direction = RIGHT;
static int boss_direction = RIGHT;
static int flag_GameOver = 0;
static int flag_NextStage = 0;
static int flag_GameWin = 0;
static int remaining_Enemies;

Ship * enemies[NUM_MAX_ENEMIES_COLUMN][NUM_MAX_ENEMIES_ROW];
Ship * my_ship;
Ship * boss;

Bullet * enemy_bullets[NUM_ENEMY_BULLETS];
Bullet * my_bullets[NUM_MY_BULLETS];

Explosion * explosions[NUM_MAX_EXPLOSIONS];

void Add_Explosion(int x, int y) {
	for (int i = 0; i < NUM_MAX_EXPLOSIONS; i++) {
		if (explosions[i]->stage > 3 || explosions[i]->stage < 1){
			explosions[i]->x = x;
			explosions[i]->y = y;
			explosions[i]->stage = 1;
		}
	}
}

int Is_Hit(Ship * ship, Bullet * bullet) {
	if (ship->x >= (bullet->x-bullet->width) &&
		ship->y <= (bullet->y-bullet->height) && (ship->y-ship->height) <= bullet->y) return 1;
	return 0;
}

Ship get_RndShip(void) {
	int col = rand()%3;
	while(1) {
		for (int i = 4; i >= 0; i--) {
			if (enemies[col][i]->hp > 0) return *enemies[col][i];
		}
		col = (col + 1)%3;
	}
}

void Check_Hit() {
	switch (current_stage) {
		case 1:
			for (int i = 0; i < NUM_MY_BULLETS; i++) {
				for (int j = 0; j < NUM_MAX_ENEMIES_COLUMN; j++) {
					for (int k = 0; k < NUM_MAX_ENEMIES_ROW; k++) {
						if (enemies[j][k]->valid > 0) {
							if(Is_Hit(enemies[j][k],my_bullets[i])) {
								enemies[j][k]->hp -= my_bullets[i]->dmg;
								if (enemies[j][k]->hp <= 0) {
									Add_Explosion(enemies[j][k]->x,enemies[j][k]->y);
									enemies[j][k]->valid = 0;
									remaining_Enemies -= 1;
									if (remaining_Enemies < 10) enemyMultiplier = 2;
									if (remaining_Enemies < 5)	enemyMultiplier = 3;
									
								}
							}
						}
					}
				}
			}
			break;
		
		case 2:
			for (int i = 0; i < NUM_MY_BULLETS; i++) {
				if (boss->valid > 0) {
					if (Is_Hit(boss,my_bullets[i])) {
						boss->hp -= my_bullets[i]->dmg;
						if (boss->hp <= 0) {
							boss->valid = 0;
							flag_GameWin = 1;
						}
						else if (boss->hp < 10) enemyMultiplier = 2;
						else if (boss->hp < 5) enemyMultiplier = 3;
					}
				}
			}
	}
}

void Update_Enemies(void){
	switch (enemy_direction) {
		case (UP):
			for (int i = 0; i < NUM_MAX_ENEMIES_COLUMN; i++) {
				for (int j = 0; i < NUM_MAX_ENEMIES_ROW; j++) {
					if (enemies[i][j]->valid > 0) {
						if (enemies[i][j]->y >= 128) {
							enemy_direction = LEFT;
							Update_Enemies();
							return;
						}
						enemies[i][j]->xold = enemies[i][j]->x;
						enemies[i][j]->yold = enemies[i][j]->y;
						enemies[i][j]->y += 1;
					}
				}
			}
			break;
			
		case (LEFT):
			for (int i = NUM_MAX_ENEMIES_ROW; i >= 0; i--) {
				for (int j = 0; i < NUM_MAX_ENEMIES_COLUMN; j++) {
					if (enemies[j][i]->valid > 0) {
						if ((enemies[j][i]->x - ENEMY_SHIP_WIDTH) <= 30) {
							enemy_direction = DOWN;
							Update_Enemies();
							return;
						}
						enemies[j][i]->yold = enemies[j][i]->y;
						enemies[j][i]->xold = enemies[j][i]->x;
						enemies[j][i]->x -= 1;
					}
				}
			}
			break;
			
		case (DOWN):
			for (int i = NUM_MAX_ENEMIES_COLUMN; i >= 0; i--) {
				for (int j = 0; i < NUM_MAX_ENEMIES_ROW; j++) {
					if (enemies[i][j]->valid > 0) {
						if ((enemies[i][j]->y - ENEMY_SHIP_HEIGHT) <= 0) {
							enemy_direction = RIGHT;
							Update_Enemies();
							return;
						}
						enemies[i][j]->xold = enemies[i][j]->x;
						enemies[i][j]->yold = enemies[i][j]->y;
						enemies[i][j]->y -= 1;
					}
				}
			}
			break;
			
		case (RIGHT):
			for (int i = 0; i < NUM_MAX_ENEMIES_ROW; i++) {
				for (int j = 0; i < NUM_MAX_ENEMIES_COLUMN; j++) {
					if (enemies[j][i]->valid > 0) {
						if (enemies[j][i]->y >= 160) {
							enemy_direction = UP;
							Update_Enemies();
							return;
						}
						enemies[j][i]->yold = enemies[j][i]->y;
						enemies[j][i]->xold = enemies[j][i]->x;
						enemies[j][i]->y += 1;
					}
				}
			}
	}
}

void Update_Boss(void){
	switch (boss_direction) {
		case UP:
			if (boss->valid > 0) {
				if (boss->y >= 128) {
					boss_direction = LEFT;
					Update_Boss();
					return;
				}
				boss->xold = boss->x;
				boss->yold = boss->y;
				boss->y += 1;
			}
			break;
			
		case LEFT:
			if (boss->valid > 0) {
				if ((boss->x - BOSS_WIDTH) <= 30) {
					boss_direction = DOWN;
					Update_Boss();
					return;
				}
				boss->xold = boss->x;
				boss->yold = boss->y;
				boss->x -= 1;
			}
			break;
			
		case DOWN:
			if (boss->valid > 0) {
				if ((boss->y - BOSS_HEIGHT) <= 0) {
					boss_direction = RIGHT;
					Update_Boss();
					return;
				}
				boss->xold = boss->x;
				boss->yold = boss->y;
				boss->y -= 1;
			}
			break;
			
		case RIGHT:
			if (boss->valid > 0) {
				if (boss->x >= 160) {
					boss_direction = UP;
					Update_Boss();
					return;
				}
				boss->xold = boss->x;
				boss->yold = boss->y;
				boss->x += 1;
			}
			break;
	}
}

void Update_Enemy_Bullets(void){
	for (int i = 0; i < NUM_ENEMY_BULLETS; i++) {
		if (enemy_bullets[i]->valid > 0) {
			if ((enemy_bullets[i]->x-enemy_bullets[i]->width) > 0) {
				if (Is_Hit(my_ship,enemy_bullets[i]))	{
					my_ship->hp -= enemy_bullets[i]->dmg;
					if (my_ship->hp < 0) {
						flag_GameOver = 1;
					}
				}
				else {
					enemy_bullets[i]->xold = enemy_bullets[i]->x;
					enemy_bullets[i]->yold = enemy_bullets[i]->y;
					enemy_bullets[i]->x -= 1;
				}
			}
			else enemy_bullets[i]->valid = 0;
		}
	}	
}

void Update_My_Ship(int direction){
	my_ship->xold = my_ship->x;
	my_ship->yold = my_ship->y;
	my_ship->y += direction;
}

void Update_My_Bullets(void){
	for (int i = 0; i < NUM_MY_BULLETS; i++) {
		if (my_bullets[i]->valid > 0) {
			if (my_bullets[i]->x < 160) {
				my_bullets[i]->xold = my_bullets[i]->x;
				my_bullets[i]->yold = my_bullets[i]->y;
				my_bullets[i]->x += 1;
			}
			else my_bullets[i]->valid = 0;
		}
	}
}

void Init_Level(int stage){
	int gp_counter = 0;
	srand(time(NULL));
	switch (stage) {
		case 1:
			//Initialize Enemy Grid
			for (int i = 0; i < NUM_MAX_ENEMIES_COLUMN; i++) {
				for (int j = 0; j < NUM_MAX_ENEMIES_ROW; j++) {
					*enemies[i][j] = Enemy[rand()%3];
					enemies[i][j]->x = enemyGrid[gp_counter].x;
					enemies[i][j]->y = enemyGrid[gp_counter].y;
				}
			}
		
			//Initialize Player Ship
			*my_ship = Player;
		
			//initialize static variables
			remaining_Enemies = 20;
			current_stage = 1;
			break;
			
		case 2:
			//Initialize Boss
			*boss = Boss[rand()%2];
			
			//Initialize Player Ship
			*my_ship = Player;
		
			//initialize static variables
			remaining_Enemies = 1;
			current_stage = 2;
			break;
	}
}

void Add_Bullets(int ship){
	switch (ship) {
		case 0:
			for (int i = 0; i < NUM_MY_BULLETS; i++) {
				if (my_bullets[i]->valid != 0 && my_bullets[i]->valid != 1 && my_ship->valid > 0) {
					my_bullets[i]->x = my_ship->x + my_ship->width + 1;
					my_bullets[i]->xold = -100;
					my_bullets[i]->y = my_ship->y - (my_ship->height/2);
					my_bullets[i]->yold = -100;
					my_bullets[i]->valid = 1;
					my_bullets[i]->dmg = 1;
					my_bullets[i]->height = BULLET_HEIGHT;
					my_bullets[i]->width = BULLET_WIDTH;
				}
			}
			break;
		case 1: 
			for (int i = 0; i < NUM_ENEMY_BULLETS; i++) {
				if (enemy_bullets[i]->valid != 0 && my_bullets[i]->valid != 1 && remaining_Enemies > 0) {
					Ship tmp = get_RndShip();
					enemy_bullets[i]->x = tmp.x - tmp.width - 1;
					enemy_bullets[i]->xold = -100;
					enemy_bullets[i]->y = tmp.y - (tmp.height/2);
					enemy_bullets[i]->yold = -100;
					enemy_bullets[i]->valid = 1;
					enemy_bullets[i]->dmg = 1;
					enemy_bullets[i]->height = BULLET_HEIGHT;
					enemy_bullets[i]->width = BULLET_WIDTH;
				}
			}
			break;
		case 2:
			for (int i = 0; i < NUM_MY_BULLETS; i++) {
				if (!my_bullets[i]->valid != 0 && my_bullets[i]->valid != 1 && boss->valid > 0) {
					enemy_bullets[i]->x = boss->x - boss->width - 1;
					enemy_bullets[i]->xold = -100;
					enemy_bullets[i]->y = boss->y - (boss->height/2);
					enemy_bullets[i]->yold = -100;
					enemy_bullets[i]->valid = 1;
					enemy_bullets[i]->dmg = 2;
					enemy_bullets[i]->height = BULLET_HEIGHT;
					enemy_bullets[i]->width = BULLET_WIDTH;
				}
			}
	}
}

int Is_GameOver(void) {
	return flag_GameOver;
}

int Is_LevelWon(void) {
	return (!remaining_Enemies);
}

int Is_GameWon(void) {
	return flag_GameWin;
}

// ------------------------ Get Functions ------------------------
shipMatrix * Get_Enemies(void) {
	return &enemies;
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