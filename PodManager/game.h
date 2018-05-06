typedef void (*StateRef)();

uint8_t previousButtonState, currentButtonState, _justPressed;
#define justPressed( x ) (_justPressed&(x))
#define isPressed( x ) (currentButtonState&(x))
void pollButtons(){
    previousButtonState = currentButtonState;
    currentButtonState = arduboy.buttonsState();
    _justPressed = (previousButtonState^currentButtonState)&currentButtonState;
}

uint8_t cameraShakeX, cameraShakeY;
int8_t cameraOffsetX, cameraOffsetY;

uint8_t clearScreen;
#define CLEAR_NONE  0x00
#define CLEAR_WHITE 0x81
#define CLEAR_BLACK 0x80
#define CLEAR_GRAY  0x82

typedef uint8_t *uint8_tp;
typedef uint16_t *uint16_tp;
typedef uint32_t *uint32_tp;
typedef int8_t *int8_tp;
typedef int16_t *int16_tp;
typedef int32_t *int32_tp;

/* */

const uint8_t sincos[64] PROGMEM = {
0x00, 0x06, 0x0c, 0x12, 0x19, 0x1f,
0x25, 0x2b, 0x31, 0x38, 0x3e, 0x44,
0x4a, 0x50, 0x56, 0x5c, 0x61, 0x67,
0x6d, 0x73, 0x78, 0x7e, 0x83, 0x88,
0x8e, 0x93, 0x98, 0x9d, 0xa2, 0xa7,
0xab, 0xb0, 0xb5, 0xb9, 0xbd, 0xc1,
0xc5, 0xc9, 0xcd, 0xd1, 0xd4, 0xd8,
0xdb, 0xde, 0xe1, 0xe4, 0xe7, 0xea,
0xec, 0xee, 0xf1, 0xf3, 0xf4, 0xf6,
0xf8, 0xf9, 0xfb, 0xfc, 0xfd, 0xfe,
0xfe, 0xff, 0xff, 0xff  
};


inline int16_t sine(unsigned char x)
{
    if (x < 64)
        return (pgm_read_byte(&sincos[x]));
    else if (x < 128)
        return (pgm_read_byte(&sincos[127-x]));
    else if (x < 192)
        return (-pgm_read_byte(&sincos[x-128]));
    else
        return (-pgm_read_byte(&sincos[255-x]));
}

typedef struct {
  
  int16_t v;
  
  operator int16_t(){
    return v;
  }
  
} fraction;

inline int16_t SIN16( int16_t x ){
  if( x < 0 )
    x += 256;
  return sine( x );
}

inline int16_t COS16( int16_t x ){
  return SIN16( x + 64 );
}

inline fraction SIN( int16_t x ){
  return { SIN16( x ) };
}

inline fraction COS( int16_t x ){
  return { COS16( x ) };
}

/* */

/* * /
const uint8_t sincos[64] PROGMEM = 
{
130,133,136,139,142,145,148,151,
154,157,160,163,166,169,172,175,
178,181,184,186,189,192,195,197,
200,202,205,207,210,212,214,217,
219,221,223,225,227,229,231,233,
234,236,238,239,241,242,243,245,
246,247,248,249,250,251,252,252,
253,253,254,254,255,255,255,255
};

unsigned char sine(unsigned char x)
{
    if (x < 64)
        return (pgm_read_byte(&sincos[x]));
    else if (x < 128)
        return (pgm_read_byte(&sincos[127-x]));
    else if (x < 192)
        return (255-pgm_read_byte(&sincos[x-128]));
    else
        return (255-pgm_read_byte(&sincos[255-x]));
}

inline int8_tS IN( int16_t x ){
  if( x < 0 )
    x += 256;
  return sine( x ) - 128;
}

inline int8_t COS( int16_t x ){
  x += 64;
  return SIN(x);
}

/* */

/* * /
const int8_t PROGMEM sincos[] = {
    0,3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,59,62,65,67,70,73,75,78,80,82,85,87,89,91,94,96,98,100,102,103,105,107,108,110,112,113,114,116,117,118,119,120,121,122,123,123,124,125,125,126,126,126,126,126,127,126,126,126,126,126,125,125,124,123,123,122,121,120,119,118,117,116,114,113,112,110,108,107,105,103,102,100,98,96,94,91,89,87,85,82,80,78,75,73,70,67,65,62,59,57,54,51,48,45,42,39,36,33,30,27,24,21,18,15,12,9,6,3,0,-3,-6,-9,-12,-15,-18,-21,-24,-27,-30,-33,-36,-39,-42,-45,-48,-51,-54,-57,-59,-62,-65,-67,-70,-73,-75,-78,-80,-82,-85,-87,-89,-91,-94,-96,-98,-100,-102,-103,-105,-107,-108,-110,-112,-113,-114,-116,-117,-118,-119,-120,-121,-122,-123,-123,-124,-125,-125,-126,-126,-126,-126,-126,-127,-126,-126,-126,-126,-126,-125,-125,-124,-123,-123,-122,-121,-120,-119,-118,-117,-116,-114,-113,-112,-110,-108,-107,-105,-103,-102,-100,-98,-96,-94,-91,-89,-87,-85,-82,-80,-78,-75,-73,-70,-67,-65,-62,-59,-57,-54,-51,-48,-45,-42,-39,-36,-33,-30,-27,-24,-21,-18,-15,-12,-9,-6,-3
};

inline int8_t SIN( int16_t x){
    if( x < 0 )
        x = 256-x;
    return pgm_read_byte(sincos+(x&0xFF));
}

inline int8_t COS( int16_t x ){
    return SIN(x+180);
}
/* */

template <int S> void memcpy_Pn( void *out, const uint8_t *src ){
    memcpy_P( out, src, S );
}
template <> void memcpy_Pn <1> ( void *out, const uint8_t *src ){
    *uint8_tp(out) = pgm_read_byte(src);
}
template <> void memcpy_Pn <2> ( void *out, const uint8_t *src ){
    *uint16_tp(out) = pgm_read_word(src);
}
template <> void memcpy_Pn <3> ( void *out, const uint8_t *src ){
    *uint16_tp(out) = pgm_read_word(src);
    *(uint8_tp(out)+2) = pgm_read_byte(src+2);
}
template <> void memcpy_Pn <4> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
}
template <> void memcpy_Pn <5> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint8_tp(out)+4) = pgm_read_byte(src+4);
}
template <> void memcpy_Pn <6> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint16_tp(out)+2) = pgm_read_word(src+4);
}
template <> void memcpy_Pn <7> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint16_tp(out)+2) = pgm_read_word(src+4);
    *(uint8_tp(out)+6) = pgm_read_byte(src+6);
}
template <> void memcpy_Pn <8> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint32_tp(out)+1) = pgm_read_dword(src+4);
}
template <> void memcpy_Pn <9> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint32_tp(out)+1) = pgm_read_dword(src+4);
    *(uint8_tp(out)+8) = pgm_read_byte(src+8);
}
template <> void memcpy_Pn <10> ( void *out, const uint8_t *src ){
    *uint32_tp(out) = pgm_read_dword(src);
    *(uint32_tp(out)+1) = pgm_read_dword(src+4);
    *(uint16_tp(out)+4) = pgm_read_word(src+8);
}

template <typename T> void pgm_read_struct( T *header, const void *src ){
    memcpy_Pn<sizeof(T)>( header, (const uint8_t *) src );
}

#define ANIM_GRAY 1
#define ANIM_LOOP 2
#define ANIM_OFFSET 4
#define ANIM_OFFSET_FEEDBACK (4|8)
#define ANIM_WHITE 16
#define ANIM_BLACK 32
#define ANIM_INVERT 64
#define ANIM_BBOX 128

#define ACTOR_HIDDEN 1
#define ACTOR_MIRROR 2
#define ACTOR_PLAY   4

struct AnimHeader {
    uint8_t flags;
    uint8_t frameCount;
    uint8_t frameTime;
    uint8_t width;
    uint8_t height;
};
struct AnimFrameW {
    const uint8_t *white;
};
struct AnimFrameWXY {
    const uint8_t *white;
    int8_t x, y;
};
struct AnimFrameWB {
    const uint8_t *white, *black;
};
struct AnimFrameWBXY {
    const uint8_t *white, *black;
    int8_t x, y;
};
struct AnimFrameWBXYWH {
    const uint8_t *white, *black;
    int8_t x, y, w, h;   
};

struct Actor;

Actor *drawQueue[ 32 ];
uint8_t queueSize;
typedef void (*CollisionCB)(Actor *other);

struct Actor {
    union{
        int16_t x;
        struct {
            uint8_t xL;
            int8_t xH;
        };
    };
    union{
        int16_t y;
        struct {
            uint8_t yL;
            int8_t yH;			
        };
    };
    
    int8_t left, right;
    int8_t top, bottom;

    int8_t targetX, targetY, tweenWeight;
    Actor *parent;
    uint8_t frame, currentFrameTime, flags, actorFlags;
    const void *animation;
    void (*_onAnimationComplete)();
    void (*_onTweenComplete)();

    Actor &init(){
	tweenWeight = 0;
	_onAnimationComplete = NULL;
	_onTweenComplete = NULL;
	parent = NULL;
	flags = actorFlags = 0;
	return *this;
    }

    Actor &onAnimationComplete( void (*cb)() ){
	_onAnimationComplete = cb;
	return *this;
    }

    Actor &onTweenComplete( void (*cb)() ){
	_onTweenComplete = cb;
	return *this;
    }

    Actor &setParent( Actor &parent ){
	this->parent = &parent;
	return *this;
    }

    Actor &setFlags( uint8_t anim, uint8_t actor ){
	this->flags = anim;
	this->actorFlags = actor;
	return *this;
    }

    Actor &setPosition( uint8_t x, uint8_t y ){
	xH = targetX = x;
	xL = 0;
	yH = targetY = y;
	yL = 0;
	tweenWeight = 0;
	return *this;
    }

    Actor &moveTo( int8_t x, int8_t y ){
	targetX = x;
	targetY = y;
	tweenWeight = 1;
	return *this;
    }

    Actor &setTweenWeight( int8_t w ){
	tweenWeight = w;
	return *this;
    }

    Actor &setAnimation( const void *_animation ){
	frame = 0;
	currentFrameTime = 0xFF;
	animation = _animation;
	return *this;
    }

    Actor &show(){
	drawQueue[ queueSize++ ] = this;
	return *this;
    }

    Actor &hide(){
		
	for( auto i=queueSize; i; --i ){
	    if( drawQueue[i-1] == this ){
		for( ; i<queueSize; ++i )
		    drawQueue[i-1] = drawQueue[i];
		queueSize--;
		return *this;
	    }
	}
		
	return *this;
    }

    void checkCollision(
	Actor *check,
	uint8_t checkStride,
	uint8_t count,
	CollisionCB cb
	){
	for( uint8_t j=0; j<count; ++j ){
	    Actor &target = *(Actor *)(((uint8_tp)check)+j*checkStride);
	    
	    if( !(target.left   >= right  ||
		  target.right  <= left   ||
		  target.top    >= bottom ||
		  target.bottom <= top) ){
		cb( &target );
	    }
		
	}
    }

    template< typename T > void checkCollision(
	T *target,
	uint8_t count,
	CollisionCB cb
	){
	Actor *check = (Actor *) target;
	checkCollision( check, sizeof(T), count, cb );
    }
    
};

typedef void (*DelayedCB)();

template <uint8_t max>
struct Delayed {
    DelayedCB cbs[max];
    uint8_t framesLeft[max];
    
    Delayed &init(){
	for( uint8_t i=0; i<max; ++i )
	    cbs[i] = NULL;
	return *this;
    }
    
    DelayedCB &frames( uint8_t c ){
	for( uint8_t i=0; i<max; ++i ){
	    if( !cbs[i] ){
		framesLeft[i] = c;
		return cbs[i];
	    }
	}
	return cbs[0];
    }
    
    void update(){
	for( uint8_t i=0; i<max; ++i ){
	    auto cb = cbs[i];
	    auto &frames = framesLeft[i];
	    if( cb ){
		frames--;
		if( frames ) continue;
		cbs[i] = NULL;
		cb();
	    }
	}
    }
    
};



void flushDrawQueue(){

    cameraOffsetX = int16_t(COS( arduboy.frameCount<<5 )) * cameraShakeX >> 9;
    cameraOffsetY = int16_t(SIN( arduboy.frameCount<<5 )) * cameraShakeY >> 9;
    if( cameraShakeX ) cameraShakeX--;
    if( cameraShakeY ) cameraShakeY--;
    
    for( uint8_t i=0; i<queueSize; ++i ){

        Actor &actor = *drawQueue[i];

	auto actorFlags = actor.actorFlags;

	if( actorFlags & ACTOR_HIDDEN )
	    continue;

	bool mirror = actorFlags & ACTOR_MIRROR;
	bool play   = actorFlags & ACTOR_PLAY;
	
	if( actor.tweenWeight ){
	    int16_t t;
	    *(uint8_tp(&t)) = 0;
	    *(uint8_tp(&t)+1) = actor.targetX;
	    actor.x -= (actor.x - t) >> actor.tweenWeight;
			
	    *(uint8_tp(&t)+1) = actor.targetY;
	    actor.y -= (actor.y - t) >> actor.tweenWeight;

	    if( actor.xH == actor.targetX && actor.yH == actor.targetY ){
		actor.tweenWeight = 0;
		if( actor._onTweenComplete )
		    (*actor._onTweenComplete)();
	    }
	}
    
        AnimHeader header;

        pgm_read_struct( &header, actor.animation );
        
        uint8_t flags = actor.flags | header.flags;

        if( play ){
            actor.currentFrameTime++;
		
            if( actor.currentFrameTime >= header.frameTime ){
                actor.frame++;
                actor.currentFrameTime = 0;
            }
        }

        if( actor.frame >= header.frameCount ){
            if( flags & ANIM_LOOP ) actor.frame = 0;
            else actor.frame = header.frameCount - 1;
	    if( actor._onAnimationComplete ){
		(*actor._onAnimationComplete)();
		flags = actor.flags | header.flags;
	    }
        }
        
        AnimFrameWBXYWH frame;
	uint8_t sizeofframe = 0;
	if( flags & ANIM_WHITE )
	    sizeofframe += sizeof(void *);
	if( flags & ANIM_BLACK )
	    sizeofframe += sizeof(void *);
	if( flags & ANIM_OFFSET )
	    sizeofframe += 2;
	if( flags & ANIM_BBOX )
	    sizeofframe += 2;

	uint8_t *addr = ((uint8_t *)actor.animation) + sizeof(header) + actor.frame*sizeofframe;

	if( flags & ANIM_WHITE ){
	    frame.white = pgm_read_word( addr );
	    addr += sizeof(void *);
	}else frame.white = NULL;
		
	if( flags & ANIM_BLACK ){
	    frame.black = pgm_read_word( addr );
	    addr += sizeof(void *);
	}else frame.black = NULL;
		
	if( flags & ANIM_OFFSET ){
	    frame.x = pgm_read_word(addr++);
	    frame.y = pgm_read_word(addr++);
	}else frame.x = frame.y = 0;

	if( flags & ANIM_BBOX ){
	    frame.w = pgm_read_word(addr++);
	    frame.h = pgm_read_word(addr++);
	}else{
	    frame.w = frame.h = 0;		
	}

        int8_t x;
        int8_t y;

	if( mirror )
	    frame.x = frame.w-frame.x;

        if( play && (flags & ANIM_OFFSET_FEEDBACK) == ANIM_OFFSET_FEEDBACK ){
            if( !actor.currentFrameTime ){
                actor.xH += frame.x;
                actor.yH += frame.y;
            }
            x = actor.xH;
            y = actor.yH;
        }else{
            x = actor.xH + frame.x;
            y = actor.yH + frame.y;
        }

	if( actor.parent ){
	    x += actor.parent->xH;
	    y += actor.parent->yH;
	}

	actor.left = x;
	actor.right = x + header.width;
	actor.top = y;
	actor.bottom = y + header.height;

	x += cameraOffsetX;
	y += cameraOffsetY;
	    
	bool icolor = flags & ANIM_INVERT;
	bool color = !icolor;
	color ^= (flags & ANIM_GRAY) & (arduboy.frameCount&1);
	
        if( frame.white )
            arduboy.drawCompressed( x, y, frame.white, color, mirror );
            
        if( frame.black )
            arduboy.drawCompressed( x, y, frame.black, icolor, mirror );

    }
    
}

enum class State {
#define STATE( NAME, VAR, INIT, UPDATE, ... ) NAME,
#include "states.h"
    MAX
	} state, prevState = State::MAX;
#undef STATE

#include "global.h"

#define STATE( NAME, VARS, INIT, UPDATE, ... )	\
    namespace ns ## NAME {			\
	void init();				\
	void update();				\
	struct Type_ ## NAME VARS;		\
    }
#include "states.h"
#undef STATE

static union {
#define STATE( NAME, VAR, INIT, UPDATE, ... ) ns ## NAME :: Type_ ## NAME NAME;
#include "states.h"  
} stateData;
#undef STATE

const StateRef stateUpdate[] PROGMEM = {
#define STATE( NAME, VAR, INIT, UPDATE, ... ) ns ## NAME :: update,
#include "states.h"
    NULL
};
#undef STATE

const StateRef stateInit[] PROGMEM = {
#define STATE( NAME, VAR, INIT, UPDATE, ... ) ns ## NAME :: init,
#include "states.h"
    NULL
};
#undef STATE

#define STATE( NAME, VAR, INIT, UPDATE, ... )		\
    namespace ns ## NAME {				\
	Type_ ## NAME & scope = ::stateData.NAME;	\
	typedef Type_ ## NAME Scope;			\
	__VA_ARGS__					\
	    void init() INIT				\
	    void update() UPDATE			\
	    }
#include "states.h"
#undef STATE

void tick(){
    if (!(arduboy.nextFrameDEV()))
	return;

    pollButtons();

    StateRef func;
    if( state != prevState ){

	queueSize = 0;
    
	func = (StateRef) pgm_read_word( stateInit + uint8_t(state) );
	if( func ) (*func)();
    
	prevState = state;

    }

    func = (StateRef) pgm_read_word( stateUpdate + uint8_t(state) );
    if( func ) (*func)();

    // flushDrawQueue();

    // arduboy.setCursor(0,0);
    // arduboy.print( arduboy.cpuLoad() );
  
    arduboy.display();

    if( clearScreen ){
	uint8_t c, ac;
	if( clearScreen == CLEAR_GRAY ){
	    c = 0b10101010;
	    if( arduboy.frameCount & 1 )
		c = ~c;
	    ac = ~c;
	}else c = ac = -(clearScreen&1);

	// local variable for screen buffer pointer,
	// which can be declared a read-write operand
	uint8_t* bPtr = arduboy.sBuffer;

	asm volatile(
	    "clr __tmp_reg__\n"
	    "loopto_gray:\n"
	    // (4x) push c/ac into screen buffer,
	    // then increment buffer position
	    "st Z+, %[color]\n"
	    "st Z+, %[acolor]\n"
	    "st Z+, %[color]\n"
	    "st Z+, %[acolor]\n"
	    // increase counter
	    "inc __tmp_reg__\n"
	    // repeat for 256 loops
	    // (until counter rolls over back to 0)
	    "brne loopto_gray\n"
	    "main_cont:\n"
	    : [color] "+d" (c),
	      [acolor] "+d" (ac),
	      "+z" (bPtr)
	    :
	    :
	    );	  
    }
	
}
