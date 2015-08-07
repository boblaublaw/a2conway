#include "a2conway.h"
#include <peekpoke.h>   // POKE

/*
 * This is the totally unoptimized implementation of Conway's
 * Game Of Life.  It is intended to prove correctness and 
 * measure baseline performance.
 */

extern uint16_t gr_page[2][24];

uint8_t peek_pixel(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    return (((uint8_t *)baseaddr[row/2])[col] & MASK_BY_ROW(row)) ? 1 : 0;
}

uint8_t count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, ROWABOVE(row), col)) {
        count++;
    }
    if (peek_pixel(baseaddr, ROWBELOW(row), col)) {
        count++;
    }
    if (peek_pixel(baseaddr, row, COLLEFT(col))) {
        count++;
    }
    if (peek_pixel(baseaddr, row, COLRIGHT(col))) {
        count++;
    }
    if (peek_pixel(baseaddr, ROWABOVE(row), COLLEFT(col))) {
        count++;
    }  
    if (peek_pixel(baseaddr, ROWBELOW(row), COLLEFT(col))) {
        count++;
    }  
    if (peek_pixel(baseaddr, ROWABOVE(row), COLRIGHT(col))) {
        count++;
    }   
    if (peek_pixel(baseaddr, ROWBELOW(row), COLRIGHT(col))) {
        count++;
    }   
    return count;
}   

void naive_analyze(uint16_t src[], uint16_t dst[])
{
    uint8_t row, col, n;

    for (row=0; row < MAXROWCNT; row++) {   
        for (col=0; col < MAXCOLCNT; col++) {
            n = count_neighbors(src, row, col);
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

uint8_t naive_engine(void)
{
    uint8_t res;
    while (1) {
        if (res = process_keys())
            return res;
        naive_analyze(gr_page[0], gr_page[1]);
        softsw(SS_PAGE2ON);
        naive_analyze(gr_page[1], gr_page[0]);
        softsw(SS_PAGE2OFF);
    }
}
