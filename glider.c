#include <apple2enh.h>          // CH_ENTER, TGI_*
#include "a2conway.h"

/*
 * draws a glider in the top left of the screen
 */
void glider(uint16_t page[])
{
        lo_clear(page, TGI_COLOR_BLACK);
        lo_plot(page, 0, 1, 0xf);
        lo_plot(page, 1, 2, 0xf);
        lo_plot(page, 2, 0, 0xf);
        lo_plot(page, 2, 1, 0xf);
        lo_plot(page, 2, 2, 0xf);
}
