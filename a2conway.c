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

/*
 * GLOBALS:
 *
 * the base addresses for the 48 scanline pairs 
 * though we only use the first 20 pairs 
 * for lores graphics mode 40 x 48 x 16 colors 
 * we are using 40 x 40 x 16 
 */

// engine variables tell this program to stop, start, and which
// cell processing "engine" to use.
uint8_t engine_sel, engine_state;

// infinity determines if the world wraps around at the edges
uint8_t infinity;

uint16_t gr_page[2][24]={ {
    // first page of lores graphics memory is 24 pairs of rows
    0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780,
    0x0428, 0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 0x07A8,
    0x0450, 0x04D0, 0x0550, 0x05D0, 0x0650, 0x06D0, 0x0750, 0x07D0 }, {
    // second page of lores graphics memory is also 24 pairs of rows
    0x0800, 0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80,
    0x0828, 0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 0x0BA8,
    0x0850, 0x08D0, 0x0950, 0x09D0, 0x0A50, 0x0AD0, 0x0B50, 0x0BD0 } };

/*
 * FUNCTIONS:
 */
void wait_for_keypress(uint8_t key)
{
    uint8_t c;
    CLEARKEYBUF;
    for (;;) {
        if (kbhit() > 0) {
            if (key == (c = cgetc()))
            {
                CLEARKEYBUF;
                return;
            }
            if (c == 'q')
                exit(EXIT_SUCCESS);
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
    for (idx = 0; idx < MAXROWPAIRCNT; idx++) {
        memset ((uint8_t *)baseaddr[idx], color, MAXCOLCNT);
    }
}

void lo_plot(uint16_t baseaddr[], uint8_t row, uint8_t col, uint8_t color)
{
    uint8_t pairrow = row / 2;
    uint8_t *rowptr = baseaddr[pairrow];
    if (row & 0x1) {
        color <<= 4;
        rowptr[col] = (rowptr[col] & EVEN_ROW_MASK ) | color;
    }
    else {
        color &= 0x0F;
        rowptr[col] = (rowptr[col] & ODD_ROW_MASK ) | color;
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
    lo_clear(gr_page[0], TGI_COLOR_BLACK);

    while (count--) {
        r = rand();
        // use the high bit to determine which nibble we turn on
        row = r & ROWRANDMASK;
        col = (r & COLRANDMASK) >> 8;
        row %= MAXROWCNT;
        col %= MAXCOLCNT;
        //printf("turning on %d,%d with %04x\n", row, col, r);
        lo_plot(baseaddr, row, col, 0xf);
    }
}

void toggle_infinity(void)
{
    if (infinity == INFINITE)
        infinity = FINITE;
    else
        infinity = INFINITE;
#ifdef MIXED_MODE
    printf ("toggling infinity to %s\n", (infinity ? "on" : "off"));
#endif
}

// returning 1 will cause the currently running engine to exit
uint8_t process_keys(void) 
{
    uint8_t c;

    if (kbhit() > 0) {
        c=cgetc();
        CLEARKEYBUF;

        if (c == 'p') {
#ifdef MIXED_MODE
            printf("PAUSED. Press p to Continue.\n");
#endif
            wait_for_keypress('p');
        }
        else if (c == '1') {
            engine_sel=ENGINE_SEL_NAIVE;
            engine_state=ENGINE_RUN;
            return 1;
        }
        else if (c == '2') {
            engine_sel=ENGINE_SEL_OPT1;
            engine_state=ENGINE_RUN;
            return 1;
        }
#if 0
        else if (c == '3') {
            engine_sel=ENGINE_SEL_OPT2;
            engine_state=ENGINE_RUN;
            return 1;
        }
#endif
        else if (c == 'q') {
            engine_state=ENGINE_STOP;
            return 1;
        }
        else if (c == 'i')
            if (engine_sel != ENGINE_SEL_NAIVE) {
                printf("finite field not supported outside naive engine\n");
            } 
            else 
                toggle_infinity();
        else if (c == 'r') 
            randomize(gr_page[0], 400);
        else if (c == 'g')
            gospergun(gr_page[0]);
        else if (c == 's')
            simkins(gr_page[0]);
    }
    return 0;
}

void startup(void)
{
    printf("built at %s %s\n", __DATE__, __TIME__);
    printf("\nHotkeys:\n");
    printf("\tr: randomize\n");
    printf("\tg: gosper glider gun\n");
    printf("\ts: simkins glider gun\n");
    printf("\ti: toggle finite edge wrap (naive only)\n");
    printf("\tp: pause\n");
    printf("\t1: switch to naive engine\n");
    printf("\t2: switch to optimized engine 1\n");
    printf("\t3: switch to optimized engine 2\n");
    printf("\tq: quit\n");
    printf("press enter to start\n");
    wait_for_keypress(CH_ENTER);
}

int main(void)
{
    infinity = FINITE;
    engine_sel = ENGINE_SEL_OPT1;
    engine_state = ENGINE_RUN;

    startup();

#ifdef MIXED_MODE
    // our program just uses the bottom 4 lines of the display
    gotoxy(0,MAXROWPAIRCNT);
    POKE(TEXTWINDOW_TOP_EDGE,MAXROWPAIRCNT);
    gr_mode(SS_PAGE2OFF, SS_MIXEDON);
#else
    gr_mode(SS_PAGE2OFF, SS_MIXEDOFF);
#endif
    glider(gr_page[0]);

    while (engine_state == ENGINE_RUN) {
        if (engine_sel == ENGINE_SEL_NAIVE)
            naive_engine();
        else if (engine_sel == ENGINE_SEL_OPT1)
            opt1_engine();
        else
            opt2_engine();
    }
    
    // all done
    POKE(TEXTWINDOW_TOP_EDGE,0);
    softsw(SS_TEXTON);
    
    return EXIT_SUCCESS;
}
