// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /circles.c
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
*
*****************************************************/

#include "zxspectrum.h"
#include "zxspectrum.c"
#include "circles.h"


int main (void) {
  uint8_t x=SCREEN_WIDTH/2,y=SCREEN_HEIGHT/2,r=32;
  int8_t dx=0,dy=0;
  
  __asm di __endasm;/* HWMODE: interrupts disabled */  
  BORDER = Black;
  screen_clear(0x55,KW);
  
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
      if (key(ROWBSP,KEYSP)) screen_clear(0xAA,KW);
    } else {
    /* friction */
      dx-=(dx>0)-(dx<0); /* TRICK: sign extraction */
      dy-=(dy>0)-(dy<0);
    }
  }  
}

void circle_paint (uint8_t cx, uint8_t cy, uint8_t r) {
  uint8_t x=0,y=r,p=r;
  
  while (x<y) {
   
    if (p>y) {
      y--;
      p-=y;
    }
    x++;
    p+=x;
    
    /* right half-moon */
    bits_fill(cx,cy,x,y);
    bits_fill(cx,cy,y,x);
    bits_fill(cx,cy,x,-y);
    bits_fill(cx,cy,y,-x);   
  }
    bits_fill(cx,cy,r,0);  /* central line */
}

void bits_fill (uint8_t x, uint8_t y, uint8_t dx, int8_t dy) {
  int8_t bytes_to_fill;
  uint8_t y_in_bits = (y+dy); 
  if (y_in_bits<=SCREEN_HEIGHT-1) { /* clip at the screen bottom */
    uint8_t y_in_lines;
    register uint8_t right_x_in_bits;
    uint8_t right_x_in_bytes;
    uint8_t left_x_in_bits;
    uint8_t left_x_in_bytes;
    register uint8_t* screenaddr;
    
    
    y_in_lines = ytolines(y_in_bits);
    right_x_in_bits = (x+dx);
    if (x>(x+dx)) {
      right_x_in_bits = SCREEN_WIDTH-1;  
    } else {
        
    };
    
    right_x_in_bytes = (right_x_in_bits>>3)-1;
    left_x_in_bits = (x-dx);
    left_x_in_bytes = (left_x_in_bits>>3);
    screenaddr = &SCREEN [y_in_lines][right_x_in_bytes];
    
    if (left_x_in_bytes> right_x_in_bytes)
      right_x_in_bytes = (SCREEN_WIDTH/8)-1;
    *screenaddr = PointsR[right_x_in_bits&7];
    
    bytes_to_fill = (right_x_in_bytes - left_x_in_bytes);

    --screenaddr = bytes_fill(screenaddr,bytes_to_fill,0xAA);
    *screenaddr = PointsL[left_x_in_bits&7];
  }
  
}
