#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern int mapSizeX;extern int mapSizeY;extern int mapS;extern int offset;extern int worldX;extern int worldY;extern int screenWidth; extern int screenHeight;

extern float wall[2][64*64][3];
extern float playerTextureCoords[8][2];
extern float redMoblinTextureCoords[8][2];
extern float redOctorockTextureCoords[8][2];
extern float swordTextureCoords[4][2];
extern float arrowTextureCoords[4][2];
extern float spriteAtlas[192][1536][3];
extern int worldMapBlocking[88][256];
extern int worldMapTiles[88][256];
extern int worldMapSpritesTextureCoords[144][2];
extern float worldMapTilesSprites[544][1224][3];
extern int spriteWorldMap[11][16];
extern int Dir[4][2];

extern int map[11][16];

typedef struct Weapon{
    int x;
    int y;
    int dx;
    int dy;
    int state;
    int length;
    int width;
    int speed;
    int damage;
    bool frame;
    bool used;
    bool changeColor;
    float textureCoords[4][2];
} Weapon;

typedef struct Entity{
    int x, y, ox, oy;
    int dx;
    int dy;
    int state;
    int length;
    int width;
    int speed;
    int health;
    int animTimer;
    int animMax;
    bool hurt;
    bool frame;
    bool standing;
    bool alive;
    float textureCoords[8][2];
    Weapon * weapon;
} entity;

extern entity * player;
extern entity * enemies[11][16];

// Framebuffer: write pixels here, flushed to screen each frame as a single texture upload
#define FB_W 1024
#define FB_H 704
extern unsigned char framebuffer[FB_H][FB_W][3];

static inline unsigned char fb_clamp(float v){
    if(v >= 1.0f) return 255;
    if(v <= 0.0f) return 0;
    return (unsigned char)(v * 255.0f);
}

static inline void fb_pixel(int x, int y, float r, float g, float b){
    if((unsigned)x >= FB_W || (unsigned)y >= FB_H) return;
    framebuffer[y][x][0] = fb_clamp(r);
    framebuffer[y][x][1] = fb_clamp(g);
    framebuffer[y][x][2] = fb_clamp(b);
}

void fb_clear(void);
void fb_flush(void);

//entity
void drawWeapon(Weapon * w);
void drawEntity(entity * entity);
void drawWall(int wallSprite, int x,int y,int xo,int yo, int mapS, int offset);
entity * initEntity(int width, int height, int x, int y, int speed, int state, int health, int animMax, float textureCoords[8][2], int ifWeapon);
Weapon * initWeapon(entity * ep,int width,int length, int speed, int damage, bool changeColor, float textureCoords[4][2]);
entity * walkCycle(entity * e);
entity * animation(entity * e);
entity * changeEntity(entity * e, int x, int y, int ox, int oy, int health, float textureCoords[8][2], bool alive);

//math
int staticCollision(int map[11][16], int x1, int y1, int width, int length);
int dynamicCollision(int x1, int width1, int x2, int width2, int y1, int length1, int y2, int length2);
Weapon * updateWeapon(Weapon * weapon, entity * player);
Weapon * resetWeapon(Weapon * weapon,entity * e);
entity * EntityCollision(entity * e, entity * target);
void buttons(unsigned char key, int x, int y);
entity * obstacleCollision(entity * e);
int collisionBorder(int x, int y,int width,int length, bool ifChange);
int scrollMap(entity * e);
void drawWorldSprite(int x, int y, int textureCoords[2]);
void drawWorld();
void update_enemies();
