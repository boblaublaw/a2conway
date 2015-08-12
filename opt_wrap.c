#include <peekpoke.h>           // POKE
#include "a2conway.h"

/*
 * This is the no-wrap optimized implementation of Conway's
 * Game Of Life. 
 *
 * memory addresses for rows are loaded into indexed lookup tables.
 * Since LO-RES graphics use one byte for two pixels, two life cells
 * are examined at a time, A5, and A8:
 *
 *                  +----+----+----+
 *                  |    |    |    |
 * aboveptr         | A1 | A2 | A3 |
 * ODD_ROW_MASK     |    |    |    |
 *                  +----+----+----+
 *                  |    | ** |    |
 * rowptr           | A4 |*A5*| A6 |
 * EVEN_ROW_MASK    |    | ** |    |
 *                  +----+----+----+
 *                  |    | ** |    |
 * rowptr           | A7 |*A8*| A9 |
 * ODD_ROW_MASK     |    | ** |    |
 *                  +----+----+----+
 *                  |    |    |    |
 * belowptr         | B1 | B2 | B3 |
 * EVEN_ROW_MASK    |    |    |    |
 *                  +----+----+----+
 *
 * The column loop is partially unrolled in that the first and last
 * columns are specialed outside the loop, due to their special cases
 * for wrapping the world from col 39 to col 0.
 */

extern uint16_t gr_page[2][24];
#ifdef MIXED_MODE
/*
 * In mixed text/graphics mode, only 20 rowpair addresses are needed
 */
uint16_t pageabove[2][24]={ {
    0x05D0, 0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 
    0x0780, 0x0428, 0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 
    0x07A8, 0x0450, 0x04D0, 0x0550, 0x0000, 0x0000, 0x0000, 0x0000 }, {
    0x09D0, 0x0800, 0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 
    0x0B80, 0x0828, 0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 
    0x0BA8, 0x0850, 0x08D0, 0x0950, 0x0000, 0x0000, 0x0000, 0x0000 } };
uint16_t pagebelow[2][24]={ {
    0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780, 0x0428, 
    0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 0x07A8, 0x0450, 
    0x04D0, 0x0550, 0x05D0, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000 }, {
    0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80, 0x0828, 
    0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 0x0BA8, 0x0850, 
    0x08D0, 0x0950, 0x09D0, 0x0800, 0x0000, 0x0000, 0x0000, 0x0000 } };
 #else
 /*
  * In fullscreen graphics mode, 24 rowpair addresses are needed
  */
uint16_t pageabove[2][24]={ {
    0x07D0, 0x0400, 0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 
    0x0780, 0x0428, 0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 
    0x07A8, 0x0450, 0x04D0, 0x0550, 0x05D0, 0x0650, 0x06D0, 0x0750 }, {
    0x0BD0, 0x0800, 0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 
    0x0B80, 0x0828, 0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 
    0x0BA8, 0x0850, 0x08D0, 0x0950, 0x09D0, 0x0A50, 0x0AD0, 0x0B50 } };
uint16_t pagebelow[2][24]={ {
    0x0480, 0x0500, 0x0580, 0x0600, 0x0680, 0x0700, 0x0780, 0x0428, 
    0x04A8, 0x0528, 0x05A8, 0x0628, 0x06A8, 0x0728, 0x07A8, 0x0450, 
    0x04D0, 0x0550, 0x05D0, 0x0650, 0x06D0, 0x0750, 0x07D0, 0x0400 }, {
    0x0880, 0x0900, 0x0980, 0x0A00, 0x0A80, 0x0B00, 0x0B80, 0x0828, 
    0x08A8, 0x0928, 0x09A8, 0x0A28, 0x0AA8, 0x0B28, 0x0BA8, 0x0850, 
    0x08D0, 0x0950, 0x09D0, 0x0A50, 0x0AD0, 0x0B50, 0x0BD0, 0x0800 } };
#endif

void opt_nowrap_engine(void)
{
    uint8_t src, dst, col, result, rowpair;
    uint8_t A, B, A1, A2, A3, A4, A5, A6, A7, A8, A9, B1, B2, B3, CV;
    register uint8_t *rowptr, *aboveptr, *belowptr;
    uint8_t *dstptr;
    uint16_t *srcpage, *dstpage, *belowsrc, *abovesrc;

    while (1) {
        for (src=0; src < 2; src++) {
            if (process_keys())
                return;
            dst = !src;
            abovesrc = pageabove[src];
            srcpage = gr_page[src];
            belowsrc = pagebelow[src];
            dstpage = gr_page[dst];
            for (rowpair=0; rowpair < MAXROWPAIRCNT; rowpair++) {   
                // row specific metadata need only be set up once:
                aboveptr   = abovesrc[rowpair];
                rowptr = srcpage[rowpair];
                belowptr = belowsrc[rowpair];
                dstptr = dstpage[rowpair];

                A1 = aboveptr[MAXCOLIDX]    & ODD_ROW_MASK  ? 1 : 0;
                A2 = aboveptr[0]            & ODD_ROW_MASK  ? 1 : 0;
                A3 = aboveptr[1]            & ODD_ROW_MASK  ? 1 : 0;
                A4 = rowptr[MAXCOLIDX]      & EVEN_ROW_MASK ? 1 : 0;
                A5 = rowptr[0]              & EVEN_ROW_MASK ? 1 : 0;
                A6 = rowptr[1]              & EVEN_ROW_MASK ? 1 : 0;
                A7 = rowptr[MAXCOLIDX]      & ODD_ROW_MASK  ? 1 : 0;
                A8 = rowptr[0]              & ODD_ROW_MASK  ? 1 : 0;
                A9 = rowptr[1]              & ODD_ROW_MASK  ? 1 : 0;
                B1 = belowptr[MAXCOLIDX]    & EVEN_ROW_MASK ? 1 : 0;
                B2 = belowptr[0]            & EVEN_ROW_MASK ? 1 : 0;
                B3 = belowptr[1]            & EVEN_ROW_MASK ? 1 : 0;

                // CV is a value common to the sum of A and B, add it once
                CV = A4 + A6 + A7 + A9;
                A = A1 + A2 + A3 + CV + A8;
                B = A5 + B1 + B2 + B3 + CV;

                //start by presuming everything is dead
                result = 0;
                // upper cell is centered on A5
                if ((A5 && ((A == 2) || (A == 3))) || 
                    ((A5 == 0) && (A == 3))) {
                    //even row is alive
                    result = EVEN_ROW_MASK;
                }
                // lower cell is centered on A8
                if ((A8 && ((B == 2) || (B == 3))) || 
                    ((A8 == 0) && (B == 3))) {
                    //odd row is alive
                    result |= ODD_ROW_MASK;
                }
                // save new upper and lower cell together
                dstptr[0]=result;

                // shift a 4x3 grid one cell to the right, allowing
                // us to reuse the counted values from the left 2 
                // columns.  only the right column is new information.
                for (col = 1; col < MAXCOLCNT - 1; col++) {
                    A1 = A2;
                    A2 = A3;
                    A3 = aboveptr[col+1]        & ODD_ROW_MASK  ? 1 : 0;
                    A4 = A5;
                    A5 = A6;
                    A6 = rowptr[col+1]          & EVEN_ROW_MASK ? 1 : 0;
                    A7 = A8;
                    A8 = A9;
                    A9 = rowptr[col+1]          & ODD_ROW_MASK  ? 1 : 0;
                    B1 = B2;
                    B2 = B3;
                    B3 = belowptr[col+1]        & EVEN_ROW_MASK ? 1 : 0;

                    CV = A4 + A6 + A7 + A9;
                    A = A1 + A2 + A3 + CV + A8;
                    B = A5 + B1 + B2 + B3 + CV;

                    result = 0;
                    if ((A5 && ((A == 2) || (A == 3))) || 
                        ((A5 == 0) && (A == 3))) {
                        result = EVEN_ROW_MASK;
                    }

                    if ((A8 && ((B == 2) || (B == 3))) || 
                        ((A8 == 0) && (B == 3))) {
                        result |= ODD_ROW_MASK;
                    }
                    dstptr[col]=result;
                }

                // on the rightmost examination, the cells to the right
                // are actually the first cell in this row (due to wrap
                // around.)  
                A1 = A2;
                A2 = A3;
                A3 = aboveptr[0]                  & ODD_ROW_MASK  ? 1 : 0;
                A4 = A5;
                A5 = A6;
                A6 = rowptr[0]                    & EVEN_ROW_MASK ? 1 : 0;
                A7 = A8;
                A8 = A9;
                A9 = rowptr[0]                    & ODD_ROW_MASK  ? 1 : 0;
                B1 = B2;
                B2 = B3;
                B3 = belowptr[0]                  & EVEN_ROW_MASK ? 1 : 0;

                CV = A4 + A6 + A7 + A9;
                A = A1 + A2 + A3 + CV + A8;
                B = A5 + B1 + B2 + B3 + CV;

                result = 0;
                if ((A5 && ((A == 2) || (A == 3))) || 
                    ((A5 == 0) && (A == 3))) {
                    result = EVEN_ROW_MASK;
                }
                if ((A8 && ((B == 2) || (B == 3))) || 
                    ((A8 == 0) && (B == 3))) {
                    result |= ODD_ROW_MASK;
                }
                dstptr[MAXCOLIDX]=result;
            }
            // flip the graphics page
            softsw(dst + SS_PAGE2OFF);
        }
    }
}

