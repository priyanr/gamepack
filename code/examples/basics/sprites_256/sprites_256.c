#include <msp430.h>
#include <stdint.h>
#include "../../../GP.h"

#include "header.h"
#include "background.h"

#define NUM_SPRITES     62

struct
{
    uint16_t x;
    uint16_t y;
    int8_t vx;
    int8_t vy;
    uint8_t sprite;
} sprites[NUM_SPRITES];

int main(void)
{
    uint8_t i;

    // initialize the gamepack
    GP_begin();

    // set background color to gray
    GP_wr16(BG_COLOR, RGB(25, 25, 25));

    // set the first background palette to transparent so we see the gray
    GP_wr16(RAM_PAL, TRANSPARENT);

    // load the sprite palette and images
    GP_copy(PALETTE16A, sprite_pal, sizeof(sprite_pal));
    GP_copy(RAM_SPRIMG, sprite_img, sizeof(sprite_img));

    GP_copy(RAM_PIC, sprites256_pic, sizeof(sprites256_pic));
    GP_copy(RAM_CHR, sprites256_chr, sizeof(sprites256_chr));
    GP_copy(RAM_PAL, sprites256_pal, sizeof(sprites256_pal));


    for (i = 0; i < NUM_SPRITES; i++) sprites[i].x = random(0, 400-16);
    for (i = 0; i < NUM_SPRITES; i++) sprites[i].y = random(0, 300-16);
    for (i = 0; i < NUM_SPRITES; i++) sprites[i].vx = random(0,6)-3;
    for (i = 0; i < NUM_SPRITES; i++) sprites[i].vy = random(0,6)-3;
    for (i = 0; i < NUM_SPRITES; i++) sprites[i].sprite = random(0, SPRITE_FRAMES);

    while (1)
    {

        GP_waitvblank();

        __wstartspr(0);

        // update all sprites
        for (i = 0; i < NUM_SPRITES; i++)
        {
            if (sprites[i].x + sprites[i].vx + 16 > 400) sprites[i].vx *= -1;
            if (sprites[i].y + sprites[i].vy + 16 > 300) sprites[i].vy *= -1;
            if (sprites[i].x + sprites[i].vx < 0) sprites[i].vx *= -1;
            if (sprites[i].y + sprites[i].vy < 0) sprites[i].vy *= -1;

            sprites[i].x += sprites[i].vx;
            sprites[i].y += sprites[i].vy;

            // draw 4 to get close to 256 since we don't have enough ram to actually store them all
            draw_sprite(sprites[i].x,
                        sprites[i].y,
                        sprites[i].sprite,
                        0, 0);
            draw_sprite(400 - sprites[i].x - 16,
                        sprites[i].y,
                        sprites[i].sprite,
                        0, 0);
            draw_sprite(sprites[i].x,
                        300 - sprites[i].y - 16,
                        sprites[i].sprite,
                        0, 0);
            draw_sprite(400 - sprites[i].x - 16,
                        300 - sprites[i].y - 16,
                        sprites[i].sprite,
                        0, 0);

        }

        while (gp_cur_spr < 255) { GP_xhide(); }

        __end();
    }

    return 0;
}
