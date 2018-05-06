#include "Arduboy2Ex.h"

Arduboy2Ext arduboy;

#include "game.h"

/*
int32_t qmul8( int32_t A, int32_t B ){
  int32_t acc = 0;
  int32_t a = A;

  if( B&(1<<16) ) acc += A;
  
  a >>= 1; A <<= 1;
  if( B&(1<<17) ) acc += A;
  if( B&(1<<15) ) acc += a;
  
  a >>= 1; A <<= 1;
  if( B&(1<<18) ) acc += A;
  if( B&(1<<14) ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<19) ) acc += A;
  if( B&(1<<13) ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<20) ) acc += A;
  if( B&(1<<12) ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<21) ) acc += A;
  if( B&(1<<11) ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<22) ) acc += A;
  if( B&(1<<10) ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<23) ) acc += A;
  if( B&(1<<9)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<24) ) acc += A;
  if( B&(1<<8)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<25) ) acc += A;
  if( B&(1<<7)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<26) ) acc += A;
  if( B&(1<<6)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<27) ) acc += A;
  if( B&(1<<5)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<28) ) acc += A;
  if( B&(1<<4)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<29) ) acc += A;
  if( B&(1<<3)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<30) ) acc += A;
  if( B&(1<<2)  ) acc += a;

  a >>= 1; A <<= 1;
  if( B&(1<<31) ) acc += A;
  if( B&(1<<1)  ) acc += a;
  
  a >>= 1;
  if( B&(1<<0)  ) acc += a;
  
  return acc;
}
*/

void setup() {
  arduboy.boot();
  arduboy.flashlight();
  arduboy.setFrameRate(60);
}

void loop() {
  tick();
}

