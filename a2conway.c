/* 
 * Thanks to reference code provided by Bill Buckels:
 * http://www.appleoldies.ca/azgraphics33/rod33.htm
 */
#include <stdlib.h>     // srand, rand
#include <stdio.h>      // printf
#include <string.h>
#include <conio.h>      // kbhit, cgetc
#include <peekpoke.h>   // POKE
#include <apple2enh.h>  // CH_ENTER
#include "a2conway.h"   // prototypes

// globals:
/* the base addresses for the 48 scanline pairs */
/* though we only use the first 20 pairs */
/* for lores graphics mode 40 x 48 x 16 colors */
/* we are using 40 x 40 x 16 */
//uint8_t stats[MAXROW][MAXCOL];
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

void clearkeybuf(void)
{
    /* return the last key press  */
    uint8_t *kp = (uint8_t*)KEYPRESS_BUF_ADDR;
    /* clear the last key press   */
    uint8_t *kc = (uint8_t*)KEYCLEAR_BUF_ADDR;
    uint8_t c;

    /* clear stragglers from the keyboard buffer */
    while((c = kp[0]) > 127) {
        printf ("wiping extra keystroke %d from buf\n", c);
        kc[0]=0;
    }
}

void wait_for_keypress(uint8_t key)
{
    uint8_t c;
    clearkeybuf();
    for (;;) {
        if (kbhit() > 0) {
            c=cgetc();
            //printf ("key hit: %d\n", c);
            clearkeybuf();
            if (c==key) {
                //printf ("done waiting for %u\n", key);
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

#define LORES_COLS              40
#define LORES_ROWS              20

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
        //printf ("writing %d bytes (%x) into addr %p\n", 
        //          LORES_COLS, color, baseaddr[idx]);
    }
}

int8_t keypress(void)
{
    uint8_t *kp = (uint8_t *)KEYPRESS_BUF_ADDR;
    int8_t  c = kp[0];

    // read the keyboard buffer
    // and return 0 if no character is waiting
    if(c < 128) {
        return 0;
    }
    return c;
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

#define ROWABOVE(x) ( x == 0 ? (MAXROW -1)  : (x - 1))
#define ROWBELOW(x) ( x == (MAXROW - 1) ? 0 : (x + 1))
#define COLLEFT(y)  ( y == 0 ? (MAXCOL -1)  : (y - 1))
#define COLRIGHT(y) ( y == (MAXCOL - 1) ? 0 : (y + 1))

uint8_t peek_pixel(uint16_t baseaddr[], uint8_t row, uint8_t col) 
{
    uint8_t val;
    uint8_t *rowptr = baseaddr[ row / 2 ];

    if (row & 0x1)
        val = rowptr[col] & 0xF0;
    else
        val = rowptr[col] & 0x0F;

    if (val)
        return 1;
    return 0;
}

uint8_t count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, ROWABOVE(row), col)) {
        count++;
        //printf("\t%d,%d has a neighbor above\n", row, col);
    }
    if (peek_pixel(baseaddr, ROWBELOW(row), col)) {
        count++;
        //printf("\t%d,%d has a neighbor below\n", row, col);
    }
    if (peek_pixel(baseaddr, row, COLLEFT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor to the left\n", row, col);
    }
    if (peek_pixel(baseaddr, row, COLRIGHT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor to the right\n", row, col);
    }
    if (peek_pixel(baseaddr, ROWABOVE(row), COLLEFT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor above left\n", row, col);
    }
    if (peek_pixel(baseaddr, ROWBELOW(row), COLLEFT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor below left\n", row, col);
    }
    if (peek_pixel(baseaddr, ROWABOVE(row), COLRIGHT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor above right\n", row, col);
    }
    if (peek_pixel(baseaddr, ROWBELOW(row), COLRIGHT(col))) {
        count++;
        //printf("\t%d,%d has a neighbor below right\n", row, col);
    }
    return count;
}

uint16_t analyze(uint16_t src[], uint16_t dst[])
{
    uint8_t row, col, n, x;
    uint16_t total=0;

    for (row=0; row < MAXROW; row++) {
        for (col=0; col < MAXCOL; col++) {
            n = count_neighbors(src, row, col);
            //stats[row][col]=n;
            if (x=peek_pixel(src, row, col)) {
                if ((n == 2) || (n == 3)) {
                    lo_plot(dst, row, col, 0xF);
                    //printf ("%d,%d stays alive (res: %x, %d neighbors)\n", row, col, x, n);
                    //wait_for_keypress(CH_ENTER);
                    total++;
                }
                else
                    lo_plot(dst, row, col, 0x0);
            }
            else {
                if (n == 3) {
                    lo_plot(dst, row, col, 0xF);
                    //printf ("%d,%d stays alive (res: %x, %d neighbors)\n", row, col, x, n);
                    //wait_for_keypress(CH_ENTER);
                    total++;
                }
                else
                    lo_plot(dst, row, col, 0x0);
            }
        }
    }
    return total;
}

void run(void) 
{
    //uint8_t row,col,i;
    uint16_t total;
    while (1) {
        analyze(page1, page2);
        softsw(SS_PAGE2ON);
        total = analyze(page2, page1);
        if (total == 0) {
            //randomize(page1, 400);
            //gospergun(page1);
            //glider(page1);
            simkins(page1);
        }
        softsw(SS_PAGE2OFF);
    }
}

int main(void)
{
    //memset(stats, 0, MAXROW * MAXCOL);

    // our program just uses the bottom 4 lines of the display
    printf ("built at %s %s\npress enter to start\n",
        __DATE__, __TIME__);
    wait_for_keypress(CH_ENTER);

    //asm("JSR $FC58"); // APPLESOFT HOME: WHY DOESNT THIS WORK?

    gotoxy(0,LORES_ROWS);
    POKE(TEXTWINDOW_TOP_EDGE,LORES_ROWS);

    gr_mode(SS_PAGE2OFF, SS_MIXEDON);
    lo_clear(page1, TGI_COLOR_BLACK);
    
    // randomly create critters
    //randomize(page1, 400);
    //gospergun(page1);
    simkins(page1);
    //glider(page1);

    run();
    
    printf ("all done! press enter to end\n");
    wait_for_keypress(CH_ENTER);
    POKE(TEXTWINDOW_TOP_EDGE,0);
    softsw(SS_TEXTON);
    
    return EXIT_SUCCESS;
}
