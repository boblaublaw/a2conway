#include "a2conway.h"

/*
 * This is the totally unoptimized implementation of Conway's
 * Game Of Life.  It is intended to prove correctness and 
 * measure baseline performance.
 */

uint8_t peek_pixel(uint16_t baseaddr[], uint8_t row, uint8_t col);
uint8_t count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col);

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

    for (row=0; row < MAXROW; row++) {   
        for (col=0; col < MAXCOL; col++) {
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
