/*
 * Simple include file for our utility routines
 */

#ifndef __CPA_H
#define __CPA_H

#include <stdint.h>
#include <libopencm3/stm32/gpio.h>

#ifndef NULL
#define NULL (void *)(0x0000)
#endif

extern int console; // System console for the application
extern int debug_console; // debug console for the application

/* 
 * Giant enum which gives a name to all of the GPIO pins on
 * the chip. Ports A - I.
 */
enum GPIO_PORT_PIN {
        PA0 = 0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
        PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
        PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
        PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
        PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
        PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
        PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7,
        PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
        PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7,
        PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
        PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7,
        PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
        PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7,
        PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15,
        PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7,
        PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15,
        PI0, PI1, PI2, PI3, PI4, PI5, PI6, PI7,
        PI8, PI9, PI10, PI11, PI12, PI13, PI14, PI15,
};


// dump functions
uint8_t *dump_line(int chan, uint8_t *addr, uint8_t *base);
uint8_t *dump_page(int chan, uint8_t *addr, uint8_t *base);

#ifndef CONSOLE_UART
#define CONSOLE_UART    USART6
#endif
/*
 * Systick / Clock functions
 */
void msleep(uint32_t delay);
uint32_t mtime(void);
void clock_init(int enable_systick);
void clock_init_heartbeat(int enable_systick, enum GPIO_PORT_PIN led);
char *asctime(uint32_t millis);

/* 
 * Debugging functions
 */
void debug_init(void);
char debug_getc(int wait);
void debug_putc(char c);
void debug_puts(const char *s);
void debug_wait(void);

/*
 * SDIO Functions
 */
enum SD_CLOCK_DIV {
    CLOCK_24MHZ = 0,
    CLOCK_16MHZ,
    CLOCK_12MHZ,
    CLOCK_8MHZ,
    CLOCK_4MHZ,
    CLOCK_1MHZ,
    CLOCK_400KHZ
};

int sd_bus(int bits, enum SD_CLOCK_DIV freq);

/* this define lets the init code know that you are using a GPIO as a card
 * detect pin */
#define SDIO_HAS_CARD_DETECT

enum SDIO_CLOCK_DIV {
    SDIO_24MHZ = 0,
    SDIO_16MHZ,
    SDIO_12MHZ,
    SDIO_8MHZ,
    SDIO_4MHZ,
    SDIO_1MHZ,
    SDIO_400KHZ
};

enum SDIO_POWER_STATE {
    SDIO_POWER_ON,
    SDIO_POWER_OFF
};

#define SDIO_CARD_CCS(c)     (((c)->ocr & 0x20000000) != 0)
#define SDIO_CARD_UHS2(c)   (((c)->ocr & 0x40000000) != 0)
#define SDIO_CARD_LVOK(c)   (((c)->ocr & 0x01000000) != 0)

typedef struct SDIO_CARD_DATA {
    uint32_t    props;
    uint32_t    ocr;
    uint32_t    cid[4];
    uint32_t    csd[4];
    uint32_t    scr[2];
    uint32_t    status[16];
    uint32_t    size;
    uint16_t    rca;
} * SDIO_CARD;


int sdio_bus(int bits, enum SDIO_CLOCK_DIV freq);
void sdio_init(void);
void sdio_reset(enum SDIO_POWER_STATE state);
SDIO_CARD sdio_open(void);
int sdio_command(uint32_t cmd, uint32_t arg);
int sdio_readblock(SDIO_CARD, uint32_t lba, uint8_t *buf);
int sdio_writeblock(SDIO_CARD, uint32_t lba, uint8_t *buf);
int sdio_status(SDIO_CARD);
void sdio_print_log(int console, int number_of_entries);
const char *sdio_errmsg(int err);
uint32_t sdio_bit_slice(uint32_t a[], int bits, int msb, int lsb);

/*
 * Some 'curses' like functions that come in handy.
 */

enum TEXT_COLOR { DEFAULT, RED, YELLOW, WHITE, GREEN, BLUE };

/* Prototypes */
void move_cursor(int channel, int row, int col);
void clear_screen(int channel);
void clear_eol(int channel);
void text_color(int channel, enum TEXT_COLOR color);

/*
 * UART Functions
 */

/* All pins that can be a TX or RX pin for a USART or a UART 
        PA0,    // UART4_TX
        PA1,    // UART4_RX
        PA2,    // USART2_TX
        PA3,    // USART2_RX
        PA9,    // USART1_TX
        PA10,   // USART1_RX
        PB6,    // USART1_TX
        PB7,    // USART1_RX
        PB10,   // USART3_TX
        PB11,   // USART3_RX
        PC6,    // USART6_TX
        PC7,    // USART6_RX
        PC10,   // USART3_TX, UART4_TX (these default to USART3)
        PC11,   // USART3_RX, UART4_RX,
        PC12,   // USART5_TX,
        PD2,    // USART5_RX,
        PD5,    // USART2_TX
        PD6,    // USART2_RX
        PD8,    // USART3_TX
        PD9,    // USART3_RX
        PE0,    // UART8_RX
        PE1,    // UART8_TX
        PE7,    // UART7_RX
        PE8,    // UART7_TX
        PF6,    // UART7_RX
        PF7,    // UART7_TX
        PG9,    // USART6_RX
        PG14,   // USART6_TX
*/

/* Defines for the putnum function */
#define FMT_SIZE_MASK      0xf << 0
#define FMT_SIZE_BYTE       1
#define FMT_SIZE_WORD       2
#define FMT_SIZE_LONG       4
// #define FMT_SIZE_LONGLONG    8       // reserved
#define FMT_BASE_10         0 << 5
#define FMT_BASE_2          1 << 5
#define FMT_BASE_8          2 << 5
#define FMT_BASE_16         3 << 5
#define FMT_BASE_MASK       3 << 5
#define FMT_SIGNED          1 << 7
#define FMT_LEADING_ZERO    1 << 8
#define FMT_ALTERNATE_FORM  1 << 9
#define FMT_LEFT_ADJUST     1 << 10 
#define FMT_NEWLINE         1 << 11
#define FMT_DECIMAL         FMT_BASE_10 | FMT_ALTERNATE_FORM | FMT_SIGNED
#define FMT_HEX_BYTE        FMT_BASE_16 | FMT_LEADING_ZERO | FMT_SIZE_BYTE
#define FMT_HEX_WORD        FMT_BASE_16 | FMT_LEADING_ZERO | FMT_SIZE_WORD
#define FMT_HEX_LONG        FMT_BASE_16 | FMT_LEADING_ZERO | FMT_SIZE_LONG
#define FMT_BINARY_BYTE     FMT_BASE_2 | FMT_LEADING_ZERO | FMT_SIZE_BYTE
#define FMT_BINARY_WORD     FMT_BASE_2 | FMT_LEADING_ZERO | FMT_SIZE_WORD
#define FMT_BINARY_LONG     FMT_BASE_2 | FMT_LEADING_ZERO | FMT_SIZE_LONG
#define FMT_HEX_CONSTANT    FMT_BASE_16 | FMT_ALTERNATE_FORM

/* Formatting discussion :
 * So the interesting thing here is that when you try to format -1 as
 * a byte it shows up as FFFFFFFF rather than FF. So to get that behavior
 * we're going to cut off the number if its all 1 bits.
 *
 * To address the above and other issues, we add the 'size' flag which
 * is the natural size of the number.
 */

/* Prototypes */
int uart_init(enum GPIO_PORT_PIN tx, enum GPIO_PORT_PIN rx, int baudrate);
void uart_setbaudrate(int ud, int baudrate);
char uart_getc(int channel, int wait);
void uart_putc(int channel, char c);
void uart_puts(int channel, const char *s);
char *uart_gets(int channel, char *s, int len);
void uart_putnum(int channel, uint16_t fmt, uint32_t num);
uint32_t uart_getnum(int channel);
void ntoa(uint32_t val, uint16_t fmt, char *buf);
uint32_t aton(char *buf);

/*
 * Graphics library functions
 */

/* 
 * A simple port of the AdaFruit minimal graphics code to my
 * demo code.
 */

#define swap(a, b) { int16_t t = a; a = b; b = t; }

void gfx_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void gfx_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void gfx_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void gfx_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_fillScreen(uint16_t color);

void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color);
void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_init(int width, int height, 
    void (*write_pixel)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t));

void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color);
void gfx_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);
void gfx_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);
void gfx_drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);
void gfx_fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);
void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t color);
void gfx_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size);
void gfx_setCursor(int16_t x, int16_t y);
void gfx_setTextColor(uint16_t c, uint16_t bg);
void gfx_setTextSize(uint8_t s);
void gfx_setTextWrap(uint8_t w);
void gfx_setRotation(uint8_t r);
void gfx_puts(char *);
void gfx_write(uint8_t);

uint16_t gfx_height(void);
uint16_t gfx_width(void);
uint8_t gfx_getRotation(void);

struct gfx_state {
    int16_t width, height; /* Actual screen sizes */
    int16_t _width, _height, cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    void (*pixel_writer)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
    uint8_t textsize, rotation;
    uint8_t wrap;
};

extern struct gfx_state __gfx_state;

#define GFX_COLOR_WHITE          0xFFFF
#define GFX_COLOR_BLACK          0x0000
#define GFX_COLOR_GREY           0xF7DE
#define GFX_COLOR_BLUE           0x001F
#define GFX_COLOR_BLUE2          0x051F
#define GFX_COLOR_RED            0xF800
#define GFX_COLOR_MAGENTA        0xF81F
#define GFX_COLOR_GREEN          0x07E0
#define GFX_COLOR_CYAN           0x7FFF
#define GFX_COLOR_YELLOW         0xFFE0

/*
 * GPIO Functions 
 */

enum GPIO_PIN_ATTR { GPIO_CLOCK, GPIO_BASE, GPIO_BIT };

void gpio_enable_clock(enum GPIO_PORT_PIN pin);
#define gpio_bit(pin)   (1 << ((uint32_t)(pin) % 16))
/*
 * I'm on the fence about using GPIOA here from the 
 * libopencm3/stm32/gpio.h file.
 */
#define gpio_base(pin) (GPIOA + ((((uint32_t)(pin) >> 4) & 0xf) << 10))

#define set_gpio(pin) gpio_set(gpio_base(pin), gpio_bit(pin))
#define clear_gpio(pin) gpio_clear(gpio_base(pin), gpio_bit(pin))
#define toggle_gpio(pin) gpio_toggle(gpio_base(pin), gpio_bit(pin))

#endif // cpa.h
