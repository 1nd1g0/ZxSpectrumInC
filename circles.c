// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /circles.c
*
* CIRCLES SPECIFIC LOGIC
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No (u)int##_t support, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
*****************************************************/
//#link "main.c"

void circle_paint (uint8_t cx, uint8_t cy, uint8_t r) {
  uint8_t x=0,y=r,p=r;
  
  while (x<y) {
    pixels_fill(cx+y,cy+x,y<<1);
    pixels_fill(cx+y,cy-x,y<<1);
    if (p>y) {
      y--;
      p-=y;
      pixels_fill(cx+x,cy+y,x<<1);
      pixels_fill(cx+x,cy-y,x<<1);
    }
    x++;
    p+=x;
  }
}

