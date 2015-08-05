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

// typedefs
typedef unsigned long   uint32_t; // 4 bytes
typedef long            int32_t;  // 4 bytes
typedef unsigned        uint16_t; // 2 bytes
typedef int             int16_t;  // 2 bytes
typedef unsigned char   uint8_t;  // 1 byte
typedef char            int8_t;   // 1 byte

// defines
#define ROWMASK                 0x00FF
#define COLMASK                 0xFF00
#define KEYPRESS_BUF_ADDR       0xC000
#define KEYCLEAR_BUF_ADDR       0xC010
#define SS_GRAPHICS_MODE        0xC050
#define SS_TEXT_MODE            0xC051
#define SS_FULLSCREEN_MODE      0xC052
#define SS_MIXED_MODE           0xC053
#define SS_PAGE1                0xC054
#define SS_PAGE2                0xC055
#define SS_LORES_MODE           0xC056
#define SS_HIRES_MODE           0xC057
#define softsw(x)               POKE(x,0)

#define LORES_PAGE1_BASE        0x400
#define LORES_PAGE2_BASE        0x800

#define TEXTWINDOW_TOP_EDGE     0x22
#define RSEED1                  0x4E
#define RSEED2                  0x4F

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

void clearkeybuf()
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

void waitforkeypress(uint8_t key)
{
    uint8_t c;
    clearkeybuf();
    for (;;) {
        if (kbhit() > 0) {
            c=cgetc();
            printf ("key hit: %d\n", c);
            clearkeybuf();
            if (c==key) {
                printf ("done waiting for %u\n", key);
                return;
            }
        }
    }
}

void gr(uint16_t page, uint16_t mode)
{
    softsw(SS_GRAPHICS_MODE);
    softsw(mode);
    softsw(page);
    softsw(SS_LORES_MODE);
}

#define LORES_COLS              40
#define LORES_ROWS              20

void loclear(uint16_t baseaddr[], uint8_t color)
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

int8_t keypress()
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

uint16_t get_random_bits(uint8_t count)
{
    uint16_t result = rand();
    uint16_t mask = 1;
    if (count > 1)
       mask = mask << (count - 1);
    mask -= 1;
    printf ("mask is %04x\n", mask);
    return result & mask;
}

void loplot(uint16_t baseaddr[], uint8_t row, uint8_t col, uint8_t color)
{
    uint8_t pairrow = row / 2;
    uint8_t *rowptr = baseaddr[pairrow];
    if (row & 0x1)
        color <<= 4;
    rowptr[col] |= color;
}

void randomize(uint16_t baseaddr[], uint8_t maxrow, uint16_t count)
{
    uint16_t r;
    uint8_t  row, col;

    printf ("randomization complete. press enter to continue\n");
    waitforkeypress(CH_ENTER);
    while (--count) {
        r = rand();
        // use the high bit to determine which nibble we turn on
        row = r & ROWMASK;
        col = (r & COLMASK) >> 8;
        row %= (maxrow * 2);
        col %= 40;
        //printf("turning on %d,%d with %04x\n", row, col, r);
        loplot(baseaddr, row, col, 0xf);
    }
}

void run(void) 
{

}

int main()
{
    // The Apple zero page has some semi-random garbage
    // suitable for seeding our PRNG
    uint16_t seed = PEEK(RSEED1) + PEEK(RSEED2) * 256;
    srand (seed);

    // our program just uses the bottom 4 lines of the display
    gotoxy(0,LORES_ROWS);
    POKE(TEXTWINDOW_TOP_EDGE,LORES_ROWS);

    printf ("built at %s %s\npress enter to start\n",
        __DATE__, __TIME__);
    waitforkeypress(CH_ENTER);

    gr(SS_PAGE1, SS_MIXED_MODE);
    loclear(page1, TGI_COLOR_BLACK);
    
    // randomly create critters
    randomize(page1, LORES_ROWS, 400);

    run();
    
    printf ("all done! press enter to end\n");
    waitforkeypress(CH_ENTER);
    POKE(TEXTWINDOW_TOP_EDGE,0);
    softsw(SS_TEXT_MODE);
    
    __asm__ ("JSR $FC58"); /* APPLESOFT HOME */
    return EXIT_SUCCESS;
}
