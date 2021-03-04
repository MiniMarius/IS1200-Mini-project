/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):
   Marius Seffer 
   Christian Krokstedt Odewale

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>
#include <stdlib.h>

void user_isr();
void labinit(void);
void convert_array_to_display_format();
void set_byte_array(int xPos, int yPos, int width, int height);
void startup_animation();
void show_surprise();
void menu_select_mode();
void select_difficulty();
void draw_gamefield();
bool p1_upbutton_pressed();
bool p1_downbutton_pressed();
bool p2_upbutton_pressed();
bool p2_downbutton_pressed();
void track_racket_movements();
void clear_pixel_array();
void clear_display();
void ball_movement();
void ball_collisions();
void reset_game();
void player_2_wins();
void player_1_wins();
void dance_animation(int a, int b);
void labwork();

//Global Variables
uint8_t pixels[32][128];	//pixel array used for easy drawing
uint8_t real_display[512]; //512 bytes used in display_image

//playfield variable
int center_line = 128 / 2;
//Menu variables
int selected_mode = 0;
int in_game = 0;
int in_settings = 0;
int in_surprise = 0;
int in_menu = 0;
int selected_difficulty = 0;

//racket variables
float racket_1_posX = 0;
float racket1_posY = 32 / 2 - 7;
float racket_2posX = 126;
float racket_2_posY = 32 / 2 - 7;
float racket_speed = 1;

//ball variables
float ball_size = 3;
float ball_speed = 1;
float ball_posX = 128 / 2;
float ball_posY = 16;
int   ball_directionX = 1; //1 is left, 0 is right
int   ball_directionY = 1; //0 is up, 1 is center, 2 is down

void user_isr() {

}
// initializations
void labinit( void ) {
  //mask leds as output
	TRISE = TRISE & 0xff00;

  //initialize port D and make bits 11-5 as inputs.
	TRISD = TRISD & 0x0fe0;
}

void convert_array_to_display_format() {
	int page, column, row;
	uint8_t shift_left = 1;
	uint8_t display_number = 0;

	for (page = 0; page < 4; page++) {
		for (column = 0; column < 128; column++) {
			shift_left = 1;
			display_number = 0;
			for (row = 0; row < 8; row++) {
				if (pixels[8 * page + row][column]) {
					display_number = display_number | shift_left;
				}
				shift_left <<= 1;
			}
			real_display[column + page * 128] = display_number;
		}
	}
}

void set_byte_array(int xPos, int yPos, int width, int height) {
	int row, column;
	for (row = 0; row < 32; row++) { // y-axis
  	for (column = 0; column < 128; column++) { // x-axis
  		if (row >= yPos && row <= (yPos + height) && column >= xPos && column <= (xPos + width)) {
  			pixels[row][column] = 1;
  		}
  	}
  }
}

void startup_animation() {
	clear_display();
	int iterator = 1;
	while (1) {
		dance_animation(0, 96);
		display_string(0, "    PONG");
		display_string(1, "    LEGENDS!");
		display_string(3, "    Group 60");
		if (iterator == 1)
			display_update();
    //iterator = 0;
		int btns = getbtns();
		if (btns & 0x4 || btns & 0x2 || btns & 0x1) {
			in_menu = 1;
			break;
		}
	}
}

void show_surprise() {
	int iterator = 96;
	while (1) {
		PORTE = iterator;
		if (iterator > 5) {
			iterator--;
			quicksleep(200000);
			display_icon(iterator, icon, 32);
		}
		else {
			display_icon(0, pokemon, 128);
		}
		int btns = getbtns();
		if (btns & 0x4 || btns & 0x2 || btns & 0x1) {
			in_menu = 1;
			in_surprise = 0;
			PORTE = 0;
			break;
		}
	}
}
//Displays menu screen and selection of these with button
void menu_select_mode() {
	int btns = getbtns();
	if(btns & 0x4) {
		selected_mode++;
		if (selected_mode > 2) {
			selected_mode = 0;
		}
		quicksleep(800000);
	}
	if (selected_mode == 0) {
		display_string(0, "   PONG LEGENDS");
		display_string(1, "-> Start Game");
		display_string(2, "   Settings");
		display_string(3, "   Surprise");
	}
	else if (selected_mode == 1) {
		display_string(0, "   PONG LEGENDS");
		display_string(1, "   Start Game");
		display_string(2, "-> Settings");
		display_string(3, "   Surprise");
	}

	else if (selected_mode == 2) {
		display_string(0, "   PONG LEGENDS");
		display_string(1, "   Start Game");
		display_string(2, "   Settings");
		display_string(3, "-> Surprise");
	}
	display_update();

	if (selected_mode == 0 && btns & 0x1) {
		quicksleep(800000);
		in_game = 1;
		in_menu = 0;
	}
	if (selected_mode == 1 && btns & 0x1) {
		quicksleep(800000);
		in_settings = 1;
		in_menu = 0;
	}
	if (selected_mode == 2 && btns & 0x1) {
		quicksleep(800000);
		in_surprise = 1;
		in_menu = 0;
	}
}

void select_difficulty() {
	int btns = getbtns();
	if(btns & 0x4) {
		selected_difficulty++;
		if (selected_difficulty > 2) {
			selected_difficulty = 0;
		}
		quicksleep(800000);
	}
	if (selected_difficulty == 0) {
		display_string(0, "   DIFFICULTY");
		display_string(1, "-> Noob");
		display_string(2, "   Normal");
		display_string(3, "   Insane");
	}
	else if (selected_difficulty == 1) {
		display_string(0, "   DIFFICULTY");
		display_string(1, "   Noob");
		display_string(2, "-> Normal");
		display_string(3, "   Insane");
	}

	else if (selected_difficulty == 2) {
		display_string(0, "   DIFFICULTY");
		display_string(1, "   Noob");
		display_string(2, "   Normal");
		display_string(3, "-> Insane");
	}
	display_update();

	if (selected_difficulty == 0 && btns & 0x1) {
		quicksleep(800000);
		ball_speed = 1;
		in_menu = 1;
		in_settings = 0;
	}
	if (selected_difficulty == 1 && btns & 0x1) {
		quicksleep(800000);
		ball_speed = 1.5;
		in_menu = 1;
		in_settings = 0;
	}
	if (selected_difficulty == 2 && btns & 0x1) {
		quicksleep(800000);
		ball_speed = 2;
		in_menu = 1;
		in_settings = 0;
	}
}

void draw_gamefield() {
  //draw ball
	set_byte_array(ball_posX, ball_posY, ball_size, ball_size);
	
  //draw center_line
	set_byte_array(center_line, 0, 0, 31);
	
  //draw rackets
	set_byte_array(racket_1_posX, racket1_posY, 1, 8);
	set_byte_array(racket_2posX, racket_2_posY, 1, 8);
	
  //display everything
	convert_array_to_display_format
();
	display_image(0, real_display);
}

bool p1_upbutton_pressed() {
	int btns = getbtns();
	if (btns & 0x4) {
		return 1;
	}
	return 0;
}
bool p1_downbutton_pressed() {
	int btns = getbtns();
	if (btns & 0x2) {
		return 1;
	}
	return 0;
}

bool p2_upbutton_pressed() {
	int sw = getsw();
	if (sw & 0x1) {
		return 1;
	}
	return 0;
}
bool p2_downbutton_pressed() {
	int sw = getsw();
	if (sw & 0x2) {
		return 1;
	}
	return 0;
}

void track_racket_movements() {
  //player 1 movements
	if (p1_upbutton_pressed() && racket1_posY != 0) {
		racket1_posY -=1;
		clear_pixel_array();
	}
	if (p1_downbutton_pressed() && racket1_posY != 23){
		racket1_posY +=1;
		clear_pixel_array();
	}

  //player 2 movements
	if (p2_upbutton_pressed() && racket_2_posY != 0) {
		racket_2_posY -=1;
		clear_pixel_array();
	}
	if (p2_downbutton_pressed() && racket_2_posY != 23){
		racket_2_posY +=1;
		clear_pixel_array();
	}
}
void clear_pixel_array() {
	int row, column;
  	for (row = 0; row < 32; row++) { // y-axis
    	for (column = 0; column < 128; column++) { // x-axis
    		pixels[row][column] = 0;
    	}
    }
}
void clear_display() {
	display_string(0, "         ");
	display_string(1, "         ");
	display_string(2, "         ");
	display_string(3, "         ");
}

void ball_movement() {

  //ball movement going straight left
	if (ball_directionX == 1 && ball_directionY == 1) {
		ball_posX -= ball_speed;
	}
  //ball movement going up and left
	if(ball_directionX == 1 && ball_directionY == 0) {
		ball_posX -= ball_speed;
		ball_posY -= ball_speed / 2;
	}
  //ball movement going down and left
	if (ball_directionX == 1 && ball_directionY == 2) {
		ball_posX -= ball_speed;
		ball_posY += ball_speed / 2;
	}
  //ball movement going straight right
	if (ball_directionX == 0 && ball_directionY == 1) {
		ball_posX += ball_speed;
	}
  //ball movement going up and right
	if (ball_directionX == 0 && ball_directionY == 0) {
		ball_posX += ball_speed;
		ball_posY -= ball_speed / 2;
	}
  //ball movement going down and right
	if (ball_directionX == 0 && ball_directionY == 2) {
		ball_posX += ball_speed;
		ball_posY += ball_speed / 2;
	}
	clear_pixel_array();
}

void ball_collisions() {
  //change y direction on roof impact
	if (ball_posY <= 0) {
		ball_directionY = 2;
	}
  //change y direction on bottom impact
	if (ball_posY >= 28) {
		ball_directionY = 0;
	}
  //check for win if ball going past a player
	if (ball_posX < (racket_1_posX - 5)) {
		player_2_wins();
	}
	if (ball_posX > (racket_2posX + 5)) {
		player_1_wins();
	}
  //racket collisions, changing ball movement directions above
  //racket1 collisions
  //hitting center part of racket
	if (ball_posX <= racket_1_posX && ball_posY <= racket1_posY +5 && ball_posY >= racket1_posY + 2) {
		ball_directionX = 0;
		ball_directionY = 1;
		ball_speed += 0.05;
	}
  //hitting upper part of racket
	if (ball_posX <= racket_1_posX && ball_posY <= racket1_posY +2 && ball_posY >= racket1_posY) {
		ball_directionX = 0;
		ball_directionY = 0;
		ball_speed += 0.05;

	}
  //hitting lower part of racket
	if (ball_posX <= racket_1_posX && ball_posY <= racket1_posY +7 && ball_posY >= racket1_posY +5) {
		ball_directionX = 0;
		ball_directionY = 2;
		ball_speed += 0.05;
	}

  //racket 2 collisions
  //hitting center part of racket
	if (ball_posX >= racket_2posX && ball_posY <= racket_2_posY +5 && ball_posY >= racket_2_posY + 2) {
		ball_directionX = 1;
		ball_directionY = 1;
		ball_speed += 0.05;
	}
  //hitting upper part of racket
	if (ball_posX >= racket_2posX && ball_posY <= racket_2_posY +2 && ball_posY >= racket_2_posY) {
		ball_directionX = 1;
		ball_directionY = 0;
		ball_speed += 0.05;
	}
  //hitting lower part of racket
	if (ball_posX >= racket_2posX && ball_posY <= racket_2_posY +7 && ball_posY >= racket_2_posY +5) {
		ball_directionX = 1;
		ball_directionY = 2;
		ball_speed += 0.05;
	}

}
void reset_game() { //reset all variables ie game
	selected_mode = 0;
	in_settings = 0;
	in_surprise = 0;
	in_game = 0;
	in_menu = 1;
	racket_1_posX = 0;
	racket1_posY = 32 / 2 - 7;
	racket_2posX = 126;
	racket_2_posY = 32 / 2 - 7;
	racket_speed = 1;
	ball_size = 3;
	ball_speed = 1;
	ball_posX = 128 / 2;
	ball_posY = 16;
	ball_directionX = 1;
	ball_directionX = 1;
}
//displays winner screen with animations
void player_2_wins() {
	clear_display();
	reset_game();
	int first_iteration = 1;
	while (1) {
		display_string(0, "    PLAYER 2");
		display_string(1, "    WINS!");
		if (first_iteration == 1) {
			display_update();
			first_iteration = 0;
		}
		dance_animation(0, 96);

		int btns = getbtns();
		if (btns & 0x4 || btns & 0x2 || btns & 0x1) {
			in_menu = 1;
			break;
		}
	}
}
//displays winner screen with animations
void player_1_wins() {
	reset_game();
	clear_display();
	int first_iteration = 1;
	while (1) {
		display_string(0, "    PLAYER 1");
		display_string(1, "    WINS!");
		if (first_iteration == 1) {
			display_update();
			first_iteration = 0;
		}
		dance_animation(0, 96);
		int btns = getbtns();
		if (btns & 0x4 || btns & 0x2 || btns & 0x1) {
			in_menu = 1;
			break;
		}
	}
}

void dance_animation(int a, int b ) { //two dances at the same time
	display_icon(a, dance0, 32);
	display_icon(b, dance0, 32);
	quicksleep(600000);
	display_icon(a, dance1, 32);
	display_icon(b, dance1, 32);
	quicksleep(600000);
	display_icon(a, dance2, 32);
	display_icon(b, dance2, 32);
	quicksleep(600000);
	display_icon(a, dance3, 32);
	display_icon(b, dance3, 32);
	quicksleep(600000);
	display_icon(a, dance4, 32);
	display_icon(b, dance4, 32);
	quicksleep(600000);
	display_icon(a, dance5, 32);
	display_icon(b, dance5, 32);
	quicksleep(600000);
	display_icon(a, dance6, 32);
	display_icon(b, dance6, 32);
	quicksleep(600000);
	display_icon(a, dance7, 32);
	display_icon(b, dance7, 32);
	quicksleep(600000);
	display_icon(a, dance8, 32);
	display_icon(b, dance8, 32);
	quicksleep(600000);
	display_icon(a, dance9, 32);
	display_icon(b, dance9, 32);
	quicksleep(600000);
	display_icon(a, dance10, 32);
	display_icon(b, dance10, 32);
	quicksleep(600000);
}

/* This function is called repetitively from the main program */
void labwork( void ) {
	if (in_menu == 1) {
		menu_select_mode();
	}
	if (in_game == 1) {
		draw_gamefield();
		ball_movement();
		ball_collisions();
		track_racket_movements();
	}
	if (in_settings == 1) {
		select_difficulty();
	}
	if (in_surprise == 1) {
		show_surprise();
	}
}

