/* 
 * Thanks to reference code provided by Bill Buckels:
 * http://www.appleoldies.ca/azgraphics33/rod33.htm
 */
#include <stdlib.h>     // srand, rand
#include <stdio.h>      // printf
#include <string.h>
#include <conio.h>      // kbhit, cgetc
#include <peekpoke.h>   // POKE
#include "a2conway.h"   // prototypes

// globals:
/* the base addresses for the 48 scanline pairs */
/* though we only use the first 20 pairs */
/* for lores graphics mode 40 x 48 x 16 colors */
/* we are using 40 x 40 x 16 */
uint16_t page1[24]={
    0x0400, // triad 0
    0x0480,
    0x0500,
    0x0580,
    0x0600,
    0x0680,
    0x0700,
    0x0780,
    0x0428, // triad 1
    0x04A8,
    0x0528,
    0x05A8,
    0x0628,
    0x06A8,
    0x0728,
    0x07A8,
    0x0450, // triad 2
    0x04D0,
    0x0550,
    0x05D0,
    0x0650,
    0x06D0,
    0x0750,
    0x07D0 };

uint16_t page2[24]={
    0x0800, // triad 0
    0x0880,
    0x0900,
    0x0980,
    0x0A00,
    0x0A80,
    0x0B00,
    0x0B80,
    0x0828, // triad 1
    0x08A8,
    0x0928,
    0x09A8,
    0x0A28,
    0x0AA8,
    0x0B28,
    0x0BA8,
    0x0850, // triad 2
    0x08D0,
    0x0950,
    0x09D0,
    0x0A50,
    0x0AD0,
    0x0B50,
    0x0BD0 };

void wait_for_keypress(uint8_t key)
{
    uint8_t c;
    CLEARKEYBUF;
    for (;;) {
        if (kbhit() > 0) {
            c = cgetc();
            CLEARKEYBUF;
            if (c == key) {
                return;
            }
        }
    }
}

void text_mode(void)
{
    softsw(SS_TEXTON);
    softsw(SS_MIXEDOFF);
    softsw(SS_PAGE2OFF);
}

void gr_mode(uint16_t page, uint16_t mode)
{
    softsw(SS_TEXTOFF);
    softsw(mode);
    softsw(page);
    softsw(SS_HIRESOFF);
}

void lo_clear(uint16_t baseaddr[], uint8_t color)
{
    int8_t idx;
    // copy the color value into both the high and lo nibble
    if (color > 0) {
        color &= 0x0F;
        color = (color << 4) | color;
    }
    // write this value into every memory address:
    for (idx = 0; idx < LORES_ROWS; idx++) {
        memset ((uint8_t *)baseaddr[idx], color, LORES_COLS);
    }
}

void lo_plot(uint16_t baseaddr[], uint8_t row, uint8_t col, uint8_t color)
{
    uint8_t pairrow = row / 2;
    uint8_t *rowptr = baseaddr[pairrow];
    if (row & 0x1) {
        color <<= 4;
        rowptr[col] = (rowptr[col] & 0x0F ) | color;
    }
    else {
        color &= 0xF;
        rowptr[col] = (rowptr[col] & 0xF0 ) | color;
    }
}

void randomize(uint16_t baseaddr[], uint16_t count)
{
    uint16_t r;
    uint8_t  row, col;
    // The Apple zero page has some semi-random garbage
    // suitable for seeding our PRNG
    uint16_t seed = PEEK(RSEED1) + PEEK(RSEED2) * 256;
    srand(seed);
    lo_clear(page1, TGI_COLOR_BLACK);

    while (count--) {
        r = rand();
        // use the high bit to determine which nibble we turn on
        row = r & ROWRANDMASK;
        col = (r & COLRANDMASK) >> 8;
        row %= MAXROW;
        col %= 40;
        //printf("turning on %d,%d with %04x\n", row, col, r);
        lo_plot(baseaddr, row, col, 0xf);
    }
}

int main(void)
{
    uint8_t c;

    // our program just uses the bottom 4 lines of the display
    printf("built at %s %s\n", __DATE__, __TIME__);
    printf("\nHotkeys:\n");
    printf("\tr: randomize\n");
    printf("\tg: gosper glider gun\n");
    printf("\ts: simkins glider gun\n");
    printf("\tp: pause\n");
    printf("\tq: quit\n");
    printf("press enter to start\n");
    wait_for_keypress(CH_ENTER);

    //asm("JSR $FC58"); // APPLESOFT HOME: WHY DOESNT THIS WORK?

    gotoxy(0,LORES_ROWS);
    POKE(TEXTWINDOW_TOP_EDGE,LORES_ROWS);

    gr_mode(SS_PAGE2OFF, SS_MIXEDON);
    lo_clear(page1, TGI_COLOR_BLACK);
    
    glider(page1);

    while (1) {
        if (kbhit() > 0) {
            c=cgetc();
            CLEARKEYBUF;

            if (c == 'p') {
                printf("PAUSED. Press Enter to Continue.\n");
                wait_for_keypress(CH_ENTER);
                continue;
            }
            else if (c == 'q') 
                break;
            else if (c == 'r') 
                randomize(page1, 400);
            else if (c == 'g')
                gospergun(page1);
            else if (c == 's')
                simkins(page1);
        }
        naive_analyze(page1, page2);
        softsw(SS_PAGE2ON);
        naive_analyze(page2, page1);
        softsw(SS_PAGE2OFF);
    }
    
    printf ("all done! press enter to end\n");
    wait_for_keypress(CH_ENTER);
    POKE(TEXTWINDOW_TOP_EDGE,0);
    softsw(SS_TEXTON);
    
    return EXIT_SUCCESS;
}
