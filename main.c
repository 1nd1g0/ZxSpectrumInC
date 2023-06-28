// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /main.c
* 
* INIT AND MAIN LOOP
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No (u)int##_t support, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
* CONTROLS:
*   Q,A,O,P, = move
*   I,K = change radius
*   Break/Space = clear screen
*
*****************************************************/
//#define DEBUG //Press 's' to singlestep, any key to slomo

#include "zxspectrum.h"
#include "circles.h"
#include "zxspectrum.c"
#include "circles.c"
void test_pixels_fill();

void main (void) {

  uint8_t x, y, r ;
  int8_t dx,dy;
  x=SCREEN_WIDTH/2;
  y=SCREEN_HEIGHT/2;
  r=32;
  
  __asm ei __endasm; /* HWMODE: interrupts disabled */  
  BORDER = Blue;
  screen_clear(0);
  attrs_clear(BW);
  
  test_pixels_fill ();
  
  dx=0; dy=0;
  while (1) {
    circle_paint (x,y,r);
    x+=(dx>>4);y+=(dy>>4);
    /* keyboard control */
    if (key(ROWANY,KEYANY)) {
      dx+=(((key(ROWYP,KEYP))&&(dx<64))<<4);
      dx-=(((key(ROWYP,KEYO))&&(dx>-64))<<4);
      dy+=(((key(ROWAG,KEYA))&&(dy<64))<<4);
      dy-=(((key(ROWQT,KEYQ))&&(dy>-64))<<4);
      r+=key(ROWYP,KEYI)&&r<128;
      r-=key(ROWHEN,KEYK)&&r>0;
      if (key(ROWBSP,KEYSP)) screen_clear(0);
    } else {
    /* friction */
      dx-=(dx>0)-(dx<0); /* TRICK: sign extraction */
      dy-=(dy>0)-(dy<0);
    }
  }  
}

void test_pixels_fill(){
 
  uint8_t j;
  
  attrs_checkboard();
  
  for (j=0;j<191;j++) {
    pixels_fill ( 32+j, j, j );
  }
  
}
