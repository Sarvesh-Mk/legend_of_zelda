#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <main.h>
#include <string.h>
#include <stdbool.h>

void drawWeapon(Weapon * w){
    float modColor;
    if(w->changeColor==true){modColor = (rand()%75)/255.0f;}
    else{modColor=0;}
    for (int y=0;y<w->length;y++){
        for (int x=0;x<w->width;x++){
            int a = y+w->textureCoords[w->state][1];
            int z = x+w->textureCoords[w->state][0];
            if(spriteAtlas[a][z][0] <= 1){
                glColor3f(spriteAtlas[a][z][0]+modColor,spriteAtlas[a][z][1]+modColor,spriteAtlas[a][z][2]+modColor);
                glVertex2i(w->x+x,w->y+y);
            }
        }
    }
}


entity * initEntity(int width, int height, int x, int y, int speed, int state, int health, int animMax, float textureCoords[8][2], int ifWeapon){
    entity *e;
    e = (entity *)malloc(sizeof(entity));
    e->x = x; e->y = y;e->animMax=animMax;
    e->width=width;e->length=height;
    e->speed=speed;e->state=state;e->frame=false;e->standing=false;
    e->frame=false;e->health=health;e->hurt=false;e->alive=true;
    e->animTimer=0;
    memcpy(e->textureCoords, textureCoords, sizeof(e->textureCoords));
    return e;
}

entity * changeEntity(entity * e, int x, int y, int ox, int oy, int health, float textureCoords[8][2], bool alive){
    e->x = x; e->y = y; e->health=health; e->alive=alive; e->ox=ox; e->oy=oy;
    memcpy(e->textureCoords, textureCoords, sizeof(e->textureCoords));
}

entity * animation(entity * e){
    if(e->standing==false && e->alive && !e->hurt){e->frame = !e->frame;}
    if(e->hurt){
        obstacleCollision(e);
        e->animTimer++; 
        if(e->animTimer>=e->animMax){e->dx=0;e->dy=0;}
        if(e->animTimer>=e->animMax+2){e->animTimer=0;e->hurt=false;}
    }
    if(!e->alive){
        e->animTimer++;
    }
    return e;
}

entity * EntityCollision(entity * e, entity * target){
    int d = 0;
    if(target->weapon->used==true){d = dynamicCollision(e->x,e->width,target->weapon->x,target->weapon->width,e->y,e->length,target->weapon->y,target->weapon->length);}
    if(d==1){
        e->dx=target->weapon->dx;e->dy=target->weapon->dy;
        resetWeapon(target->weapon,target);
        e->health--;
        if(e->health<=0){e->alive=false;worldMapBlocking[e->oy][e->ox]=1;}
        else{e->hurt=true;}
    }
    return e;
}


Weapon * initWeapon(entity * ep,int width,int length, int speed, int damage, bool changeColor, float textureCoords[4][2]){
    Weapon *e;
    e = (Weapon *)malloc(sizeof(Weapon));
    e->x=ep->x; e->y=ep->y;e->dx=0;e->dy=0;
    e->width=width; e->length=length;
    e->used=0;e->state=0;e->speed=speed;
    e->damage = damage; e->changeColor = changeColor;
    memcpy(e->textureCoords,textureCoords,sizeof(e->textureCoords));
    return e;
}

void drawWall(int wallSprite, int x,int y,int xo,int yo, int mapS, int offset){
    xo=x*mapS; yo=y*mapS;
    for (y=0;y<mapS;y++){
        for (x=0;x<mapS;x++){
            glColor3f(wall[wallSprite][y*mapS+x][0],wall[wallSprite][y*mapS+x][1],wall[wallSprite][y*mapS+x][2]);
            glVertex2i(xo+offset+x,yo+offset+y);
        }
    }
}

void drawEntity(entity * e){
    float modColor;
    if(e->hurt==true){modColor = (rand()%75)/255.0f;}
    else{modColor=0;}
    for (int y=0;y<e->length;y++){
        for (int x=0;x<e->width;x++){
            int a = y+e->textureCoords[e->state*2+e->frame][1];
            int z = x+e->textureCoords[e->state*2+e->frame][0];
            if(spriteAtlas[a][z][0] <= 1){
                glColor3f(spriteAtlas[a][z][0]+modColor,spriteAtlas[a][z][1]+modColor,spriteAtlas[a][z][2]+modColor);
                glVertex2i(e->x+x,e->y+y);
            }
        }
    }
}

void drawWorldSprite(int wx, int wy, int textureCoords[2]){
    for (int y=0;y<mapS;y++){
        for (int x=0;x<mapS;x++){
            int a = y+textureCoords[1];
            int z = x+textureCoords[0];
            glColor3f(worldMapTilesSprites[a][z][0],worldMapTilesSprites[a][z][1],worldMapTilesSprites[a][z][2]);
            glVertex2i(wx+x,wy+y);
        }
    }
}

void drawWorld(){
    for (int y=0;y<mapSizeY;y++){
        for (int x=0;x<mapSizeX;x++){
            drawWorldSprite(x*mapS, y*mapS, worldMapSpritesTextureCoords[spriteWorldMap[y][x]]);
        }
    }
}