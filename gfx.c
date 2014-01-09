/*
 * Copyright (c) 2013 - 2014 Chuck McManis (cmcmanis@mcmanis.com)
 *
 * All Rights Reserved.
 *
 * Some Graphics functions, I was inspired by trying to port the
 * Adafruit GFX class but ended up pretty much re-writing it all
 * in C.
 */

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "cpa.h"
#include "font-7x12.c"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

struct gfx_state __gfx_state;

void
gfx_drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    (__gfx_state.pixel_writer)(x, y, color, __gfx_state._width, 
                                            __gfx_state._height);
}

#define true 1

/* 
 * Initialize the graphics library by telling it how large
 * the display is and passing a pointer to a function which
 * can write a pixel.
 */
void
gfx_init(int width, int height, 
         void (*write_pixel)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)) {
    __gfx_state._width    = width;
    __gfx_state.width    = width;
    __gfx_state.pixel_writer = write_pixel;
    __gfx_state._height   = height;
    __gfx_state.height   = height;
    __gfx_state.rotation  = 0;
    __gfx_state.cursor_y  = __gfx_state.cursor_x    = 0;
    __gfx_state.textsize  = 1;
    __gfx_state.textcolor = __gfx_state.textbgcolor = 0xFFFF;
    __gfx_state.wrap      = true;
}

// Draw a circle outline
void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    gfx_drawPixel(x0  , y0+r, color);
    gfx_drawPixel(x0  , y0-r, color);
    gfx_drawPixel(x0+r, y0  , color);
    gfx_drawPixel(x0-r, y0  , color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
  
        gfx_drawPixel(x0 + x, y0 + y, color);
        gfx_drawPixel(x0 - x, y0 + y, color);
        gfx_drawPixel(x0 + x, y0 - y, color);
        gfx_drawPixel(x0 - x, y0 - y, color);
        gfx_drawPixel(x0 + y, y0 + x, color);
        gfx_drawPixel(x0 - y, y0 + x, color);
        gfx_drawPixel(x0 + y, y0 - x, color);
        gfx_drawPixel(x0 - y, y0 - x, color);
    }
}

void gfx_drawCircleHelper( int16_t x0, int16_t y0,
               int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) {
            gfx_drawPixel(x0 + x, y0 + y, color);
            gfx_drawPixel(x0 + y, y0 + x, color);
        } 
        if (cornername & 0x2) {
            gfx_drawPixel(x0 + x, y0 - y, color);
            gfx_drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            gfx_drawPixel(x0 - y, y0 + x, color);
            gfx_drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            gfx_drawPixel(x0 - y, y0 - x, color);
            gfx_drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r,
			      uint16_t color) {
    gfx_drawFastVLine(x0, y0-r, 2*r+1, color);
    gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
    uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
    
        if (cornername & 0x1) {
            gfx_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            gfx_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) {
            gfx_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            gfx_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

// Bresenham's algorithm - thx wikpedia
void gfx_drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      gfx_drawPixel(y0, x0, color);
    } else {
      gfx_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Draw a rectangle
void gfx_drawRect(int16_t x, int16_t y,
			    int16_t w, int16_t h,
			    uint16_t color) {
  gfx_drawFastHLine(x, y, w, color);
  gfx_drawFastHLine(x, y+h-1, w, color);
  gfx_drawFastVLine(x, y, h, color);
  gfx_drawFastVLine(x+w-1, y, h, color);
}

void gfx_drawFastVLine(int16_t x, int16_t y,
				 int16_t h, uint16_t color) {
  // Update in subclasses if desired!
  gfx_drawLine(x, y, x, y+h-1, color);
}

void gfx_drawFastHLine(int16_t x, int16_t y,
				 int16_t w, uint16_t color) {
  // Update in subclasses if desired!
  gfx_drawLine(x, y, x+w-1, y, color);
}

void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
			    uint16_t color) {
  // Update in subclasses if desired!
  int16_t i;
  for (i=x; i<x+w; i++) {
    gfx_drawFastVLine(i, y, h, color);
  }
}

void gfx_fillScreen(uint16_t color) {
  gfx_fillRect(0, 0, __gfx_state._width, __gfx_state._height, color);
}

// Draw a rounded rectangle
void gfx_drawRoundRect(int16_t x, int16_t y, int16_t w,
  int16_t h, int16_t r, uint16_t color) {
  // smarter version
  gfx_drawFastHLine(x+r  , y    , w-2*r, color); // Top
  gfx_drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
  gfx_drawFastVLine(x    , y+r  , h-2*r, color); // Left
  gfx_drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  gfx_drawCircleHelper(x+r    , y+r    , r, 1, color);
  gfx_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  gfx_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  gfx_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

// Fill a rounded rectangle
void gfx_fillRoundRect(int16_t x, int16_t y, int16_t w,
				 int16_t h, int16_t r, uint16_t color) {
  // smarter version
  gfx_fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  gfx_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  gfx_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

// Draw a triangle
void gfx_drawTriangle(int16_t x0, int16_t y0,
				int16_t x1, int16_t y1,
				int16_t x2, int16_t y2, uint16_t color) {
  gfx_drawLine(x0, y0, x1, y1, color);
  gfx_drawLine(x1, y1, x2, y2, color);
  gfx_drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void gfx_fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    gfx_drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    gfx_drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    gfx_drawFastHLine(a, y, b-a+1, color);
  }
}

void gfx_drawBitmap(int16_t x, int16_t y,
			      const uint8_t *bitmap, int16_t w, int16_t h,
			      uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
	gfx_drawPixel(x+i, y+j, color);
      }
    }
  }
}

void gfx_write(uint8_t c) {
  if (c == '\n') {
    __gfx_state.cursor_y += __gfx_state.textsize*12;
    __gfx_state.cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    gfx_drawChar(__gfx_state.cursor_x, __gfx_state.cursor_y, 
                c, __gfx_state.textcolor, __gfx_state.textbgcolor,
                 __gfx_state.textsize);
    __gfx_state.cursor_x += __gfx_state.textsize*8;
    if (__gfx_state.wrap && (__gfx_state.cursor_x > (__gfx_state._width - __gfx_state.textsize*8))) {
      __gfx_state.cursor_y += __gfx_state.textsize*12;
      __gfx_state.cursor_x = 0;
    }
  }
}

void gfx_puts(char *s) {
    while (*s) {
        gfx_write(*s);
        s++;
    }
}

// Draw a character
/* XXX decide large or small character, and then run the
 * machinery appropriately.
 */
void gfx_drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size) {

  int8_t i, j, line;
  int8_t descender;
  unsigned const char *glyph;

  glyph = &mcm_font[(c & 0x7f) * 9];
  if((x >= __gfx_state._width)            || // Clip right
     (y >= __gfx_state._height)           || // Clip bottom
     ((x + 9 * size) < 0) || // Clip left
     ((y + 12 * size) < 0))   // Clip top
    return;

  descender = (*glyph & 0x80) != 0;

  for (i=0; i<12; i++ ) {
    line = 0x00;
    if ( descender ) {
        if (i > 2) {
          line = *(glyph + (i - 3));
        }
    } else {
        if (i < 9) {
            line = *(glyph + i);
        }
    }
    line &= 0x7f;
    for (j = 0; j<8; j++) {
      if (line & 0x80) {
        if (size == 1) // default size
          gfx_drawPixel(x+j, y+i, color);
        else {  // big size
          gfx_fillRect(x+(j*size), y+(i*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          gfx_drawPixel(x+j, y+i, bg);
        else {  // big size
          gfx_fillRect(x+j*size, y+i*size, size, size, bg);
        }
      }
      line <<= 1;
    }
  }
}

void gfx_setCursor(int16_t x, int16_t y) {
    __gfx_state.cursor_x = x;
    __gfx_state.cursor_y = y;
}

void gfx_setTextSize(uint8_t s) {
    __gfx_state.textsize = (s > 0) ? s : 1;
}

void gfx_setTextColor(uint16_t c, uint16_t b) {
    __gfx_state.textcolor   = c;
    __gfx_state.textbgcolor = b; 
}

void gfx_setTextWrap(uint8_t w) {
    __gfx_state.wrap = w;
}

uint8_t gfx_getRotation(void) {
    return __gfx_state.rotation;
}

void gfx_setRotation(uint8_t x) {
    __gfx_state.rotation = (x & 3);
    switch(__gfx_state.rotation) {
        case 0:
        case 2:
            __gfx_state._width  = __gfx_state.width;
            __gfx_state._height = __gfx_state.height;
            break;
        case 1:
        case 3:
            __gfx_state._width  = __gfx_state.height;
            __gfx_state._height = __gfx_state.width;
            break;
    }
}

// Return the size of the display (per current rotation)
uint16_t gfx_width(void) {
    return __gfx_state._width;
}
 
uint16_t gfx_height(void) {
    return __gfx_state._height;
}

