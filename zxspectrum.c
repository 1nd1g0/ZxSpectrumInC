// SPDX-License-Identifier: GPL-3.0
/****************************************************
* /zxspectrum.c
*
* ZX SPECTRUM 48K HARDWARE ABSTRACTION LAYER
*
* (copyleft)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
*
* Targets SDCC, being ported to C89 compilers, so it
* ASSUMES: No external code, TRUE = 1.
*   short = int16, char = int8, >> duplicates sign bit
*
*****************************************************/

//#link "zxspectrum.c"

/* VRAM HW */
  inline uint8_t ytolines(uint8_t y){
    return (y & 0xc0)|((y & 7)<<3)|((y & 0x38)>>3);
  }

/* BIT TO BYTE BITMAPS */
  const uint8_t SolidPixelTailRight [8] = {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
  const uint8_t SolidPixelTailLeft  [8] = {0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01};
  const uint8_t SolidPixelTailLeftM [8] = {0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF};

/* KEYBOARD HW */
  inline uint8_t key(uint8_t row,uint8_t mask){
    return (~row & mask) > 0;
  }

  void debug_step_on_key_s(void){
    while (!key(ROW_ANY,KEY_ANY));
    while (key(ROW_AG,KEY_S));
     __asm ei halt __endasm;
  }

uint8_t* bytes_fill (uint8_t* addr, uint8_t count, uint8_t mask){
  register uint8_t m = mask;
  register uint8_t* a = addr;
  switch (count) { /* TRICK: Duff's device in action */
    case 32: *a-- = m;case 31: *a-- = m;case 30: *a-- = m;case 29: *a-- = m;
    case 28: *a-- = m;case 27: *a-- = m;case 26: *a-- = m;case 25: *a-- = m;
    case 24: *a-- = m;case 23: *a-- = m;case 22: *a-- = m;case 21: *a-- = m;
    case 20: *a-- = m;case 19: *a-- = m;case 18: *a-- = m;case 17: *a-- = m;
    case 16: *a-- = m;case 15: *a-- = m;case 14: *a-- = m;case 13: *a-- = m;
    case 12: *a-- = m;case 11: *a-- = m;case 10: *a-- = m;case  9: *a-- = m;
    case  8: *a-- = m;case  7: *a-- = m;case  6: *a-- = m;case  5: *a-- = m;
    case  4: *a-- = m;case  3: *a-- = m;case  2: *a-- = m;case  1: *a-- = m;
  }
  return a;
}

void pixels_fill (uint8_t rx, uint8_t y, uint8_t count) {
  uint8_t* pen_address;
  static uint8_t last_y;
  
  #ifdef DEBUG
  debug_step_on_key_s();
  #endif
  //clip...
  if ((y > SCREEN_HEIGHT-1) || (last_y == y) || (!count))
    return; //...bottom
  last_y = y;  
  if (rx < (count-1))
    count = rx + 1; //...left
  pen_address = & SCREEN [ytolines(y)] [(rx >> 3)];
  
    if ((rx & 7) >= (count - 1)){
      *pen_address = SolidPixelTailRight[(rx & 7)]
                   & SolidPixelTailLeft [(rx & 7) - (count)];
    } else {
      *pen_address = SolidPixelTailRight[(rx & 7)];
      pen_address--;
      count -= (rx & 7) + 1;
      if (count>>3)
        pen_address = bytes_fill (pen_address,count>>3,0xFF);
      *pen_address = SolidPixelTailLeftM [((count)&7)];
    }

}
  

void screen_clear (uint8_t fill_mask){
  
  uint8_t* i = & SCREEN[191][31];
  
  while (i != (& SCREEN[0][0])-1) {
    i=bytes_fill(i,32,fill_mask);
  }  
}


void attrs_clear (uint8_t fill_attribute) {
 
  uint8_t* i = & ATTRS[23][31];
  while (i != & ATTRS[0][0]-1){
    i=bytes_fill(i,32,fill_attribute);
  }
  
}

void attrs_checkboard() {
  size_t i;
  uint8_t* p;
  
  p = & ATTRS[23][31];
  
  for (i=sizeof ATTRS; i>0; i--, p--) {
    *p = *p ^ (( ((((uint16_t)p)>>5)&1)
            ^ ((     (uint8_t)p)&1))<<3);
  }

}

/* Math */
uint8_t int8_abs(int8_t s){
  uint8_t const mask = s>>((sizeof s)*8-1);
  return (s + mask) ^ mask;
}

