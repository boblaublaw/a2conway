#include <peekpoke.h>           // POKE
#include "a2conway.h"

/*
 * This is the first optimized implementation of Conway's
 * Game Of Life. 
 *
 * by using precomputed indices for the pages above and below
 */

extern uint16_t page1[24];
extern uint16_t page2[24];

uint16_t page1above[24]={
    0x07D0, 0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 
    0x0780, 0x0428, 0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 
    0x07A8, 0x0450, 0x04D0, 0x0550, 0x05D0, 0x0650, 0x06D0, 0x0750 };

uint16_t page2above[24]={
    0x0BD0, 0x0800, 0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 
    0x0B80, 0x0828, 0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 
    0x0BA8, 0x0850, 0x08D0, 0x0950, 0x09D0, 0x0A50, 0x0AD0, 0x0B50 };

uint16_t page1below[24]={
    0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780, 0x0428, 
    0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 0x07A8, 0x0450, 
    0x04D0, 0x0550, 0x05D0, 0x0650, 0x06D0, 0x0750, 0x07D0, 0x0400 };

uint16_t page2below[24]={
    0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80, 0x0828, 
    0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 0x0BA8, 0x0850, 
    0x08D0, 0x0950, 0x09D0, 0x0A50, 0x0AD0, 0x0B50, 0x0BD0, 0x0800 };


void opt1_engine(void)
{
    uint8_t row, col, n, alive;
    uint8_t *rowptr;

    while (1) {
        if (process_keys())
            break;

        for (row=0; row < MAXROW; row++) {   
            rowptr = page1[row];
            for (col=0; col < MAXCOL; col++) {
                // work page 1 to page 2
                alive = 0;
                n = 0;
                if (alive) {
                    if ((n == 2) || (n == 3))
                        lo_plot(page2, row, col, 0xF);
                    else
                        lo_plot(page2, row, col, 0x0);
                }
                else {
                    if (n == 3)
                        lo_plot(page2, row, col, 0xF);
                    else
                        lo_plot(page2, row, col, 0x0);
                }
            }
        }
        softsw(SS_PAGE2ON);

        for (row=0; row < MAXROW; row++) {   
            rowptr = page2[row];
            for (col=0; col < MAXCOL; col++) {
                // work page 2 to page 1
                alive = 0;
                n = 0;
                if (alive) {
                    if ((n == 2) || (n == 3))
                        lo_plot(page1, row, col, 0xF);
                    else
                        lo_plot(page1, row, col, 0x0);
                }
                else {
                    if (n == 3)
                        lo_plot(page1, row, col, 0xF);
                    else
                        lo_plot(page1, row, col, 0x0);
                }
            }
        }
        softsw(SS_PAGE2OFF);
    }
}
