//
//  snake.cpp
//  Snake
//
//  Created by Austin Jacobs on 8/19/16.
//  Copyright © 2016 Austin Jacobs. All rights reserved.
//

#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cstdlib>
#include <queue>

using namespace std;

#define FREE 0
#define SNAKE 1
#define WALL 2

#define snake_color al_map_rgb(4,112,24)
#define food_color al_map_rgb(173,7,115) 
#define background_color al_map_rgb(224, 221, 197)
#define wall_color al_map_rgb(92,50,10)


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int SQUARE_WIDTH = 16;
const int GRID_WIDTH = (SCREEN_WIDTH/SQUARE_WIDTH) - 2; //ignore boardering space
const int GRID_HEIGHT = (SCREEN_HEIGHT/SQUARE_WIDTH) - 2; 

struct Snake{
    vector< vector<int> > grid; 
    pair<int,int> front;
    pair<int,int> tail;
    queue<int> dirs; //tail at front, head at back
    int length;
};

void moveCoords(pair<int,int> &front, int dir){
    switch(dir) {
        case ALLEGRO_KEY_LEFT:
            front.first--;
            break;
        case ALLEGRO_KEY_RIGHT:
            front.first++;
            break;
        case ALLEGRO_KEY_UP:
            front.second--;
            break;
        case ALLEGRO_KEY_DOWN:
            front.second++;
            break;
    }
}

void addRect(pair<int,int> coord, ALLEGRO_COLOR col){
    float x = coord.first * SQUARE_WIDTH + SQUARE_WIDTH;
    float y = coord.second * SQUARE_WIDTH + SQUARE_WIDTH;
    al_draw_filled_rounded_rectangle(x, y, x+SQUARE_WIDTH-1, y+SQUARE_WIDTH-1, 2, 2, col);
}

pair<int, int> addFood(Snake *snake){	
	int x = 0;
	int y = 0;
	while(snake->grid[y][x] != FREE) {	
		x = rand() % (GRID_WIDTH-2) + 1; //exclude walls
		y = rand() % (GRID_HEIGHT-2) + 1;	
	}
    pair<int,int> food = make_pair(x,y);
    addRect(food, food_color);
    return food;
}

bool moveSnake(Snake *snake, int dir, pair<int,int> &food){
    
    moveCoords(snake->front, dir);
    snake->dirs.push(dir);
    
    if(snake->grid[snake->front.second][snake->front.first] == FREE) {
        addRect(snake->front, snake_color); //Add to head
        snake->grid[snake->front.second][snake->front.first] = SNAKE;
        
        if(food == snake->front){
            snake->length++;
            food = addFood(snake); //seg faults here
        } else {
            addRect(snake->tail, background_color); //remove tail
            snake->grid[snake->tail.second][snake->tail.first] = FREE;
            moveCoords(snake->tail, snake->dirs.front());
            snake->dirs.pop();
        }

		al_flip_display();
        al_rest(0.1);
        return false;
    } else{
        return true; //game over
    }
}

vector< vector<int> > initGrid(){
    vector< vector<int> > grid(GRID_HEIGHT, vector<int>(GRID_WIDTH, 0)); 
    
    grid[0] = vector<int>(GRID_WIDTH, WALL);
    grid[GRID_HEIGHT-1] = vector<int>(GRID_WIDTH, WALL);

    for(int i=0; i < GRID_HEIGHT; i++){	
        grid[i][0] = WALL;
        grid[i][GRID_WIDTH-1] = WALL;
    }
    return grid;
}

Snake *createSnake(){
    Snake *snake = new Snake();
    snake->length = 3;
    vector< vector<int> > grid = initGrid(); 
    
    for(int i=2; i >= 0; i--){
        int x = (GRID_WIDTH/2) + i;
        int y = (GRID_HEIGHT/2);
        grid[y][x] = SNAKE;
        addRect(make_pair(x,y), snake_color);
    }
    queue<int> dirs;
    dirs.push(ALLEGRO_KEY_LEFT);
    dirs.push(ALLEGRO_KEY_LEFT);
    snake->dirs = dirs;

    snake->grid = grid;

    snake->front = make_pair(GRID_WIDTH/2, GRID_HEIGHT/2);
	snake->tail = make_pair((GRID_WIDTH/2)+2, GRID_HEIGHT/2);

    return snake;
}


int oppositeDir(int dir){
    switch(dir) {
        case ALLEGRO_KEY_LEFT:
            return ALLEGRO_KEY_RIGHT;
        case ALLEGRO_KEY_RIGHT:
            return ALLEGRO_KEY_LEFT;
        case ALLEGRO_KEY_UP:
            return ALLEGRO_KEY_DOWN;
        case ALLEGRO_KEY_DOWN:
			return ALLEGRO_KEY_UP;
	}
	return 0;
}

void displayText(const char *text, int time){
	ALLEGRO_FONT *font = al_load_ttf_font("Arial Bold.ttf",72,0);
	int x = (SCREEN_WIDTH - al_get_text_width(font, text))/2;
	int y = (SCREEN_HEIGHT - al_get_font_line_height(font))/2;

	al_draw_text(font, snake_color, x, y, 0, text);
	al_flip_display(); 
	al_rest(time);
	al_draw_filled_rectangle(2*SQUARE_WIDTH, 2*SQUARE_WIDTH, SCREEN_WIDTH-2*SQUARE_WIDTH, SCREEN_HEIGHT-2*SQUARE_WIDTH, background_color);
}

void countDown(){
	//ALLEGRO_FONT *font = al_load_ttf_font("Arial Bold.ttf",72,0);
	displayText("READY?", 2);
	displayText("3", 1);
	displayText("2", 1);
	displayText("1", 1);
}


int main(int argc, char *argv[]){
	
    al_init();

    al_install_keyboard();
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue,  al_get_keyboard_event_source());
   
    al_init_primitives_addon();
    al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 1, ALLEGRO_REQUIRE);

    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(background_color);
    al_draw_rectangle(1.9*SQUARE_WIDTH, 1.9*SQUARE_WIDTH, SCREEN_WIDTH-1.8*SQUARE_WIDTH, SCREEN_HEIGHT-1.8*SQUARE_WIDTH, wall_color, 2.0);
    
    al_rest(2);
	countDown();


    bool gameover = false; 
    int dir = ALLEGRO_KEY_LEFT; 
    
    Snake *snake = createSnake();
    pair<int,int> food = addFood(snake);

    al_flip_display(); 
	
    while(!gameover){ //escape key is quit as well

		ALLEGRO_EVENT ev;
		if(al_get_next_event(event_queue, &ev) && oppositeDir(dir) != ev.keyboard.keycode) {
			dir = ev.keyboard.keycode;
			al_flush_event_queue(event_queue);
		}

        gameover = moveSnake(snake, dir, food);
        if(gameover){
        	displayText("Game Over!", 5);
            //cout << "Game over!" << endl; 
        } else if(snake->length == (GRID_WIDTH-2)*(GRID_HEIGHT-2)){
            displayText("You Win!", 5);
            //cout << "You win!" << endl;
            gameover = true;
        }
	}

	al_destroy_display(display);
	al_shutdown_primitives_addon();
	al_shutdown_font_addon();
	//al_shutdown_ttf_addon(); //seg fault
	return 0;
}

