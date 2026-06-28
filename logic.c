#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <main.h>
#include <string.h>
#include <stdbool.h>

int staticCollision(int map[11][16], int x1, int y1, int width, int length){
    for (int y=0;y<mapSizeY;y++){
        for (int x=0;x<mapSizeX;x++){
            if (map[y][x]==0){
                if(x1+width>x*mapS && x1<x*mapS+mapS && y1+length>y*mapS && y1+length/2<y*mapS+mapS){
                    return 1;
                }
            }
        }
    }
    return 0;
}

int dynamicCollision(int x1, int width1, int x2, int width2, int y1, int length1, int y2, int length2){
    if(x1+width1>x2 && x1<x2+width2 && y1+length1>y2 && y1<y2+length2){
        return 1;
    }
    else {return 0;}

}

int collisionBorder(int x, int y,int width,int length, bool ifChange){
    if(x<0){if(ifChange){worldX-=1;}return 1;}
    if(x+width>=screenWidth){if(ifChange){worldX+=1;}return 1;}
    if(y<0){if(ifChange){worldY-=1;}return 1;}
    if(y+length>=screenHeight){if(ifChange){worldY+=1;}return 1;}
    return 0;
}


int scrollMap(entity * e){
    if(e->x<0){e->x=screenWidth-e->width;return 1;}
    if(e->x+e->width>=screenWidth){e->x=0+e->width/2;return 1;}
    if(e->y<0){e->y=screenHeight-e->length;return 1;}
    if(e->y+e->length>=screenHeight){e->y=0+e->length/2;return 1;}
    return 0;
}

Weapon * resetWeapon(Weapon * weapon,entity * e){
    weapon->used=0;
    weapon->dx=0;weapon->dy=0;
    weapon->x=e->x+e->width-weapon->width;weapon->y=e->y+e->length-weapon->length;
    return weapon;
}

Weapon * updateWeapon(Weapon * weapon, entity * e){
    if(weapon->used==1){
        int c = staticCollision(map,weapon->x,weapon->y,weapon->width,weapon->length);
        int d = collisionBorder(weapon->x,weapon->y,weapon->width/2,weapon->length/2,false);
        if(c==0 && d==0){
            drawWeapon(weapon);
            weapon->x+=weapon->dx; weapon->y+=weapon->dy;
        }
        else if(c==1 || d==1){resetWeapon(weapon,e);}
    }
    else{
        weapon->state = e->state;
        weapon->x=e->x+e->width-weapon->width;weapon->y=e->y+e->length-weapon->length;;
    }
    return weapon;
}

entity * walkCycle(entity * e){
    int r = rand()%8;
    if(r>=4){
        e->dx=0;e->dy=0;e->standing=true;
        if(e->weapon->used!=1 && r==4){
            e->weapon->used=1;
            e->weapon->dx=Dir[e->state][0]*e->weapon->speed;
            e->weapon->dy=Dir[e->state][1]*e->weapon->speed;
        }
    }
    else{
        e->state = r;e->standing=false;
        e->dx = Dir[e->state][0];e->dy = Dir[e->state][1];
    }
    return e;
}

entity * obstacleCollision(entity * e){
    int c = staticCollision(map,e->x+e->dx*e->speed,e->y+e->dy*e->speed,e->width,e->length);
    if(c == 0){e->x+=e->dx*e->speed;e->y+=e->dy*e->speed;}
    else{e->frame=0;e->standing=true;}
}

void update_enemies(){
    for(int y=0;y<mapSizeY;y++){
        for(int x=0;x<mapSizeX;x++){
            if(enemies[y][x]->alive){
                EntityCollision(enemies[y][x],player);
                drawEntity(enemies[y][x]);
                int c = staticCollision(map,enemies[y][x]->x+enemies[y][x]->dx,enemies[y][x]->y+enemies[y][x]->dy,enemies[y][x]->width,enemies[y][x]->length);
                if(c == 0){
                    enemies[y][x]->x+=enemies[y][x]->dx;enemies[y][x]->y+=enemies[y][x]->dy;
                }
                else{enemies[y][x]->standing=true;}
            }
            updateWeapon(enemies[y][x]->weapon,enemies[y][x]);
        }
    }
}

void buttons(unsigned char key, int x, int y){
    if(key=='a' && !player->hurt) {player->dx=-1;player->dy=0;player->state=0;}
    if(key=='d' && !player->hurt) {player->dx=1;player->dy=0;player->state=2;}
    if(key=='w' && !player->hurt) {player->dy=-1;player->dx=0;player->state=3;}
    if(key=='s' && !player->hurt) {player->dy=1;player->dx=0;player->state=1;}
    if(key==32) {
        if(player->weapon->used==0){
            player->weapon->used=1;
            player->weapon->dx=Dir[player->state][0]*player->weapon->speed; player->weapon->dy=Dir[player->state][1]*player->weapon->speed;
        }
        player->standing=true;
    }
    if (key=='a' || key=='d' || key=='w' || key=='s'){
        obstacleCollision(player);
        player->standing=false;
    }
    glutPostRedisplay();
}