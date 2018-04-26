#define			MY_SHIP						0
#define			ENEMY1						1
#define			ENEMY2						2
#define			ENEMY3						3
#define			BOSS1							10
#define			BOSS2							11
#define 		NUM_ENEMIES				20
#define 		NUM_ENEMY_BULLETS	10
#define 		NUM_MY_BULLETS		10
// reserve 4-9 for adding more enemy types in the future


typedef struct {
	int x;
	int y;
	int width;
	int height;
	int hp;
	int type; //0 for my ship; 1, 2, or 3 for enemy ships (for drawing)
}Ship;

typedef struct {
	int x;
	int y;
	int direction;
	int has_hit;
}Bullet;


int current_stage;

Ship * enemies[NUM_ENEMIES];
Ship * my_ship;
Ship * boss;

Bullet * enemy_bullets[NUM_ENEMY_BULLETS];
Bullet * my_bullets[NUM_MY_BULLETS];

// the only 2 functions that main() calls
void Init_Level(int stage);
void Update_Every_Object(void);

