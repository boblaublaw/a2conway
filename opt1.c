#include <peekpoke.h>           // POKE
#include <stdio.h>              // printf
#include <apple2enh.h>          // CH_ENTER
#include "a2conway.h"

/*
 * This is the first optimized implementation of Conway's
 * Game Of Life. 
 *
 * by using precomputed indices for the pages above and below
 */

extern uint16_t gr_page[2][24];
#ifdef MIXED_MODE
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


void opt1_engine(void)
{
    uint8_t src, dst, row, col, n, alive, mask, result, rowpair;
    uint8_t A, B, A1, A2, A3, A4, A5, A6, A7, A8, A9, B1, B2, B3;
    uint8_t *dstptr, *rowptr, *aboveptr, *belowptr;
    uint16_t *srcpage, *dstpage, *belowsrc, *abovesrc;

    while (1) {
        if (process_keys())
            break;
        for (src=0; src < 2; src++) {
            dst = !src;
            //printf("src page: %d dst page: %d\n", src,dst);
            abovesrc=pageabove[src];
            srcpage=gr_page[src];
            belowsrc=pagebelow[src];
            dstpage=gr_page[dst];
            for (rowpair=0; rowpair < MAXROWPAIRCNT; rowpair++) {   
                // row specific metadata need only be set up once:
                aboveptr = abovesrc[rowpair];
                rowptr = srcpage[rowpair];
                belowptr = belowsrc[rowpair];
                dstptr = dstpage[rowpair];
                //printf("above: %p, self %p, below %p dst: %p\n", aboveptr, rowptr, belowptr, dstptr);
                //wait_for_keypress(CH_ENTER);

                for (col = 0; col < MAXCOLCNT; col++) {
                    if (col == 0) {
                        A1 = aboveptr[MAXCOLIDX]    & ODD_ROW_MASK ? 1 : 0;
                        A2 = aboveptr[0]            & ODD_ROW_MASK ? 1 : 0;
                        A3 = aboveptr[1]            & ODD_ROW_MASK ? 1 : 0;
                        A4 = rowptr[MAXCOLIDX]      & EVEN_ROW_MASK ? 1 : 0;
                        A5 = rowptr[0]              & EVEN_ROW_MASK ? 1 : 0;
                        A6 = rowptr[1]              & EVEN_ROW_MASK ? 1 : 0;
                        A7 = rowptr[MAXCOLIDX]      & ODD_ROW_MASK ? 1 : 0;
                        A8 = rowptr[0]              & ODD_ROW_MASK ? 1 : 0;
                        A9 = rowptr[1]              & ODD_ROW_MASK ? 1 : 0;

                        B1 = belowptr[MAXCOLIDX]    & EVEN_ROW_MASK ? 1 : 0;
                        B2 = belowptr[0]            & EVEN_ROW_MASK ? 1 : 0;
                        B3 = belowptr[1]            & EVEN_ROW_MASK ? 1 : 0;
                    }
                    else if (col == MAXCOLIDX) {
                        A1 = aboveptr[col-1]        & ODD_ROW_MASK ? 1 : 0;
                        A2 = aboveptr[col]          & ODD_ROW_MASK ? 1 : 0;
                        A3 = aboveptr[0]            & ODD_ROW_MASK ? 1 : 0;
                        A4 = rowptr[col-1]          & EVEN_ROW_MASK ? 1 : 0;
                        A5 = rowptr[col]            & EVEN_ROW_MASK ? 1 : 0;
                        A6 = rowptr[0]              & EVEN_ROW_MASK ? 1 : 0;
                        A7 = rowptr[col-1]          & ODD_ROW_MASK ? 1 : 0;
                        A8 = rowptr[col]            & ODD_ROW_MASK ? 1 : 0;
                        A9 = rowptr[0]              & ODD_ROW_MASK ? 1 : 0;

                        B1 = belowptr[col-1]        & EVEN_ROW_MASK ? 1 : 0;
                        B2 = belowptr[col]          & EVEN_ROW_MASK ? 1 : 0;
                        B3 = belowptr[0]            & EVEN_ROW_MASK ? 1 : 0;
                    }
                    else {
                        A1 = aboveptr[col-1]        & ODD_ROW_MASK ? 1 : 0;
                        A2 = aboveptr[col]          & ODD_ROW_MASK ? 1 : 0;
                        A3 = aboveptr[col+1]        & ODD_ROW_MASK ? 1 : 0;
                        A4 = rowptr[col-1]          & EVEN_ROW_MASK ? 1 : 0;
                        A5 = rowptr[col]            & EVEN_ROW_MASK ? 1 : 0;
                        A6 = rowptr[col+1]          & EVEN_ROW_MASK ? 1 : 0;
                        A7 = rowptr[col-1]          & ODD_ROW_MASK ? 1 : 0;
                        A8 = rowptr[col]            & ODD_ROW_MASK ? 1 : 0;
                        A9 = rowptr[col+1]          & ODD_ROW_MASK ? 1 : 0;

                        B1 = belowptr[col-1]        & EVEN_ROW_MASK ? 1 : 0;
                        B2 = belowptr[col]          & EVEN_ROW_MASK ? 1 : 0;
                        B3 = belowptr[col+1]        & EVEN_ROW_MASK ? 1 : 0;
                    }

                    A = (A1 + A2 + A3 \
                       + A4      + A6 \
                       + A7 + A8 + A9);

                    B = (A4 + A5 + A6 \
                       + A7      + A9 \
                       + B1 + B2 + B3);

                    if ((A5 && ((A == 2) || (A == 3))) || 
                        ((A5 == 0) && (A == 3))) {
                        //even row is alive
                        result = EVEN_ROW_MASK;
                    }
                    else
                        //even row is dead
                        result = 0;

                    // at this point odd row has already been assigned as dead
                    if ((A8 && ((B == 2) || (B == 3))) || 
                        ((A8 == 0) && (B == 3))) {
                        //odd row is alive
                        result |= ODD_ROW_MASK;
                    }

                    dstptr[col]=result;
#if 0
                    printf("[%d,%d]:n:%d %d%d%d %d%d%d %d%d%d", rowpair * 2, col, A, A1, A2, A3, A4, A5, A6, A7, A8, A9);
                    printf("\n");
                    printf("[%d,%d]:n:%d %d%d%d %d%d%d %d%d%d", (rowpair * 2) + 1, col, B, A4, A5, A6, A7, A8, A9, B1, B2, B3);
                    wait_for_keypress(CH_ENTER);
                    printf("\n");
#endif
                }
            }
            softsw(dst + SS_PAGE2OFF);
        }
    }
}
