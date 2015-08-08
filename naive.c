#include "a2conway.h"
#include <peekpoke.h>   // POKE
#include <stdio.h>      // printf

/*
 * This is the totally unoptimized implementation of Conway's
 * Game Of Life.  It is intended to prove correctness and 
 * measure baseline performance.
 */

extern uint16_t gr_page[2][24];
extern uint8_t infinity;

uint8_t peek_pixel(uint16_t baseaddr[], int8_t row, int8_t col)
{
    if ((row == 255) || (col == 255)) {
        //printf ("out of bounds: %d,%d\n", row, col);
        return 0;
    }
    return (((uint8_t *)baseaddr[row/2])[col] & MASK_BY_ROW(row)) ? 1 : 0;
}

uint8_t finite_count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, FIN_ROWABOVE(row), col)) count++;
    if (peek_pixel(baseaddr, FIN_ROWBELOW(row), col)) count++;
    if (peek_pixel(baseaddr, row, FIN_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, row, FIN_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, FIN_ROWABOVE(row), FIN_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, FIN_ROWBELOW(row), FIN_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, FIN_ROWABOVE(row), FIN_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, FIN_ROWBELOW(row), FIN_COLRIGHT(col))) count++;
    return count;
}   

uint8_t infinite_count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col)
{
    uint8_t count = 0;
    if (peek_pixel(baseaddr, INF_ROWABOVE(row), col)) count++;
    if (peek_pixel(baseaddr, INF_ROWBELOW(row), col)) count++;
    if (peek_pixel(baseaddr, row, INF_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, row, INF_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, INF_ROWABOVE(row), INF_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, INF_ROWBELOW(row), INF_COLLEFT(col))) count++;
    if (peek_pixel(baseaddr, INF_ROWABOVE(row), INF_COLRIGHT(col))) count++;
    if (peek_pixel(baseaddr, INF_ROWBELOW(row), INF_COLRIGHT(col))) count++;
    return count;
}   


void naive_analyze(uint16_t src[], uint16_t dst[])
{
    uint8_t row, col, n;

    for (row=0; row < MAXROWCNT; row++) {   
        for (col=0; col < MAXCOLCNT; col++) {
            if (infinity)
                n = infinite_count_neighbors(src, row, col);
            else
                n = finite_count_neighbors(src, row, col);
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

void naive_engine(void)
{
    while (1) {
        if (process_keys())
            break;
        naive_analyze(gr_page[0], gr_page[1]);
        softsw(SS_PAGE2ON);
        naive_analyze(gr_page[1], gr_page[0]);
        softsw(SS_PAGE2OFF);
    }
}
