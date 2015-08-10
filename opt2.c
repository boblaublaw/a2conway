#include <peekpoke.h>           // POKE
#include "a2conway.h"

/*
 * This is the second optimized implementation of Conway's
 * Game Of Life. 
 */

extern uint16_t gr_page[2][24];
#ifdef MIXED_MODE
/*
 * In mixed text/graphics mode, only 20 rowpair addresses are needed
 */
extern uint16_t pageabove[2][24];
extern uint16_t pagebelow[2][24];
#else
/*
 * In fullscreen graphics mode, 24 rowpair addresses are needed
 */
extern uint16_t pageabove[2][24];
extern uint16_t pagebelow[2][24];
#endif


void opt2_engine(void)
{
}
