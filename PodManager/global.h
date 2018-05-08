#include "music.h"

#include "track.h"
#include "title.h"

/*
#include "bmp/tiles.h"

#include "bmp/shot1.h"
#include "bmp/shot2.h"

struct {
    AnimHeader header;
    AnimFrameWB f;
} const titleAnim PROGMEM = {
    {
	ANIM_WHITE,
	1, 0
    },
    { title_comp_a }
};


struct {
    AnimHeader header;
    AnimFrameWXY f[2];
} const shot PROGMEM = {
    {
	ANIM_WHITE |
	ANIM_INVERT |
	ANIM_OFFSET,
	1, 0,
	8, 8
    },
    {
//	{ shot1_comp_a, -4, -4 },
	{ shot2_comp_a, -4, -4 }
    }
};
*/

uint16_t seed, seedSequence;

uint16_t NOISE( uint8_t x, uint8_t y, uint8_t z ){
    return ((SIN( (uint16_t(((y)+seed)/z)*13) + (uint16_t(((x)+seed)/z)*78) )*289&0xFF)*z);
}

int8_t random(int8_t min, int8_t max){
    return (SIN( (arduboy.frameCount*13+(seedSequence++)*79)*seed )*23789&0x3FF>>2 ) % (max-min) + min;
}

uint8_t noise3( uint8_t x, uint8_t y, uint8_t a, uint8_t b ){
    int16_t acc = 0;// 0x80;
    acc += NOISE( x, y, 4 );
    acc += NOISE( x, y, 2 );
    acc += NOISE( x, y, 1 );

    acc >>= 4;

    if( acc > b ) return 2;
    if( acc > a ) return 1;
    return 0;
    
}

uint8_t noise1( uint8_t x, uint8_t y, uint8_t a ){
    int16_t acc = 0;// 0x80;
    acc += NOISE( x, y, 4 );
    acc += NOISE( x, y, 2 );
    acc += NOISE( x, y, 1 );

    acc >>= 4;

    if( acc > a ) return 1;
    return 0;
    
}

typedef uint8_t (*PointCB)( uint8_t x, uint8_t y );

struct TileWindow {
    const unsigned char *tileset;
    uint8_t matrix[81];
    int16_t tx, ty;
    int16_t x, y;
    int8_t speedX, speedY;
    PointCB point;

    void clear(){
	for( uint8_t i=0; i<81; ++i )
	    matrix[i] = 0xFF;
    }
    
    void init( const unsigned char * tileset, PointCB p ){
	point = p;
	this->tileset = tileset;
	tx = ty = x = y = 0;
	speedX = speedY = 0;
	clear();
    }

    void shift( int8_t x, int8_t y ){
	if( x > 8 || y > 8 || x < -8 || y < -8 ){
	    clear();
	    return;
	}

	int8_t sy = 0, ey = 9, iy = 1, by = 9;
	int8_t sx = 0, ex = 9, ix = 1, bx = 9;
	if( y < 0 ){
	    iy = -1;
	    ey = -1;
	    sy = 8;
	    by = -1;
	}
	if( x < 0 ){
	    ix = -1;
	    ex = -1;
	    sx = 8;
	    bx = -1;
	}
	ey -= y;
	ex -= x;

	int8_t y9 = y*9;
  
	for( int8_t cy=sy; cy!=ey; cy += iy ){
	    int8_t cy9 = cy * 9;
	    for( int8_t cx=sx; cx!=ex; cx += ix )
		matrix[cy9+cx] = matrix[cy9+y9+cx+x];
	    for( int8_t cx=ex; cx!=bx; cx += ix )
		matrix[cy9+cx] = 0xFF;
	}
	
	for( int8_t cy=ey; cy!=by; cy += iy ){
	    int8_t cy9 = cy * 9;
	    for( int8_t cx=sx; cx!=bx; cx += ix )
		matrix[cy9+cx] = 0xFF;
	}
	
    }

    void tileToScreen( int16_t &x, int16_t &y ){
	x = x - this->tx;
	y = y - this->ty;
	x <<= 4;
	y <<= 4;
	x += this->x & 0xF;
	y += this->y & 0xF;
    }

    void render( ){
	const uint8_t size = 16;

	int16_t x = this->x + speedX;
	int16_t y = this->y + speedY;
	
	int8_t xL = x & 0xF;
	int8_t yL = y & 0xF;
	uint8_t xH = -x >> 4;
	uint8_t yH = -y >> 4;

	if( xL ) xH++;
	xL -= size;

	if( yL ) yH++;
	yL -= size;


	int8_t xd = xH - this->tx;
	int8_t yd = yH - this->ty;
	
	this->tx = xH;
	this->ty = yH;
	this->x = x;
	this->y = y;

	if( xd || yd )
	    shift( xd, yd );

	for( uint8_t ry=0; ry<5; ++ry ){
	    uint8_t ry9 = ry * 9;
	    int8_t screenY = ry*size+yL+cameraOffsetY;
	    for( uint8_t rx=0; rx<9; ++rx ){
		uint8_t i = ry9+rx;
		uint8_t tile = matrix[i];
		
		if( tile == 0xFF )
		    tile = matrix[i] = point(rx+xH, yH+ry);

		if( tile != 0xFF ){
		    Sprites::drawBitmap(
			rx*size+xL+cameraOffsetX,
			screenY,
			// (const uint8_tp) pgm_read_word( tileset+tile ),
			tileset+tile*32,
			NULL,
			16,16,
			2
			);
		}
		
	    }
	}
    }
};

Delayed<5> after;

State nextState;
uint8_t transitionColor;
void changeState( State ns, uint8_t tc ){
    nextState = ns;
    state = State::Transition;
    clearScreen = CLEAR_NONE;
    transitionColor = tc;
}

#include "point.hpp"
