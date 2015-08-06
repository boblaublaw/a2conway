#include "a2conway.h"

/*
 * Draws a Gosper Glider Gun
 */

void gospergun(uint16_t page[])
{
        lo_clear(page, TGI_COLOR_BLACK);
        lo_plot(page, 0, 24, 0xf);
        lo_plot(page, 1, 22, 0xf);
        lo_plot(page, 1, 24, 0xf);
        lo_plot(page, 2, 12, 0xf);
        lo_plot(page, 2, 13, 0xf);
        lo_plot(page, 2, 20, 0xf);
        lo_plot(page, 2, 21, 0xf);
        lo_plot(page, 2, 34, 0xf);
        lo_plot(page, 2, 35, 0xf);
        lo_plot(page, 3, 11, 0xf);
        lo_plot(page, 3, 15, 0xf);
        lo_plot(page, 3, 20, 0xf);
        lo_plot(page, 3, 21, 0xf);
        lo_plot(page, 3, 34, 0xf);
        lo_plot(page, 3, 35, 0xf);
        lo_plot(page, 4, 0, 0xf);
        lo_plot(page, 4, 1, 0xf);
        lo_plot(page, 4, 10, 0xf);
        lo_plot(page, 4, 16, 0xf);
        lo_plot(page, 4, 20, 0xf);
        lo_plot(page, 4, 21, 0xf);
        lo_plot(page, 5, 0, 0xf);
        lo_plot(page, 5, 1, 0xf);
        lo_plot(page, 5, 10, 0xf);
        lo_plot(page, 5, 14, 0xf);
        lo_plot(page, 5, 16, 0xf);
        lo_plot(page, 5, 17, 0xf);
        lo_plot(page, 5, 22, 0xf);
        lo_plot(page, 5, 24, 0xf);
        lo_plot(page, 6, 10, 0xf);
        lo_plot(page, 6, 16, 0xf);
        lo_plot(page, 6, 24, 0xf);
        lo_plot(page, 7, 11, 0xf);
        lo_plot(page, 7, 15, 0xf);
        lo_plot(page, 8, 12, 0xf);
        lo_plot(page, 8, 13, 0xf);
}
