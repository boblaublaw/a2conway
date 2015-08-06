#include "a2conway.h"

void glider(uint16_t page[])
{
        lo_plot(page, 0, 1, 0xf);
        lo_plot(page, 1, 2, 0xf);
        lo_plot(page, 2, 0, 0xf);
        lo_plot(page, 2, 1, 0xf);
        lo_plot(page, 2, 2, 0xf);
}
