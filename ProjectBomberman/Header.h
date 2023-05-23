#pragma once

enum IDS { PLAYER, BOMB, ENEMY };

struct Player {
	int ID;
	int x;
	int y;
	int lives;
	int speed;
	int boundx;
	int middle_point_x;
	int middle_point_y;
	int boundy;
	int score;
	bool bomb;
};
typedef struct Player Player;

struct Bomb {
	int ID;
	int x;
	int y;
	bool alive;
	int time;
};
typedef struct Bomb Bomb;
struct Enemy {
	int ID;
	int x;
	int y;
	bool alive;
	int speed;
	int boundx;
	int boundy;
};
typedef struct Enemy Enemy;

typedef struct Map {
	int x;
	int y;

}Map;