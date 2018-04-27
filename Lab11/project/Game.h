#define			MY_SHIP									0
#define			ENEMY1									1
#define			ENEMY2									2
#define			ENEMY3									3
#define			BOSS1										10
#define			BOSS2										11
#define 		NUM_MAX_ENEMIES_ROW			5
#define			NUM_MAX_ENEMIES_COLUMN	4
#define 		NUM_ENEMY_BULLETS				100
#define 		NUM_MY_BULLETS					20
#define 		NUM_MAX_EXPLOSIONS			21

// reserve 4-9 for adding more enemy types in the future


typedef struct {
	int x;
	int y;
	int vx;
	int vy;
	int width;
	int height;
	int hp;
	int type; //0 for my ship; 1, 2, or 3 for enemy ships (for drawing)
}Ship;

typedef struct {
	int x;
	int y;
	int vx;
	int vy;
	int width;
	int height;
	int valid;
}Bullet;

typedef struct {
	int x;
	int y;
	int stage;
}Explosion;


// functions that main() calls
void Init_Level(int stage);
void Update_Every_Object(void);
int Is_GameOver(void);
int Is_LevelWon(void);	

// ------------------ Get Functions ------------------
Ship ** Get_Enemies(void);
Ship * Get_Boss(void);
Ship * Get_My_Ship(void);
Bullet ** Get_Enemy_Bullets(void);
Bullet ** Get_My_Bullets(void);
Explosion ** Get_Explosions(void);
