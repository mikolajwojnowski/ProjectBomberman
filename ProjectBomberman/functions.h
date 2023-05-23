#pragma once
#pragma once
#include "Header.h"
void init_bomb(Bomb bomb[], int size);
void init_player(Player* p);
void movePlayerLeft(Player* p);
void movePlayerDown(Player* p);
void movePlayerRight(Player* p);
void movePlayerUp(Player* p);
void destructBomb(Bomb b[], int size, Player* p, double bomb_time[], double t1, bool bombActive[], ALLEGRO_SAMPLE_INSTANCE* bomb_explosion_instance);
void plantBomb(Bomb b[], int size, Player* p, double bomb_time[], bool bombActive[]);
void drawBomb(Bomb bomb[], int size, ALLEGRO_BITMAP* bomb_bitmap, int sourceBombX, int sourceBombY);
void init_enemy(Enemy e[], int num_enemies);
void drawMap(int map_arr[15][15], ALLEGRO_BITMAP* map);
void collision(int map_arr[15][15], Player* p, int dir);
