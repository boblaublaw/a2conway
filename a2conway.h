// typedefs
typedef unsigned long   uint32_t; // 4 bytes
typedef long            int32_t;  // 4 bytes
typedef unsigned        uint16_t; // 2 bytes
typedef int             int16_t;  // 2 bytes
typedef unsigned char   uint8_t;  // 1 byte
typedef char            int8_t;   // 1 byte

// defines
// #define MIXED_MODE              1

#define FINITE                  0
#define INFINITE                1

#define ENGINE_STOP             0
#define ENGINE_RUN              1

#define ENGINE_SEL_NAIVE        1
#define ENGINE_SEL_OPT1         2

#define ROWRANDMASK             0x00FF
#define COLRANDMASK             0xFF00

#ifdef MIXED_MODE
#define MAXROWPAIRCNT           20
#else
#define MAXROWPAIRCNT           24
#endif

#define MAXROWCNT               (MAXROWPAIRCNT * 2)
#define MAXCOLCNT               40

#define MAXROWIDX               (MAXROWCNT - 1)
#define MAXROWPAIRIDX           (MAXROWPAIRCNT - 1)
#define MAXCOLIDX               (MAXCOLCNT - 1)

#define KEYPRESS_BUF_ADDR       0xC000
#define KEYCLEAR_BUF_ADDR       0xC010

#define SS_80COLOFF             0xC00C
#define SS_80COLON              0xC00D

#define SS_TEXTOFF              0xC050
#define SS_TEXTON               0xC051
#define SS_MIXEDOFF             0xC052
#define SS_MIXEDON              0xC053
#define SS_PAGE2OFF             0xC054
#define SS_PAGE2ON              0xC055
#define SS_HIRESOFF             0xC056
#define SS_HIRESON              0xC057
#define softsw(x)               POKE(x,0)

#define LORES_PAGE1_BASE        0x400
#define LORES_PAGE2_BASE        0x800

#define TEXTWINDOW_TOP_EDGE     0x22
#define RSEED1                  0x4E
#define RSEED2                  0x4F

#define CLEARKEYBUF             while((PEEK(KEYPRESS_BUF_ADDR)) > 127) POKE(KEYCLEAR_BUF_ADDR, 0)
#define EVEN_ROW_MASK           0x0F
#define ODD_ROW_MASK            0xF0
#define MASK_BY_ROW(x)          (((x) & 0x1) ? ODD_ROW_MASK : EVEN_ROW_MASK)

#define INF_ROWABOVE(x) ( x == 0 ? MAXROWIDX  : (x - 1))
#define INF_ROWBELOW(x) ( x == MAXROWIDX ? 0 : (x + 1))
#define INF_COLLEFT(y)  ( y == 0 ? MAXCOLIDX  : (y - 1))
#define INF_COLRIGHT(y) ( y == MAXCOLIDX ? 0 : (y + 1))

#define FIN_ROWABOVE(x) ( x == 0 ? -1  : (x - 1))
#define FIN_ROWBELOW(x) ( x == MAXROWIDX ? -1 : (x + 1))
#define FIN_COLLEFT(y)  ( y == 0 ? -1  : (y - 1))
#define FIN_COLRIGHT(y) ( y == MAXCOLIDX ? -1 : (y + 1))

void text_mode(void);
void gr_mode(uint16_t page, uint16_t mode);
void lo_clear(uint16_t baseaddr[], uint8_t color);
void lo_plot(uint16_t baseaddr[], uint8_t row, uint8_t col, uint8_t color);
void glider(uint16_t page[]);
void simkins(uint16_t page[]);
void gospergun(uint16_t page[]);
void randomize(uint16_t baseaddr[], uint16_t count);
void naive_engine(void);
uint8_t process_keys(void);
void opt1_engine(void);
void wait_for_keypress(uint8_t key);
uint8_t peek_pixel(uint16_t baseaddr[], int8_t row, int8_t col);
uint8_t count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col);
