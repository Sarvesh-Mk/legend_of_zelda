#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <main.h>
#include <string.h>
#include <stdbool.h>

int mapSizeX=16,mapSizeY=11,mapS=64,offset=0,worldX=7,worldY=7,screenWidth=1024,screenHeight=704;
int Dir[4][2] = {{-1,0},{0,1},{1,0},{0,-1}};
entity * player;
entity * enemies[11][16];

// CPU framebuffer — all pixels are written here then uploaded once per frame
unsigned char framebuffer[FB_H][FB_W][3];
static unsigned int fb_tex;

void fb_clear(void){
    memset(framebuffer, 77, sizeof(framebuffer)); // 77/255 ≈ 0.3 grey
}

void fb_flush(void){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fb_tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FB_W, FB_H, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    // Row 0 of framebuffer = top of screen (y=0 in our y-down ortho).
    // GL uploads row 0 at v=0 (normally bottom), so map v=0 to screen top.
    glTexCoord2f(0,0); glVertex2i(0,    0);
    glTexCoord2f(1,0); glVertex2i(FB_W, 0);
    glTexCoord2f(1,1); glVertex2i(FB_W, FB_H);
    glTexCoord2f(0,1); glVertex2i(0,    FB_H);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

static void fb_init(void){
    glGenTextures(1, &fb_tex);
    glBindTexture(GL_TEXTURE_2D, fb_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FB_W, FB_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}

void frameTimer(int id){
    for(int y=0;y<mapSizeY;y++){for(int x=0;x<mapSizeX;x++){animation(enemies[y][x]);}}
    animation(player);
    player->standing=true;
    glutTimerFunc(100, frameTimer, 0);
}

void gameEvents(int id){
    for(int y=0;y<mapSizeY;y++){for(int x=0;x<mapSizeX;x++){if(enemies[y][x]->alive && !enemies[y][x]->hurt){walkCycle(enemies[y][x]);}}}
    glutTimerFunc(1000, gameEvents, 0);
}

void allocEnemies(){
    for(int y=0;y<mapSizeY;y++){
        for(int x=0;x<mapSizeX;x++){
            enemies[y][x] = initEntity(64,64,x*mapS,y*mapS,1,0,2,1,redOctorockTextureCoords,1);
            enemies[y][x]->weapon = initWeapon(enemies[y][x],64,64,4,1,false,arrowTextureCoords);
            enemies[y][x]->alive = false;
        }
    }
}

void readMap(){
    for(int y=0;y<mapSizeY;y++){
        for(int x=0;x<mapSizeX;x++){
            map[y][x] = worldMapBlocking[worldY*11+y][worldX*16+x];
            spriteWorldMap[y][x] = worldMapTiles[worldY*11+y][worldX*16+x];
            if(map[y][x]==2){
                changeEntity(enemies[y][x], x*mapS, y*mapS, worldX*16+x, worldY*11+y, 2, redOctorockTextureCoords,true);
            }
            else{changeEntity(enemies[y][x],0,0,x,y,1,redOctorockTextureCoords,false);}
        }
    }
    drawWorld();
}

void updatePlayer(){
    if(collisionBorder(player->x,player->y,player->width,player->length,true)==1){readMap();scrollMap(player);}
    updateWeapon(player->weapon,player);
    drawEntity(player);
    for(int y=0;y<mapSizeY;y++){
        for(int x=0;x<mapSizeX;x++){
            if(enemies[y][x]->alive){
                EntityCollision(player,enemies[y][x]);
            }
        }
    }
}

void drawMap(){
    int x,y,xo,yo;
    for(y=0;y<mapSizeY;y++){
        for(x=0;x<mapSizeX;x++){
            if(map[y][x]==0){drawWall(1,x,y,xo,yo,mapS,offset);}else{drawWall(0,x,y,xo,yo,mapS,offset);}
        }
    }
}

void display(){
    fb_clear();
    drawWorld();
    update_enemies();
    updatePlayer(player);
    fb_flush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void init(){
    fb_init();
    gluOrtho2D(0,screenWidth,screenHeight,0);
    allocEnemies();
    readMap();
    player = initEntity(64,64,5*mapS,5*mapS,8,0,6,2,playerTextureCoords,1);
    player->weapon = initWeapon(player,64,64,8,1,true,swordTextureCoords);
    frameTimer(0);
    gameEvents(0);
}

int main(int argc, char * argv[]){
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("legend of zelda");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}
