#include "pch.h"
#include<allegro5\allegro5.h> 
#include<allegro5\allegro_native_dialog.h> 
#include<allegro5\allegro_primitives.h>
#include "SnakeAllegro.h"
#include <iostream>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <vector>

#include "Snake.h"
#include "Food.h"
#include <time.h>
 

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>


// Politechnika Śląska 2018 Marek Świerkot - Makrokierunek
// Snake game written using C++ language and Allegro 5.0 library


using namespace std;

																				// Functions takes 3 parameter - vector is taken to check whether generated position is not within 
																				// snake body, also X and Y, sets the range, usually size of the game
																					
int * randomiseFoodPosition( vector<Snake> snakeBody, int X, int Y )		
{
	srand(time(NULL));

	int			position[2];													// stores randomised position of generated Food object 
	bool		spawnedWithinSnake;												// determines whether Food object was spawned within snake body - which shouldn't be possible								
	
	do
	{
		spawnedWithinSnake = false;
																				// randomised from 20 to X or Y - 20 - 20 pixels is just wall margin so apple
																				// is not inside the wall or uncomfortably close to it (half-sized for example)
		position[0] = rand() % (X-20) + 20;										 
		position[1] = rand() % (Y-20) + 20;

																				// 10 and 20, are just margin numbers so apple isn't spawned just exactly by the snake
		for (Snake element : snakeBody)
		{																		
			if (element.getX() - 10 <= position[0] && element.getX() + 20 >= position[0] && element.getY() - 10 <= position[1] && element.getY() + 20 >= position[1])
			{
				spawnedWithinSnake = true;
			}	
		}

	} while (spawnedWithinSnake != false); 

	return position;
}
 
																				// for each vector element called segment, there is a rectangle drawn at position of segment,
																				// with color changing from dark green to light forth and back, 
																				// color change is based on two varialbes colorMixer, changeColor 
void displaySnakeOnScreen( vector<Snake> snakeBody )
{
																				
	int			colorMixer = 0;
	int			changeColor = 5;

	for(Snake segment : snakeBody)
	{
	 

		al_draw_rectangle(segment.getX(), segment.getY(), segment.getX() + snakeSize, segment.getY() + snakeSize, al_map_rgb(0 , 30 + colorMixer, 0), snakeSize);
		
		colorMixer += changeColor;
		
		if (colorMixer % 50 == 0)
		{
			changeColor *= (-1);
		}
		 
	}
	al_flip_display();
}


																				  
bool detectFoodCollision(vector<Snake> snakeBody, Food food)						 
{
																				// again margin set so that apple is eaten when its covered not neccesarly entirely by the head

	if (snakeBody[0].getX() - (snakeSize) <= food.getX() && snakeBody[0].getX() + (snakeSize + 3) >= food.getX() ) 
	{
		if (snakeBody[0].getY() - (snakeSize ) <= food.getY()  && snakeBody[0].getY() + (snakeSize + 3) >= food.getY() )
		{
			if (food.getType() == 1)
			{
				result++;														// getType() - 1 for normal, 2 for special apple		
			}
			if (food.getType() == 2)
			{
				result += 2;
			}

			return true;
		}
	}

	return false;
}
void wallDissapear(vector <Snake> * snakeBody)									 
{
		if (snakeBody->at(0).getX() <= 0)
		{
			snakeBody->at(0).setX(DISPLAY_WIDTH - 10);
		}
		else if (snakeBody->at(0).getX() >= (DISPLAY_WIDTH - 10))
		{
			snakeBody->at(0).setX(10);
		}
		else if (snakeBody->at(0).getY() <= 0)
		{
			snakeBody->at(0).setY(DISPLAY_HEIGHT - 10);
		}
		else if (snakeBody->at(0).getY() >= (DISPLAY_HEIGHT - 10))
		{
			snakeBody->at(0).setY(10);
		}
}
 void detectCollision( vector<Snake> snakeBody)
{
	if (snakeBody[0].getX() <=15 || snakeBody[0].getX() >= (DISPLAY_WIDTH-25) || snakeBody[0].getY() <= 15|| snakeBody[0].getY() >= (DISPLAY_HEIGHT-25))  
	{
		GAME = FALSE;
		 
	}
	else  
	{
		for (int i = 3; i< snakeBody.size(); i++)
		{
			if (snakeBody[0].getX() == snakeBody[i].getX() && snakeBody[0].getY() == snakeBody[i].getY())
			{
				GAME = FALSE;
			}
		}
	}
}
 
   
 
int main()
{
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);

																				// initializing allegro, creating display, event queue, timer pointers,
																				// three font types based on one file, registering event
																				// bunch of allegro-setup related things
	al_init();
	 
	ALLEGRO_DISPLAY						*display;												
	ALLEGRO_EVENT_QUEUE					*event_queue;
	ALLEGRO_KEYBOARD_STATE				keyboardState;
	ALLEGRO_TIMER						*timer = al_create_timer(1.0 / FPS);
	ALLEGRO_EVENT						event;
	al_init_font_addon();
	al_init_ttf_addon();

	ALLEGRO_FONT *font = al_load_ttf_font("font.ttf", 17, 0);
	ALLEGRO_FONT *fontCounter = al_load_ttf_font("font.ttf", 130, 0);
	ALLEGRO_FONT *fontEnding = al_load_ttf_font("font.ttf", 60, 0);
 
	event_queue	= al_create_event_queue();
	display	= al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	
	al_init_primitives_addon();
	al_install_keyboard();

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_start_timer(timer);																	

 
																				// snakeBody is a vector for snake segments, one head, two types of food - objects
	vector<Snake>						 snakeBody;														 
	Snake								 head(250, 250);										 																														
	Food								 apple(250, 380, 1 );
	Food							     specialApple(-20, -20, 2);
	snakeBody.push_back(head);
	
 
 
	while (GAME)
	{
	
		al_wait_for_event(event_queue, &event);
		al_clear_to_color(al_map_rgb(219, 225, 219));
		
		al_draw_filled_circle(apple.getX(), apple.getY(), 5, al_map_rgb(235, 83, 0));
		al_draw_filled_circle(specialApple.getX(), specialApple.getY(), 5, al_map_rgb(10, 83, 0));
		
		if (result == 45 ) movementVariable = 5;																	// Speeds up a game, when certain score is reached
		if (result == 80 ) movementVariable = 6;											 
											 
	 

																													// wall color changes based on if wall colision is off or on
		if (!collisionOff)
		{
			Rcolor = 4;
			Gcolor = 54;
			Bcolor = 1;
			detectCollision(snakeBody);
		}
		if (collisionOff)
		{
			wallDissapear(&snakeBody);
			cooldown--;																								// cooldown is a variable which works as a timer - walls are
																													// disabled only for a certain amount of time
			Rcolor = 104;
			Gcolor = 154;
			Bcolor = 101;
		 
			
			if (cooldown < 300)
			{
				Rcolor = 74;
				Gcolor = 124;
				Bcolor = 71;
			}

			if (cooldown <= 0)
			{
				collisionOff = false;
			}
		}
		

																													// text displaying - scores, timer if special apple is used
		al_draw_text(font, al_map_rgb(0, 0, 0), 50, 10, ALLEGRO_ALIGN_CENTRE,  "SCORE:" );
		al_draw_textf(font, al_map_rgb(0, 0, 0), 100, 10, ALLEGRO_ALIGN_CENTRE, "%d" ,result );
		
		al_draw_text(font, al_map_rgb(0, 0, 0), 390, 460, ALLEGRO_ALIGN_CENTRE, " Magic apples:");
		al_draw_textf(font, al_map_rgb(0, 0, 0), 465, 460, ALLEGRO_ALIGN_CENTRE, "%d", specialApples);

		if (collisionOff == true)
		{
			 al_draw_textf(fontCounter, al_map_rgb(230,230,230), 250, 150, ALLEGRO_ALIGN_CENTRE, "%d", cooldown/10);
		}


																													// drawing walls
		al_draw_rectangle(0, 0, DISPLAY_WIDTH, 0, al_map_rgb(Rcolor, Gcolor, Bcolor), 20);
		al_draw_rectangle(0, 0, 0, DISPLAY_HEIGHT, al_map_rgb(Rcolor, Gcolor, Bcolor), 20);
		al_draw_rectangle(DISPLAY_WIDTH, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, al_map_rgb(Rcolor, Gcolor, Bcolor), 20);
		al_draw_rectangle(0, DISPLAY_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT, al_map_rgb(Rcolor, Gcolor, Bcolor), 20);

		
		specialAppleTimer--;
	 
																													// special apple is spawned only when player has 0 of them,
																													// when one is not currently used or already spawned
																							
		if (specialApples == 0 &&  specialAppleSpawned == false && collisionOff == false)							
		{
																													// position is randomised from a wider range - sometimes 
																													// it should happen that apple is beyond the game size, which
																													// is treated as "not spawned for a while" state
			int *position = randomiseFoodPosition(snakeBody, 600, 600);												
			specialApple.updatePosition((*position), *(position+1));
			specialAppleSpawned = true;
			
		}

																													// when spawned it reamins on the position for as long	
																													// specialTimer is not 0 - then it changes possition
																													// possibly for out of range position, causing it temporar 
																													// dissapearance
		if (specialAppleSpawned = true)																				
		{
			if (specialAppleTimer == 0)
			{
				specialAppleTimer = 300;
				specialAppleSpawned = false;
			}
		}

		if (detectFoodCollision(snakeBody, apple))
		{
			int *position;

			position = randomiseFoodPosition(snakeBody, DISPLAY_WIDTH-20, DISPLAY_HEIGHT-20);

			 
				apple.updatePosition((*position), *(position + 1));

				Snake eatenApple(*position, *(position + 1));

				snakeBody.push_back(eatenApple);																	// actually thre segments are pushed, so its becomes longer quicker
				snakeBody.push_back(eatenApple);
				snakeBody.push_back(eatenApple);
 
		}
		if (detectFoodCollision(snakeBody, specialApple))
		{
			specialApples++;
			specialAppleSpawned = false;
			specialApple.updatePosition(-20, -20);																	// if speciallApple is eaten, its position will be changed
																													// for -20, -20, and it will disapear
		}
 
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			GAME = false;
		}

		else if (event.type == ALLEGRO_EVENT_TIMER)
		{
			al_get_keyboard_state(&keyboardState);

			if (al_key_down(&keyboardState, ALLEGRO_KEY_RIGHT) && snakeDirection != LEFT)
			{
				snakeDirection = RIGHT;
			}
			else if (al_key_down(&keyboardState, ALLEGRO_KEY_DOWN) && snakeDirection != UP)
			{
				snakeDirection = DOWN;
			}
			else if (al_key_down(&keyboardState, ALLEGRO_KEY_UP) && snakeDirection != DOWN)
			{
				snakeDirection = UP;
			}
			else if (al_key_down(&keyboardState, ALLEGRO_KEY_LEFT) && snakeDirection != RIGHT)
			{
				snakeDirection = LEFT;
			}	
			else if (al_key_down(&keyboardState, ALLEGRO_KEY_SPACE) && specialApples > 0)
			{
				if (collisionOff == false)
				{
					specialApples--;
					cooldown = 900;
				}
				collisionOff = true;
				
				
			}

		}

		switch (snakeDirection)
		{
			case UP:
			{
				targetY = -movementVariable;
				targetX = 0;
				break;
			}
			case RIGHT:
			{
				targetY = 0;
				targetX = movementVariable;
				break;
			}
			case DOWN:
			{
				targetY = movementVariable;
				targetX = 0;
				break;
			}
			case LEFT:
			{
				targetY = 0;
				targetX = -movementVariable;
				break;
			}
		}
		

		int newX, newY;																								// idea for moving snake explained in PDF file

		for (int i = 0; i < snakeBody.size(); i++)
		{
			if (snakeBody.size() > 1)
			{
				if (i == 0)
				{
					newX = snakeBody[0].getX() + targetX;
					newY = snakeBody[0].getY() + targetY;

					Snake snake(newX, newY);

					snakeBody.insert(snakeBody.begin(), snake);
				}
				if (i == snakeBody.size() - 1)
				{
					snakeBody.pop_back();
				}
			}
			else																		
			{
				snakeBody[i].setX(snakeBody[i].getX() + targetX);
				snakeBody[i].setY(snakeBody[i].getY() + targetY);
			}

		}
		
		displaySnakeOnScreen(snakeBody);
		
	}
 
	for (int i =0; i < 10000; i++)
	{
		al_draw_textf(fontEnding, al_map_rgb(23, 23, 23), 250, 150, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
		al_draw_textf(font, al_map_rgb(23, 23, 23), 250, 230, ALLEGRO_ALIGN_CENTRE,   "Your result:");
		al_draw_textf(font, al_map_rgb(23, 23, 23), 320, 230, ALLEGRO_ALIGN_CENTRE, "%d",  result);
		al_flip_display();
	}
		
		 
	
	
																													// avoiding memory leak by deleting pointers
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
	al_destroy_font(font);
	al_destroy_font(fontCounter);
	al_destroy_font(fontEnding);

}

