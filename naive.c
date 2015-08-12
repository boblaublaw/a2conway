#include "a2conway.h"
#include <peekpoke.h>   // POKE
#include <stdio.h>      // printf


#define WRAP_ROWABOVE(x) ( x == 0 ? MAXROWIDX  : (x - 1))
#define WRAP_ROWBELOW(x) ( x == MAXROWIDX ? 0 : (x + 1))
#define WRAP_COLLEFT(y)  ( y == 0 ? MAXCOLIDX  : (y - 1))
#define WRAP_COLRIGHT(y) ( y == MAXCOLIDX ? 0 : (y + 1))

#define NOWRAP_ROWABOVE(x) ( x == 0 ? -1  : (x - 1))
#define NOWRAP_ROWBELOW(x) ( x == MAXROWIDX ? -1 : (x + 1))
#define NOWRAP_COLLEFT(y)  ( y == 0 ? -1  : (y - 1))
#define NOWRAP_COLRIGHT(y) ( y == MAXCOLIDX ? -1 : (y + 1))

/*
 * This is the totally unoptimized implementation of Conway's
 * Game Of Life.  It is intended to prove correctness and 
 * measure baseline performance.
 */

extern uint16_t gr_page[2][24];

uint8_t peek_pixel(uint16_t baseaddr[], int8_t row, int8_t col)
{
    if ((row == 255) || (col == 255)) {
        //printf ("out of bounds: %d,%d\n", row, col);
        return 0;
    }
    return (((uint8_t *)baseaddr[row/2])[col] & MASK_BY_ROW(row)) ? 1 : 0;
}

uint8_t nowrap_count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, NOWRAP_ROWABOVE(row), col)) count++;
    if (peek_pixel(baseaddr, NOWRAP_ROWBELOW(row), col)) count++;
    if (peek_pixel(baseaddr, row, NOWRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, row, NOWRAP_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, NOWRAP_ROWABOVE(row), NOWRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, NOWRAP_ROWBELOW(row), NOWRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, NOWRAP_ROWABOVE(row), NOWRAP_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, NOWRAP_ROWBELOW(row), NOWRAP_COLRIGHT(col))) count++;
    return count;
}   

uint8_t wrap_count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, WRAP_ROWABOVE(row), col)) count++;
    if (peek_pixel(baseaddr, WRAP_ROWBELOW(row), col)) count++;
    if (peek_pixel(baseaddr, row, WRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, row, WRAP_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, WRAP_ROWABOVE(row), WRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, WRAP_ROWBELOW(row), WRAP_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, WRAP_ROWABOVE(row), WRAP_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, WRAP_ROWBELOW(row), WRAP_COLRIGHT(col))) count++;
    return count;
}   


void naive_analyze(uint16_t src[], uint16_t dst[], uint8_t wrap)
{
    uint8_t row, col, n;

    for (row=0; row < MAXROWCNT; row++) {   
        for (col=0; col < MAXCOLCNT; col++) {

            if (wrap)
                n = wrap_count_neighbors(src, row, col);
            else
                n = nowrap_count_neighbors(src, row, col);

            if (peek_pixel(src, row, col)) {
                if ((n == 2) || (n == 3))
                    lo_plot(dst, row, col, 0xF);
                else
                    lo_plot(dst, row, col, 0x0);
            }
            else {
                if (n == 3)
                    lo_plot(dst, row, col, 0xF);
                else
                    lo_plot(dst, row, col, 0x0);
            }
        }
    }
    return;
}

void naive_wrap_engine(void)
{
    while (1) {
        if (process_keys())
            break;
        naive_analyze(gr_page[0], gr_page[1], 1);
        softsw(SS_PAGE2ON);
        if (process_keys())
            break;
        naive_analyze(gr_page[1], gr_page[0], 1);
        softsw(SS_PAGE2OFF);
    }
}

void naive_nowrap_engine(void)
{
    while (1) {
        if (process_keys())
            break;
        naive_analyze(gr_page[0], gr_page[1], 0);
        softsw(SS_PAGE2ON);
        if (process_keys())
            break;
        naive_analyze(gr_page[1], gr_page[0], 0);
        softsw(SS_PAGE2OFF);
    }
}
