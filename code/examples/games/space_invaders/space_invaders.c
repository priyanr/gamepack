#include <msp430.h>
#include <stdint.h>
#include "GP.h"

#include "header.h"

#define BULLET_SPRITE   8
#define BULLET_SPEED    3
#define NUMBER_OF_BULLETS	25

#define BOULDER_Y		32*7

#define ALIEN_A         0
#define ALIEN_B         1
#define ALIEN_C         2

#define SHIP_Y        32*8+16 // pixel location of top of ship
#define SHIP_SPEED    5       // pixels per gameloop


// x y values are top left of alien
void draw_alien(uint8_t alien, uint16_t x, uint16_t y, uint8_t anim)
{
    switch (alien)
    {
        case ALIEN_A:
            draw_sprite(x+8, y, 0 + (anim * 1) , 0 , 0);
            break;
        case ALIEN_B:
            draw_sprite(x   , y, 3 + (anim * 3) , 0 , 0);
            draw_sprite(x+16, y, 4 + (anim * 3) , 0 , 0);
            break;
        case ALIEN_C:
            draw_sprite(x   , y, 9 + (anim * 3)  , 0 , 0);
            draw_sprite(x+16, y, 10 + (anim * 3) , 0 , 0);
            break;
    }
}

// x y values are top left of brick
void draw_brick(uint16_t x, uint16_t y, uint8_t anim)
{
    uint8_t base = 15 + 6*anim;

    draw_sprite(x    , y    , base+0 , 0 , 0);
    draw_sprite(x+16 , y    , base+1 , 0 , 0);
    draw_sprite(x+32 , y    , base+2 , 0 , 0);

    draw_sprite(x    , y+16 , base+3 , 0 , 0);
    draw_sprite(x+16 , y+16 , base+4 , 0 , 0);
    draw_sprite(x+32 , y+16 , base+5 , 0 , 0);
}

// x value is center of the ship
void draw_ship(uint16_t x, uint16_t y)
{
    draw_sprite(x-16, y, 2, 0, 0);
    draw_sprite(x, y, 5, 0, 0);
}

// x,y is the center of explosion
void draw_explosion(uint16_t x, uint16_t y)
{
	draw_sprite(x-16, y, 14, 0, 0);
	draw_sprite(x, y, 11, 0, 0);
}


int main(void)
{
	uint16_t ship_x = 26;
	uint8_t paused = 1;
	//uint16_t bullet_x;
	//uint16_t bullet_y;

	typedef struct {
		int bullet_x;
		int bullet_y;
		int draw;
	} bullet;
	bullet bullet_array[NUMBER_OF_BULLETS] = {0,0,0};

	typedef struct {
		int alien_x;
		int alien_y;
		int killed;
	}aliens;
	aliens grid[5][9] = {0,0,0};

	unsigned int col;
	unsigned int row;
	unsigned int toRight = 1;
	unsigned int mvt = 0;
	unsigned int down = 0;
	unsigned int speed = 0;
	unsigned int bulletnum = 0;
	unsigned int pressA = 0;
	unsigned int bulletstart = 0;
	unsigned int bulletend = 0;
	unsigned int win = 0;
	unsigned int end = 0;
	unsigned int k;


    // initialize the gamepack
    GP_begin();

    // setup ascii
    GP_ascii();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE4A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    // hide the rest
    while (gp_cur_spr < 255) { GP_xhide(); }

    __end();

    while (1) {

        GP_waitvblank();

        GP_nes_read();

	// ********************************************************************
	//                         DRAWING
	// ********************************************************************

	// reset the sprite counter to 0
	__wstartspr(0);

	// ********************************************************************
	//                      GRID OF ALIENS
	// ********************************************************************

	if (paused == 0)
		{

		for (col = 0; col<9; col++)			// top row
		{
			if (grid[0][col].killed == 0) //if it has not been hit
			{
				grid[0][col].alien_x = col*32 + mvt;
				grid[0][col].alien_y = 32*0 + down*16;
				draw_alien(ALIEN_A, grid[0][col].alien_x, grid[0][col].alien_y, 0);
			}
		}
		for (col = 0; col<9; col++)
		{
			if (grid[1][col].killed == 0)
			{
				grid[1][col].alien_x = col*32 + mvt;
				grid[1][col].alien_y = 32*1 + down*16;
				draw_alien(ALIEN_B, grid[1][col].alien_x, grid[1][col].alien_y, 0);
			}
		}
		for (col = 0; col<9; col++)
		{
			if (grid[2][col].killed == 0)
			{
				grid[2][col].alien_x = col*32 + mvt;
				grid[2][col].alien_y = 32*2 + down*16;
				draw_alien(ALIEN_C, grid[2][col].alien_x, grid[2][col].alien_y, 0);
			}
		}
		for (col = 0; col<9; col++)
		{
			if (grid[3][col].killed == 0)
			{
				grid[3][col].alien_x = col*32 + mvt;
				grid[3][col].alien_y = 32*3 + down*16;
				draw_alien(ALIEN_B, grid[3][col].alien_x, grid[3][col].alien_y, 1);
			}
		}
		for (col = 0; col<9; col++)
		{
			if (grid[4][col].killed == 0)
			{
				grid[4][col].alien_x = col*32 + mvt;
				grid[4][col].alien_y = 32*4 + down*16;
				draw_alien(ALIEN_C, grid[4][col].alien_x, grid[4][col].alien_y, 1);
			}
		}

	speed++;

	if (toRight == 1 && speed%2 == 0)
	{
		mvt++;
		if (mvt == 110)
		{
			mvt--;
			toRight = 0;
			//down++;
		}
	}

	if (toRight == 0 && speed%2 == 0)
	{
		mvt--;
		if (mvt == 0)
		{
			mvt++;
			toRight = 1;
			down++;
		}
	}

	}



	// ********************************************************************
	//                      BOULDERS AND SHIP
	// ********************************************************************

	unsigned int i;
	for (i = 0; i < 4; i++)
	{
		draw_brick(26+96*i, BOULDER_Y, 0);
	}

	draw_ship(ship_x, SHIP_Y);




	// ********************************************************************
	//                         BULLETS
	// ********************************************************************
	//draw all bullets and move them up

			for (k = 0; k < NUMBER_OF_BULLETS; k++)
			{
				if (bullet_array[k].draw == 0) //draw
				{
					draw_sprite(bullet_array[k].bullet_x, bullet_array[k].bullet_y,8,0,0);
					bullet_array[k].bullet_y -= BULLET_SPEED;
					if (bullet_array[k].bullet_y < 2) //maybe 0
					{
						bullet_array[k].draw = 1; //don't draw anymore
					}

					//if hit alien, remove both from drawing:
					//for each column. case statement using bullet_x
					for (col = 0; col<9; col++)
					{
						//for each row
						for (row = 0; row < 5; row++)
						{
							//if bullet is within x bounds
							if (bullet_array[k].bullet_x > grid[row][col].alien_x && bullet_array[k].bullet_x < grid[row][col].alien_x + 14)
							{
								//if it is also at the y coordinate
								if (bullet_array[k].bullet_y <= grid[row][col].alien_y)
								{
									//remove the alien
									if (grid[row][col].killed == 0 && bullet_array[k].draw == 0) //if still alive and bullet exists... kill!
									{
										grid[row][col].killed = 1;
										bullet_array[k].draw = 1; //don't draw anymore
									}
								}
							}
						}
					}
				}
			}



	//if a bullet hits a boulder, it dies a little.


	// hide the rest
	while (gp_cur_spr < 255) { GP_xhide(); }

	__end();

    if (paused)
    {
        if (GP_player_1(NES_START))
            paused = 0;

        continue;
    }

    // ********************************************************************
	//                           SHIP
	// ********************************************************************

	if (GP_player_1(NES_LEFT)) ship_x -= SHIP_SPEED;
	if (GP_player_1(NES_RIGHT)) ship_x += SHIP_SPEED;

	if (ship_x >= 380) ship_x = 380;
	if (ship_x <= 16) ship_x = 16;


    // ********************************************************************
	//                          BULLETS
	// ********************************************************************

	if (GP_player_1(NES_A))
	{
		pressA = 1;
	}

	if (pressA == 1 && !(GP_player_1(NES_A))) // the player released the button
	{
		pressA = 0;
		for (bulletnum = 0; bulletnum < NUMBER_OF_BULLETS; bulletnum++)
		{
			if (bullet_array[bulletnum].draw == 1) //not being drawn
					{
						bullet_array[bulletnum].bullet_x = ship_x;
						bullet_array[bulletnum].bullet_y = SHIP_Y;
						bullet_array[bulletnum].draw = 0;
						bulletnum++;
						bulletend++;
						break;
					}
		}


	}

    // ********************************************************************
	//                          GAME OVER
	// ********************************************************************

	for (col = 0; col<9; col++)
	{
		for (row = 0; row < 5; row++)
		{
			if (grid[row][col].alien_y + 16 == BOULDER_Y)
			{
				paused = 1;
				GP_putstr(20, 7, "GAME OVER");
				end = 1;
			}
		}
	}

    // ********************************************************************
	//                          VICTORY
	// ********************************************************************
	win = 0;
	for (col = 0; col<9; col++)
	{
		for (row = 0; row < 5; row++)
		{
			win = win + grid[row][col].killed;
		}
	}
	if (win == 45)
	{
		paused = 1;
		GP_putstr(20, 7, "YOU WIN!");
		end = 1;
	}

	if (end == 1)
	{
		for (row = 0; row < 5; row ++)
		{
			for (col = 0; col < 9; col++)
			{
				grid[row][col].alien_x = 0;
				grid[row][col].alien_y = 0;
				grid[row][col].killed = 0;
			}
		}
		for (k = 0; k < NUMBER_OF_BULLETS; k++)
		{
			bullet_array[k].bullet_x = 0;
			bullet_array[k].bullet_y = 0;
			bullet_array[k].draw = 0;
		}


	}

    }

    return 0;
}
