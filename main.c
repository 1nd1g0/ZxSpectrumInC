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
* ASSUMES: No external code, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
* CONTROLS:
*   Q,A,O,P, = move
*   I,K = change radius
*   Break/Space = clear screen
*   C = toggle checkboard
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
    if (key(ROW_ANY,KEY_ANY)) {
      dx+=(((key(ROW_YP,KEY_P))&&(dx<64))<<4);
      dx-=(((key(ROW_YP,KEY_O))&&(dx>-64))<<4);
      dy+=(((key(ROW_AG,KEY_A))&&(dy<64))<<4);
      dy-=(((key(ROW_QT,KEY_Q))&&(dy>-64))<<4);
      r+=key(ROW_YP,KEY_I)&&r<128;
      r-=key(ROW_HEN,KEY_K)&&r>0;
      if (key(ROW_BSP,KEY_SP))
        screen_clear(0);
      if (key(ROW_SHV,KEY_C))
          attrs_checkboard();
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
  
  for (j=0;j<SCREEN_HEIGHT;j++) {
    pixels_fill ( 32+j, j, j );
  }
  
}
