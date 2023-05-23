//Gra Bomberman przy uzyciu biblioteki Allegro5 
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "Header.h"
#include "functions.h"

#define BLOCKSIZE 64
#define W 960
#define H 960
#define FPS 60
enum Dir { UP, RIGHT, DOWN, LEFT };


//-------------------------------------main------------------------------------------------------------------//
int main()
{
	//inicjalizacja allegro5
	al_init(); if (!al_init()) return -1;

	//stworzenie ekranu gry
	ALLEGRO_DISPLAY* display = al_create_display(W, H); if (!display) return -2;
	//stworzenie bitmap
	ALLEGRO_BITMAP* player = NULL;
	ALLEGRO_BITMAP* bomb_bitmap = NULL;
	ALLEGRO_BITMAP* map = NULL;
	
	
	//stworzenie timera
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS); if (!timer) return -4;

	//stworzenie kolejki wydarzen
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue(); if (!event_queue) return -5;

	//-----------------------------zmienne---------------------------------------------------//
	int loadCounterX = 0, loadCounterY = 0, mapSizeX, mapSizeY;

	int map_arr[15][15] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
							1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
							1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
						



	bool done = false, draw = true, active = false;
	int dir = DOWN, move_speed = 6, x = W / 2, y = H / 2, size = 100, enemy_1_X = 0, enemy_2_X = 0;
	int sourcex = 0, sourcey = 128, refresh = 0;;
	double bomb_time[100];
	double t0 = 0, t1 = 0;
	int sourceBombX = 0, sourceBombY = 0;
	bool bombActive[100] = { 0 };
	
	Bomb bomb[100];
	Player player1;

	init_player(&player1);
	init_bomb(bomb, size);

	Enemy enemy_player[1];
	init_enemy(enemy_player, 1);

	ALLEGRO_KEYBOARD_STATE key_state;

	al_install_keyboard();
	al_init_image_addon();
	al_init_primitives_addon();

	//-----------------------------dzwiek w grze -----------------------------------//
	//inicjalizacja audio
	al_install_audio();
	al_init_acodec_addon();

	//funkcja do ustawienia ile dzwiekow bedzie grane na raz
	al_reserve_samples(1);

	//stworzenie sampli
	ALLEGRO_SAMPLE* steps = al_load_sample("steps.wav");
	ALLEGRO_SAMPLE* plant_bomb = al_load_sample("bop.wav");
	ALLEGRO_SAMPLE* background = al_load_sample("ageofwar.wav");
	ALLEGRO_SAMPLE* bomb_explosion = al_load_sample("bang.wav");

	//inicjacja instancji sampli
	ALLEGRO_SAMPLE_INSTANCE* step_instance;
	ALLEGRO_SAMPLE_INSTANCE* bomb_instance;
	ALLEGRO_SAMPLE_INSTANCE* background_instance;
	ALLEGRO_SAMPLE_INSTANCE* bomb_explosion_instance;

	//stworzenie instancji samplo
	step_instance = al_create_sample_instance(steps);

	//dodanie sampla do mixera
	al_attach_sample_instance_to_mixer(step_instance, al_get_default_mixer());

	//ustawienie zmiennych dzwieku
	al_set_sample_instance_gain(step_instance, 0.5);
	al_set_sample_instance_speed(step_instance, 0.4);
	al_set_sample_instance_playmode(step_instance, ALLEGRO_PLAYMODE_ONCE);

	bomb_instance = al_create_sample_instance(plant_bomb);
	al_attach_sample_instance_to_mixer(bomb_instance, al_get_default_mixer());
	al_set_sample_instance_gain(bomb_instance, 0.3);
	al_set_sample_instance_playmode(bomb_instance, ALLEGRO_PLAYMODE_ONCE);

	background_instance = al_create_sample_instance(background);
	al_attach_sample_instance_to_mixer(background_instance, al_get_default_mixer());
	al_set_sample_instance_gain(background_instance, 0.1);
	al_set_sample_instance_playmode(background_instance, ALLEGRO_PLAYMODE_LOOP);

	bomb_explosion_instance = al_create_sample_instance(bomb_explosion);
	al_attach_sample_instance_to_mixer(bomb_explosion_instance, al_get_default_mixer());
	al_set_sample_instance_gain(bomb_explosion_instance, 0.3);
	//----------------------------------------------------------------------------------------//

	//zaladowanie bitmap
	player = al_load_bitmap("player.png"); if (!player) return -3;
	map = al_load_bitmap("map.png"); if (!map) return -4;
	bomb_bitmap = al_load_bitmap("bomb_explo_rem.png"); if (!bomb_bitmap) return -5;

	//map properties 
	int map_width = al_get_bitmap_width(map);
	int map_height = al_get_bitmap_height(map);


	//rejestracja wydarzen w kolejce wydarzen 
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	//wlaczenie muzyki w tle
	al_play_sample_instance(background_instance);

	//wlaczenie timera gry
	al_start_timer(timer);

	//----------------------------------------glowna petla gry-----------------------------------------//
	while (!done)
	{
		refresh++;
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) //sprawdzanie wydarzenia nacisniecia przycisku
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_SPACE:
				plantBomb(bomb, size, &player1, bomb_time, bombActive);
				al_play_sample_instance(bomb_instance);
				//bombActive = true;
				break;
			}
		}

		if (ev.type == ALLEGRO_EVENT_TIMER) // wydarzenia zalezne od timera
		{
			active = true;
			al_get_keyboard_state(&key_state);
			if (al_key_down(&key_state, ALLEGRO_KEY_DOWN))
			{
				movePlayerDown(&player1);
				dir = DOWN;
				al_play_sample_instance(step_instance);

			}

			else if (al_key_down(&key_state, ALLEGRO_KEY_UP))
			{
				movePlayerUp(&player1);
				dir = UP;
				al_play_sample_instance(step_instance);
			}

			else if (al_key_down(&key_state, ALLEGRO_KEY_RIGHT))
			{
				movePlayerRight(&player1);
				dir = RIGHT;
				al_play_sample_instance(step_instance);
			}

			else if (al_key_down(&key_state, ALLEGRO_KEY_LEFT))
			{
				movePlayerLeft(&player1);
				dir = LEFT;
				al_play_sample_instance(step_instance);
			}
			else
				active = false;

			draw = true;
		}


		if (refresh == 6) // dla zmienienia ilosci klatek w wyswietlaniu postaci
		{
			//do animacji bomby
			if (bombActive)
			{
				sourceBombX += al_get_bitmap_width(bomb_bitmap) / 4;

			}
			if (sourceBombX >= al_get_bitmap_width(bomb_bitmap))
				sourceBombX = 0;

			//do animacji postaci
			if (active)
				sourcex += al_get_bitmap_width(player) / 7;

			else
				sourcex = 0;

			if (sourcex >= 448)
				sourcex = 0;
			sourcey = dir;

			refresh = 0;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) //mozliwosc zamkniecia gry przez X
		{
			done = true;
		}

		if (draw)
		{
			draw = false;
			
			drawMap(map_arr,map);
			drawBomb(bomb, size, bomb_bitmap, sourceBombX, sourceBombY);
			al_draw_bitmap_region(player, sourcex, sourcey * al_get_bitmap_width(player) / 7, 64, 64, player1.x, player1.y, NULL);
	
			al_flip_display();
			al_clear_to_color(al_map_rgb(255, 255, 255));
		}
		destructBomb(bomb, size, &player1, bomb_time, t1, bombActive, bomb_explosion_instance);
		collision(map_arr[15][15], &player1, dir);
	}
	//----------------------------------------------------------------------------------------------------//

	//---------------------------------------zniszczenie wskaznikow-----------------------------------//
	al_destroy_display(display);
	al_destroy_bitmap(player);
	al_destroy_bitmap(bomb_bitmap);

	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);

	al_destroy_sample_instance(bomb_instance);
	al_destroy_sample_instance(step_instance);
	al_destroy_sample_instance(background_instance);

	return 0;
}

//------------------------------------------definicje funkcji---------------------------------------------------//
void init_player(Player* p)
{
	p->ID = PLAYER;
	p->x = W / 2;
	p->y = H / 2;
	p->lives = 4;
	p->speed = 5;
	p->middle_point_x = p->x / 2; //srodek gracza
	p->middle_point_y = p->y / 2;
	p->boundx = 7;
	p->boundy = 7;
	p->score = 0;
	p->bomb = false;

}
void init_bomb(Bomb b[], int size)
{
	for (int i = 0; i < size; i++)
	{
		b[i].ID = BOMB;
		b[i].alive = false;
		b[i].time = 5.0;

	}
}
void movePlayerUp(Player* p)
{
	p->y -= p->speed;
	if (p->y < 40)
	{
		p->y = 40;
	}
}
void movePlayerDown(Player* p)
{
	p->y += p->speed;
	if (p->y > H - 105)
		p->y = H -105;
}
void movePlayerRight(Player* p)
{
	p->x += p->speed;
	if (p->x > W - 105)
		p->x = W - 105;
}
void movePlayerLeft(Player* p)
{
	p->x -= p->speed;
	if (p->x < 40)
		p->x = 40;
}
void drawBomb(Bomb bomb[], int size, ALLEGRO_BITMAP* bomb_bitmap, int sourceBombX, int sourceBombY)
{
	for (int i = 0; i < size; i++)
	{
		if (bomb[i].alive)
		{

			//al_draw_bitmap(bomb_bitmap, bomb[i].x, bomb[i].y, NULL);
			al_draw_bitmap_region(bomb_bitmap, sourceBombX, sourceBombY, al_get_bitmap_width(bomb_bitmap) / 4, al_get_bitmap_height(bomb_bitmap), bomb[i].x, bomb[i].y, NULL);
			//al_draw_filled_circle(bomb[i].x, bomb[i].y, 10, al_map_rgb(0, 0, 0));
		}
	}

}
void plantBomb(Bomb b[], int size, Player* p, double bomb_time[], bool bombActive[])
{
	for (int i = 0; i < size; i++)
	{
		if (!b[i].alive)
		{
			bomb_time[i] = al_get_time();
			b[i].x = p->x;
			b[i].y = p->y;
			b[i].alive = true;
			bombActive[i] = true;
			break;
		}
	}
}
void destructBomb(Bomb b[], int size, Player* p, double bomb_time[], double t1, bool bombActive[], ALLEGRO_SAMPLE_INSTANCE* bomb_explosion_instance)
{
	for (int i = 0; i < size; i++)
	{
		if (b[i].alive)
		{
			t1 = al_get_time();
			if (t1 - bomb_time[i] > 5)
			{
				b[i].alive = false;
				bombActive[i] = false;
				if (bombActive[i] == false)
					al_play_sample_instance(bomb_explosion_instance);
				else
					continue;
			}
		}
	}
}
void init_enemy(Enemy e[], int num_enemies)
{
	for (int i = 0; i < num_enemies; i++)
	{
		srand(time(NULL));
		e[i].ID = ENEMY;
		e[i].x = rand() % 700;
		e[i].y = rand() % 700;
		e[i].alive = true;
		e[i].speed = 4;

	}


}




void collision()
{
	
}


void drawMap(int map_arr[15][15], ALLEGRO_BITMAP* map)
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (map_arr[i][j] == 0)
			{
				al_draw_bitmap_region(map,64,0,BLOCKSIZE,BLOCKSIZE,j*BLOCKSIZE,i*BLOCKSIZE,NULL);
			}
			else if (map_arr[i][j] == 1)
			{
				al_draw_bitmap_region(map,0,0,BLOCKSIZE,BLOCKSIZE,j*BLOCKSIZE,i*BLOCKSIZE,NULL);
			}
		}
	}
}