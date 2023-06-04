/****************************************************
* (c)2023 1nd1g0
* Fast vector graphics test for ZX Spectrum 48K
* originally developed for SDCC's C89'esque style
* with HTC in mind, that's why there is no uint8_t etc.
*
* CONTROLS: Q,A,O,P,I,K,Break/Space
*
* License: GPLv3
*
*****************************************************/
//Z80 main data types
#define word unsigned short
#define byte unsigned char
#define sbyte signed char

//BORDER HW
  __sfr __at (0xFFFE) BORDER;
 static const enum {Black,Blue,Red,Magenta,Green,Cyan,Yellow,White};

//VRAM HW
  //Colour attribute pallette without flash bit variants.
  //Single capital letters left to right encode bits:
  //Brightness + ink color code letter + paper letter.
  //blacK,Blue,Red,Magenta,Green,Cyan,Yellow,White
  enum {KK,KB,KR,KM,KG,KC,KY,KW,
        BK,BB,BR,BM,BG,BC,BY,BW,
        RK,RB,RR,RM,RG,RC,RY,RW,
        MK,MB,MR,MM,MG,MC,MY,MW,
        GK,GB,GR,GM,GG,GC,GY,GW,
        CK,CB,CR,CM,CG,CC,CY,CW,
        YK,YB,YR,YM,YG,YC,YY,YW,
        WK,WB,WR,WM,WG,WC,WY,WW,
        BKK,BKB,BKR,BKM,BKG,BKC,BKY,BKW,
        BBK,BBB,BBR,BBM,BBG,BBC,BBY,BBW,
        BRK,BRB,BRR,BRM,BRG,BRC,BRY,BRW,
        BMK,BMB,BMR,BMM,BMG,BMC,BMY,BMW,
        BGK,BGB,BGR,BGM,BGG,BGC,BGY,BGW,
        BCK,BCB,BCR,BCM,BCG,BCC,BCY,BCW,
        BYK,BYB,BYR,BYM,BYG,BYC,BYY,BYW,
        BWK,BWB,BWR,BWM,BWG,BWC,BWY,BWW
        };//colours
  byte __at (0x4000) SCREEN[192][32];
  byte __at (0x5800) ATTRS[24][32];
  inline byte correctscry(byte y){
    return (y & 0xc0)|((y & 7)<<3)|((y & 0x38)>>3);
  }

//BITMAPS
  const byte PointsR [8] = {0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFF};
  const byte PointsL [8] = {0xFF,0x7F,0x3F,0x1F,0xF,7,3,1};

//KEYBOARD HW
  inline byte key(byte row,byte mask){
    return (~row&mask)>0;
  }
  __sfr __banked __at (0x00FE) ROWANY;
  enum {KEYANY=0x1F};
  __sfr __banked __at (0xF7FE) ROW15;
  enum {KEY5=0x10,KEY4=8,KEY3=4,KEY2=2,KEY1=1};
  __sfr __banked __at (0xEFFE) ROW60;
  enum {KEY6=0x10,KEY7=8,KEY8=4,KEY9=2,KEY0=1};
  __sfr __banked __at (0xFBFE) ROWQT;
  enum {KEYT=0x10,KEYR=8,KEYE=4,KEYW=2,KEYQ=1};
  __sfr __banked __at (0xDFFE) ROWYP;
  enum {KEYY=0x10,KEYU=8,KEYI=4,KEYO=2,KEYP=1};
  __sfr __banked __at (0xFDFE) ROWAG;
  enum {KEYG=0x10,KEYF=8,KEYD=4,ROW=2,KEYA=1};
  __sfr __banked __at (0xBFFE) ROWHEN;
  enum {KEYH=0x10,KEYJ=8,KEYK=4,KEYL=2,KEYEN=1};
  __sfr __banked __at (0xFEFE) ROWSHV;
  enum {KEYV=0x10,KEYC=8,KEYX=4,KEYZ=2,ROWH=1};
  __sfr __banked __at (0x7FFE) ROWBSP;
  enum {KEYB=0x10,KEYN=8,KEYM=4,KEYSY=2,KEYSP=1};


byte* line_fill(byte* addr, byte count, byte mask){
  register byte m = mask;
  register byte* a = addr;
  switch(count){//TRICK:Duff's device in action
    case 32: *a-- = m;case 31: *a-- = m;case 30: *a-- = m;case 29: *a-- = m;
    case 28: *a-- = m;case 27: *a-- = m;case 26: *a-- = m;case 25: *a-- = m;
    case 24: *a-- = m;case 23: *a-- = m;case 22: *a-- = m;case 21: *a-- = m;
    case 20: *a-- = m;case 19: *a-- = m;case 18: *a-- = m;case 17: *a-- = m;
    case 16: *a-- = m;case 15: *a-- = m;case 14: *a-- = m;case 13: *a-- = m;
    case 12: *a-- = m;case 11: *a-- = m;case 10: *a-- = m;case 9: *a-- = m;
    case 8: *a-- = m;case 7: *a-- = m;case 6: *a-- = m;case 5: *a-- = m;
    case 4: *a-- = m;case 3: *a-- = m;case 2: *a-- = m;case 1: *a-- = m;
  }
  return a;
}

inline void pset (byte x, byte y, byte dx, sbyte dy){
  byte bytey = (byte)(y+dy);
BORDER=Red;  
  if (bytey<=191){//clip on screen bottom
    byte scry = correctscry(bytey);//common y of the line
    byte scrx = (x+dx);//right coordinate
    byte bytex = (scrx>>3);//rounded to byte
    byte mscrx = (x-dx);//mirrored (left) coordinate
    byte mbytex = (mscrx>>3);
    byte bytedx = (bytex - mbytex)&31;//visible bytes to fill
    byte* psetadr; byte* psetradr; byte* psetladr;
BORDER=Green;
    psetadr = &SCREEN [scry][0];
    psetradr = psetadr + bytex;
    *psetradr-- = PointsR[scrx&7];
    *psetradr = 0xFF;
    psetladr = psetadr + mbytex;   
    *psetladr = PointsL[mscrx&7];
BORDER=Blue;
    }
  
  }

void circle (byte cx, byte cy, byte r){
  byte x=0,y=r,p=r;
  
  while (x<y){
    ////RIGHT HALF
      pset(cx,cy,x,y);
      pset(cx,cy,y,x);
      pset(cx,cy,x,-y);
      pset(cx,cy,y,-x);
   
    if (p>y) {
      y--;
      p-=y;
    }
    x++;
    p+=x;
    
  }
}

void cls (byte fill_mask, byte fill_colors){
  byte* i=&ATTRS[23][31];
  while (i!=&ATTRS[0][0]-1){
    i=line_fill(i,32,fill_colors);
  }
  
  while (i!=(&SCREEN[0][0])-1) {
    i=line_fill(i,32,fill_mask);
  }  
}

void main () {
  byte x=128,y=96,r=95;
  sbyte dx=0,dy=0;
  
  __asm di __endasm;//INTERRUPTS DISABLED  
  cls(0xFF,KW);
  while (1){
    circle (x,y,r);
    x+=(dx>>4);y+=(dy>>4);
    ////KEYBOARD CONTROL
    if (key(ROWANY,KEYANY)) {
      dx+=(((key(ROWYP,KEYP))&&(dx<64))<<3);
      dx-=(((key(ROWYP,KEYO))&&(dx>-64))<<3);
      dy+=(((key(ROWAG,KEYA))&&(dy<64))<<3);
      dy-=(((key(ROWQT,KEYQ))&&(dy>-64))<<3);
      r+=key(ROWYP,KEYI)&&r<128;
      r-=key(ROWHEN,KEYK)&&r>0;
      if (key(ROWBSP,KEYSP)) cls(0xAA,KW);
    ////FRICTION
    } else {
      dx-=(dx>0)-(dx<0);//TRICK:sign extraction
      dy-=(dy>0)-(dy<0);
    }
  }  
}
