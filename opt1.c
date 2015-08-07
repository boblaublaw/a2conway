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


void opt1_engine(void)
{
    uint8_t src, dst, row, col, n, alive, mask;
    uint8_t *rowptr, *aboveptr, *belowptr;
    uint16_t *srcpage, *dstpage;

    while (1) {
        if (process_keys())
            break;
        for (src=0; src < 2; src++) {
            dst = !src;
            printf("src page: %d dst page: %d\n", src,dst);
            srcpage=gr_page[src];
            dstpage=dstpage;
#if 0
            for (rowpair=0; rowpair < MAXPAIRROWCNT; rowpair++) {   
                // row specific metadata need only be set up once:
                aboveptr = pageabove[src][rowpair];
                rowptr = srcpage[rowpair];
                belowptr = pagebelow[src][rowpair];
                printf("above: %p, self %p, below %p\n", aboveptr, rowptr, belowptr);

                A1 = 


                for (rowparity=0; rowparity < 2; rowparity++) {
                    row = (rowpair * 2) + rowparity;
                    mask = MASK_BY_ROW(row);
                    printf ("rowpair: %d rowparity: %d row: %d mask %x\n", rowpair, rowparity, row, mask);
                    wait_for_keypress(CH_ENTER);

                // logic for first column:
                col = 0;
                alive = rowptr[0] & mask;
            
                n = (aboveptr[MAXCOLIDX] & mask)    ? 1 : 0 + \
                    (aboveptr[0] & mask)            ? 1 : 0 + \
                    (aboveptr[1] & mask)            ? 1 : 0 + \
                    (rowptr[MAXCOLIDX] & mask)      ? 1 : 0 + \
                    (rowptr[1] & mask)              ? 1 : 0 + \
                    (belowptr[MAXCOLIDX] & mask)    ? 1 : 0 + \
                    (belowptr[0] & mask)            ? 1 : 0 + \
                    (belowptr[1] & mask)            ? 1 : 0;

                if (count_neighbors(srcpage, row, col) != n) {
                    printf ("alg fail: %d, %d: right: %d wrong: %d\n", row, col, count_neighbors(srcpage, row, col), n);
                    wait_for_keypress(CH_ENTER);

                    if (peek_pixel(srcpage, ROWABOVE(row), COLLEFT(col)) != ((aboveptr[MAXCOLIDX] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: above left (%d, %d) alive: r%d w%d\n", 
                            row, col, 
                            row - 1, MAXCOLIDX, 
                            peek_pixel(srcpage, ROWABOVE(row), COLLEFT(col)), 
                            ((aboveptr[MAXCOLIDX] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, ROWABOVE(row), col) != ((aboveptr[col] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: above (%d, %d) alive: r%d w%d\n", row, col, ROWABOVE(row), col, peek_pixel(srcpage, ROWABOVE(row), col), ((aboveptr[col] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, ROWABOVE(row), COLRIGHT(col)) != ((aboveptr[col + 1] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: above right (%d, %d) alive: r%d w%d\n", row, col, ROWABOVE(row), COLRIGHT(col), peek_pixel(srcpage, ROWABOVE(row), COLRIGHT(col)), ((aboveptr[col + 1] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, row, COLLEFT(col)) != ((rowptr[MAXCOLIDX] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: left (%d, %d) alive: r%d w%d\n", row, col, row, COLLEFT(col), peek_pixel(srcpage, row, COLLEFT(col)), ((rowptr[MAXCOLIDX] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, row, COLRIGHT(col)) != ((rowptr[col + 1] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: right (%d, %d) alive: r%d w%d\n", row, col, row, COLRIGHT(col), peek_pixel(srcpage, row, COLRIGHT(col)), ((rowptr[col + 1] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, ROWBELOW(row), COLLEFT(col)) != ((belowptr[MAXCOLIDX] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: below left (%d, %d) alive: r%d w%d\n", row, col, ROWBELOW(row), COLLEFT(col),peek_pixel(srcpage, ROWBELOW(row), COLLEFT(col)), ((belowptr[MAXCOLIDX] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, ROWBELOW(row), col) != ((belowptr[col] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: below (%d, %d) alive: r%d w%d\n", row, col, ROWBELOW(row), col,peek_pixel(srcpage, ROWBELOW(row), col), ((belowptr[col] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                    if (peek_pixel(srcpage, ROWBELOW(row), COLRIGHT(col)) != ((belowptr[col+1] & mask) ? 1 : 0 )) {
                        printf ("disagree %d, %d: below right (%d, %d) alive: r%d w%d\n", row, col, ROWBELOW(row), COLRIGHT(col),peek_pixel(srcpage, ROWBELOW(row), COLRIGHT(col)),((belowptr[col+1] & mask) ? 1 : 0 ));
                        wait_for_keypress(CH_ENTER);
                    }
                }
#if 0
                if (alive || n) {
                    printf ("cell %d, %d is %s\n", row, 0, (alive ? "alive" : "dead"));
                    printf ("cell %d, %d has %d neighbors\n", row, 0, n);
                    printf ("assigning value %x\n", ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
                    wait_for_keypress(CH_ENTER);
                }
#endif
                lo_plot(dstpage, row, 0, ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
#if 0
                // logic for col 1 through 38:
                for (col=1; col < MAXCOLCNT - 1; col++) {
                    alive = rowptr[col] & mask;
                    n = (aboveptr[col - 1] & mask) ? 1 : 0 + \
                        (aboveptr[col] & mask) ? 1 : 0 + \
                        (aboveptr[col + 1] & mask) ? 1 : 0 + \
                        (rowptr[col - 1] & mask) ? 1 : 0 + \
                        (rowptr[col + 1] & mask) ? 1 : 0 + \
                        (aboveptr[col - 1] & mask) ? 1 : 0 + \
                        (aboveptr[col] & mask) ? 1 : 0 + \
                        (aboveptr[col + 1] & mask) ? 1 : 0;
#if 0
                    if (alive || n) {
                        printf ("cell %d, %d is %s\n", row, col, (alive ? "alive" : "dead"));
                        printf ("cell %d, %d has %d neighbors\n", row, col, n);
                        printf ("assigning value %x\n", ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
                    }
#endif
                    if (count_neighbors(srcpage, row, col) != n) {
                        printf ("alg fail: %d, %d: right: %d wrong: %d\n", row, col, count_neighbors(srcpage, row, col), n);
                        wait_for_keypress(CH_ENTER);
                    }
                    lo_plot(dstpage, row, col, ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
                }

                // logic for the last column:
                col = MAXCOLIDX;
                alive = rowptr[MAXCOLIDX] & mask;
                n = (aboveptr[MAXCOLIDX - 1] & mask) ? 1 : 0 + \
                    (aboveptr[MAXCOLIDX] & mask) ? 1 : 0 + \
                    (aboveptr[0] & mask) ? 1 : 0 + \
                    (rowptr[MAXCOLIDX - 1] & mask) ? 1 : 0 + \
                    (rowptr[0] & mask) ? 1 : 0 + \
                    (belowptr[MAXCOLIDX - 1] & mask) ? 1 : 0 + \
                    (belowptr[MAXCOLIDX] & mask) ? 1 : 0 + \
                    (belowptr[0] & mask) ? 1 : 0;
                if (count_neighbors(srcpage, row, col) != n) {
                    printf ("alg fail: %d, %d: right: %d wrong: %d\n", row, col, count_neighbors(srcpage, row, col), n);
                    wait_for_keypress(CH_ENTER);
                }
#if 0
                if (alive || n) { 
                    printf ("cell %d, %d is %s\n", row, MAXCOLIDX, (alive ? "alive" : "dead"));
                    printf ("cell %d, %d has %d neighbors\n", row, MAXCOLIDX, n);
                    printf ("assigning value %x\n", ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
                    wait_for_keypress(CH_ENTER);
                }
#endif
                lo_plot(dstpage, row, MAXCOLIDX, ((alive && ((n == 2) || (n == 3)) || (!alive && (n == 3))) ? 0xF : 0x0));
#endif

            }
            // after drawing, switch the page
            //wait_for_keypress(CH_ENTER);
#endif
            softsw(dst + SS_PAGE2OFF);
        }
    }
}

