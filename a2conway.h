
// typedefs
typedef unsigned long   uint32_t; // 4 bytes
typedef long            int32_t;  // 4 bytes
typedef unsigned        uint16_t; // 2 bytes
typedef int             int16_t;  // 2 bytes
typedef unsigned char   uint8_t;  // 1 byte
typedef char            int8_t;   // 1 byte

// defines
#define ROWRANDMASK             0x00FF
#define COLRANDMASK             0xFF00

#define MAXROWPAIR              20
#define MAXROW                  ( MAXROWPAIR * 2 )
#define MAXCOL                  40

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

void clearkeybuf(void);
void wait_for_keypress(uint8_t key);
void text_mode(void);
void gr_mode(uint16_t page, uint16_t mode);
void lo_clear(uint16_t baseaddr[], uint8_t color);
void lo_plot(uint16_t baseaddr[], uint8_t row, uint8_t col, uint8_t color);
void glider(uint16_t page[]);
void simkins(uint16_t page[]);
void gospergun(uint16_t page[]);
void randomize(uint16_t baseaddr[], uint16_t count);
void run(void);
uint8_t peek_pixel(uint16_t baseaddr[], uint8_t row, uint8_t col);
uint8_t count_neighbors(uint16_t baseaddr[], uint8_t row, uint8_t col);
uint16_t analyze(uint16_t src[], uint16_t dst[]);
int8_t keypress(void);
