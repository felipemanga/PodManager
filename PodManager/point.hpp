/* */
#include "FixedPoints/FixedPoints.h"

template<>
SFixed<23,8> operator *(const SFixed<23,8> & left, const SFixed<23,8> & right)
{
    auto leftInternal = left.getInternal();
    auto rightInternal = right.getInternal();
    
    if( !leftInternal || !rightInternal )
	return 0;

    int64_t r = leftInternal * rightInternal;
    uint8_t *b = reinterpret_cast<uint8_t *>(&r);
    return SFixed<23, 8>::fromInternal( *reinterpret_cast<int32_t*>(b+1) );
}

typedef SFixed<23,8> Fixed;

/*/

class Fixed {
public:

    union {
	uint32_t internal;
	struct {
	    int16_t integer;
	    uint8_t fraction;
	};
    };

    Fixed( void ) = default;
    
    Fixed( int i ){
	integer = i;
	fraction = 0;
    }
    
    static Fixed fromInternal( uint32_t i ){
	Fixed f;
	f.internal = i;
	return f;
    }

    Fixed operator -(){
	Fixed ret;
	ret.internal = -internal;
	return ret;
    }

    int16_t getInteger(){
	return internal;
    }

    Fixed &operator =( int v ){
	integer = v;
	fraction = 0;
	return *this;
    }

    Fixed &operator =( float v ){
	integer = v;
	fraction = 255*(v-integer);
	return *this;
    }

    Fixed &operator *=( const Fixed &right ){
	union {
	    uint64_t big;
	    struct {
		uint8_t pad;
		Fixed fixed;
	    } s;
	} u;
	u.big = internal;
	u.big *= right.internal;
	internal = u.s.fixed.internal;
	return *this;
    }

    Fixed &operator +=( const Fixed &other ){
	internal += other.internal;
	return *this;
    }

    Fixed &operator -=( const Fixed &other ){
	internal -= other.internal;
	return *this;
    }

    Fixed &operator ++(){
	integer++;
	return *this;
    }

    Fixed &operator --(){
	integer--;
	return *this;
    }
    
};

bool operator > (const Fixed &left, const Fixed &right ){
    return left.internal > right.internal;
}

bool operator < (const Fixed &left, const Fixed &right ){
    return left.internal < right.internal;
}

bool operator <= (const Fixed &left, const Fixed &right ){
    return left.internal <= right.internal;
}

bool operator <= (const Fixed &left, int right ){
    Fixed fright;
    fright.integer = right;
    fright.fraction = 0;
    return left <= fright;
}

bool operator == (const Fixed &left, const Fixed &right ){
    return left.internal == right.internal;
}

Fixed operator + (const Fixed &left, const Fixed &right ){
    Fixed ret;
    ret.internal = left.internal + right.internal;
    return ret;
}

Fixed operator - (const Fixed &left, const Fixed &right ){
    Fixed ret;
    ret.internal = left.internal - right.internal;
    return ret;
}

Fixed operator * (const Fixed &left, const Fixed &right){
    Fixed ret;
    if( !left.internal || !right.internal ){
	ret.internal = 0;
	return 0;
    }
    union {
	uint64_t big;
	struct {
	    uint8_t pad;
	    Fixed fixed;
	} s;
    } u;
    u.big = left.internal;
    u.big *= right.internal;
    ret.internal = u.s.fixed.internal;
    return ret;
}

Fixed operator / (const Fixed &left, const Fixed &right){
    union {
	int64_t big;
	Fixed fixed;
	struct {
	    uint8_t pad;
	    Fixed high;
	} s;
    } u;
    u.big = 0;
    u.s.high.internal = left.internal;
    u.big /= right.internal;
    return u.fixed;
}

Fixed operator + (const Fixed &left, int16_t right){
    Fixed ret;
    ret.internal = left.internal;
    ret.integer += right;
    return ret;
}

Fixed operator - (const Fixed &left, int16_t right){
    Fixed ret;
    ret.internal = left.internal;
    ret.integer -= right;
    return ret;
}

Fixed operator * (const Fixed &left, int16_t right){
    Fixed ret;
    ret.internal = left.internal * right;
    return ret;
}

/* */

Fixed COSfp( Fixed a ){
  return Fixed::fromInternal( COS( a.getInteger() ) );
}

Fixed SINfp( Fixed a ){
  return Fixed::fromInternal( SIN( a.getInteger() ) );
}


template<typename Number>
class Matrix4 {
public:
  Number a[12];

  Matrix4() = default;

  Matrix4<Number> &identity(){
    auto p = a;
    *p++ = 1; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 1; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 1;
    *p++ = 0; *p++ = 0; *p++ = 0;
    return *this;
  }

  template<typename ONumber>
  Matrix4<Number> &operator =(const Matrix4<ONumber> &o ){
    auto p = a, b=o.a;
    *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++;
    return *this;
  }


  Matrix4<Number> &setTranslation( const Number &x, const Number &y, const Number &z ){
    Number *p = a;
    *p++ = 1; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 1; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 1;
    *p++ = x; *p++ = y; *p++ = z;
    return *this;
  }

  Matrix4<Number> &translateX( const Number &x ){
    a[ 9] += a[0] * x;
    a[10] += a[1] * x;
    a[11] += a[2] * x;
    return *this;
  }
  
  Matrix4<Number> &translate( const Number &x, const Number &y, const Number &z ){
    a[ 9] = a[0] * x + a[3] * y + a[6]  * z + a[ 9];
    a[10] = a[1] * x + a[4] * y + a[7]  * z + a[10];
    a[11] = a[2] * x + a[5] * y + a[8]  * z + a[11];
    return *this;
  }
  

  Matrix4<Number> &setScale( const Number &x, const Number &y, const Number &z ){
    auto p = a;
    *p++ = x; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = y; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = z;
    *p++ = 0; *p++ = 0; *p++ = 0;
    return *this;
  }

  Matrix4<Number> &scale( const Number &x, const Number &y, const Number &z ){
    a[0] *= x; a[3] *= y; a[6]  *= z;
    a[1] *= x; a[4] *= y; a[7]  *= z;
    a[2] *= x; a[5] *= y; a[8]  *= z;
    return *this;
  }


  Matrix4<Number> &setRotation( const Number &x, const Number &y, const Number &z ){
    auto p = this->a;
    Number a = COSfp(x), b = SINfp(x);
    Number c = COSfp(y), d = SINfp(y);
    Number e = COSfp(z), f = SINfp(z);
    Number ad = a * d;
    Number bd = b * d;

    p[0] =  c * e;
    p[1] = -c * f;
    p[2] = -d;

    p[3] = -bd * e + a * f;
    p[4] =  bd * f + a * e;
    p[5] = -b * c;

    p[6] =  ad * e + b * f;
    p[7] = -ad * f + b * e;
    p[8] =  a * c;

    p[9] = p[10] = p[11] = 0;
    
    return *this;
  }

  Matrix4<Number> &setRotationX( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);

    p[0] = 1;    p[3] = 0;    p[6] =  0;
    p[1] = 0;    p[4] = c;    p[7] = -s;
    p[2] = 0;    p[5] = s;    p[8] = c;

    p[9] = p[10] = p[11] = 0;
    
    return *this;
  }


  Matrix4<Number> &rotateY( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);
    const auto &ae = this->a;
    auto &te = this->a;

    auto a11 = ae[ 0 ], a12 = ae[ 3 ], a13 = ae[ 6 ],  a14 = ae[ 9 ];
    auto a21 = ae[ 1 ], a22 = ae[ 4 ], a23 = ae[ 7 ],  a24 = ae[ 10 ];
    auto a31 = ae[ 2 ], a32 = ae[ 5 ], a33 = ae[ 8 ],  a34 = ae[ 11 ];

    te[ 0 ] = a11 * c + a13 * -s;
    te[ 3 ] = a12;
    te[ 6 ] = a11 * s + a13 * c;
    te[ 9 ] = a14;

    te[ 1 ] = a21 * c + a23 * -s;
    te[ 4 ] = a22;
    te[ 7 ] = a21 * s + a23 * c;
    te[ 10 ] = a24;

    te[ 2 ] = a31 * c + a33 * -s;
    te[ 5 ] = a32;
    te[ 8 ] = a31 * s + a33 * c;
    te[ 11 ] = a34;
    
    return *this;
  }        

  Matrix4<Number> &setRotationY( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);

    p[0] = c;    p[3] = 0;    p[6] =  s;
    p[1] = 0;    p[4] = 1;    p[7] =  0;
    p[2] =-s;    p[5] = 0;    p[8] = c;

    p[9] = p[10] = p[11] = 0;
    
    return *this;
  }    

  Matrix4<Number> &rotate( const Number &x, const Number &y, const Number &z ){
    Matrix4<Number> tmp;
    tmp.setRotation( x, y, z );
    *this *= tmp;
    return *this;
  }

  template<typename ONumber>
  Matrix4<Number> &operator *=( const Matrix4<ONumber> &o ){
    return this->mul( *this, o );
  }

  template<typename ONumber>
  Matrix4<Number> &mul( const Matrix4<ONumber> &a, const Matrix4<ONumber> &b ){
    const auto &ae = a.a;
    const auto &be = b.a;
    auto &te = this->a;

    auto a11 = ae[ 0 ], a12 = ae[ 3 ], a13 = ae[ 6 ],  a14 = ae[ 9 ];
    auto a21 = ae[ 1 ], a22 = ae[ 4 ], a23 = ae[ 7 ],  a24 = ae[ 10 ];
    auto a31 = ae[ 2 ], a32 = ae[ 5 ], a33 = ae[ 8 ],  a34 = ae[ 11 ];
    
    const auto &b11=be[ 0 ], &b12 = be[ 3 ], &b13 = be[ 6 ],  &b14 = be[ 9 ];
    const auto &b21=be[ 1 ], &b22 = be[ 4 ], &b23 = be[ 7 ],  &b24 = be[ 10 ];
    const auto &b31=be[ 2 ], &b32 = be[ 5 ], &b33 = be[ 8 ],  &b34 = be[ 11 ];

    te[ 0 ] = a11 * b11 + a12 * b21 + a13 * b31 /* + a14 * b41 */;
    te[ 3 ] = a11 * b12 + a12 * b22 + a13 * b32 /* + a14 * b42 */;
    te[ 6 ] = a11 * b13 + a12 * b23 + a13 * b33 /* + a14 * b43 */;
    te[ 9 ] = a11 * b14 + a12 * b24 + a13 * b34 + a14;// * b44;

    te[ 1 ] = a21 * b11 + a22 * b21 + a23 * b31 /* + a24 * b41 */;
    te[ 4 ] = a21 * b12 + a22 * b22 + a23 * b32 /* + a24 * b42 */;
    te[ 7 ] = a21 * b13 + a22 * b23 + a23 * b33 /* + a24 * b43 */;
    te[ 10 ] = a21 * b14 + a22 * b24 + a23 * b34 + a24;// * b44;

    te[ 2 ] = a31 * b11 + a32 * b21 + a33 * b31 /* + a34 * b41 */;
    te[ 5 ] = a31 * b12 + a32 * b22 + a33 * b32 /* + a34 * b42 */;
    te[ 8 ] = a31 * b13 + a32 * b23 + a33 * b33 /* + a34 * b43 */;
    te[ 11 ] = a31 * b14 + a32 * b24 + a33 * b34 + a34;// * b44;
    
    return *this;
  }

};


typedef Matrix4<Fixed> Matrix;

const uint8_t FLAG_SQUARE=1;
const uint8_t FLAG_HIDDEN=2;
const uint8_t FLAG_UPPER=4;
const uint8_t FLAG_EXPLODE=8;

class Point3D {
public:
  Fixed x, y, z;//, w;
  uint8_t color, size;

  Point3D() = default;

  Point3D &load( const Point3D &o ){
    pgm_read_struct( &x, &o.x );
    pgm_read_struct( &y, &o.y );
    pgm_read_struct( &z, &o.z );
    pgm_read_struct( &color, &o.color );
    pgm_read_struct( &size, &o.size );
    return *this;
  }
  
  Point3D operator -( const Point3D &o ){
    return Point3D{ x-o.x, y-o.y, z-o.z };
  }

  Point3D &operator *=( const Matrix &mat ){

    auto &e = mat.a;
    Fixed x = this->x, y = this->y, z = this->z;

    this->x = e[ 0 ] * x + e[ 3 ] * y + e[ 6 ] * z + e[ 9 ];
    this->y = e[ 1 ] * x + e[ 4 ] * y + e[ 7 ] * z + e[ 10 ];
    this->z = e[ 2 ] * x + e[ 5 ] * y + e[ 8 ] * z + e[ 11 ];
    return *this;
  }

  void render( uint8_t flags ){

    Fixed fovz = 64 / (64 + z);
      
    int16_t x = (this->x * fovz + 64).getInteger();
    int16_t y = (32 - this->y * fovz).getInteger();	   
    auto size = (this->size * fovz).getInteger();
    auto hsize = size>>1;
    auto sx = x - hsize;
    auto sy = y - hsize;
    auto ex = sx + size;
    auto ey = sy + size;
    if( sx < 0 ) sx = 0;
    if( ex > 128 ) ex = 128;
    if( sy < 0 ) sy = 0;
    if( ey > 64 ) ey = 64;
	     
    if( sx>=ex || sy>=ey )
      return;

    bool square = (size <= 4) || (flags & FLAG_SQUARE);

    if( square ){
       ey -= sy;     
       for( sx=0; sx<=hsize; ++sx ){
	   arduboy.drawSymmetricVLine( x, -sx, sy, ey, color );
       }
    }else if( flags & FLAG_UPPER ){
	arduboy.fillHalfCircleUpper( x, y, hsize, this->color );
    }else{
	arduboy.fillCircle( x, y, hsize, this->color );
    }
    
  }
  
};

typedef Point3D *Point3Dp;
typedef Point3D const *cPoint3Dp;

class Node;

typedef void (*UpdateNode)( Node & );

class Node {
public:
  Fixed x, y, z, scale;
  uint8_t rotX, rotY, rotZ;
  uint8_t vertexCount;
  cPoint3Dp mesh;
  Matrix transform;
  UpdateNode update;

  uint8_t data;
  int8_t flags, id;
  int8_t screenX, screenY;

  void init( cPoint3Dp mesh, uint8_t vc ){

    x=0;   y=0;  z=0;
    scale = 1;
    flags = 0;
    rotX=0; rotY=0; rotZ=0;
    update = NULL;
    vertexCount = vc;
    this->mesh = mesh;
    
  }

  Node &setPosition( const Fixed &nx, const Fixed &ny, const Fixed &nz ){

    x = nx; y = ny; z = nz;
    return *this;

  }
  
};

template< uint8_t pointCount, uint8_t nodeCount >
class Scene {
public:

  struct ZBufferItem {
    uint8_t nodeId;
    uint8_t pointId;
  } zBuffer[ pointCount ];
  
  Node nodeList[ nodeCount ];
  Matrix camera;

  uint8_t usedPointCount, usedNodeCount;

  uint8_t invertPerspective;
  
  void init(){

    usedPointCount = 0;
    usedNodeCount = 0;
    invertPerspective = 0;
    
    for( int i=0; i<pointCount; ++i ){
      zBuffer[i].nodeId = 0xFF;
    }
    
  }

  void moveToEnd( Node &node, uint8_t pc ){
    uint8_t w = 0;
    for( uint8_t r=0; r<usedPointCount; r++ ){
      auto &zbi = zBuffer[r];
      if( zbi.nodeId != node.id )
	zBuffer[w++] = zbi;
    }

    for( uint8_t r=0; r<pc; r++ ){
      auto &zbi = zBuffer[w++];
      zbi.nodeId = node.id;
      zbi.pointId = r;
    }

  }


  void moveToStart( Node &node, uint8_t pc ){
    uint8_t w = usedPointCount-1;
    for( uint8_t r=w; r<0xFF; r-- ){
      auto &zbi = zBuffer[r];
      if( zbi.nodeId != node.id )
	zBuffer[w--] = zbi;
    }

    for( uint8_t r=pc-1; r<0xFF; --r ){
      auto &zbi = zBuffer[w--];
      zbi.nodeId = node.id;
      zbi.pointId = r;
    }

  }
  
  Node &initNode( cPoint3Dp mesh, uint8_t pc ){
    
    Node &node = nodeList[usedNodeCount];
    node.init( mesh, pc );
    node.id = usedNodeCount;
    node.data = 0;
    node.flags = 0;
    
    for( uint8_t v=0; v<pc; v++ ){
      auto &zbi = zBuffer[usedPointCount++];
      zbi.nodeId = usedNodeCount;
      zbi.pointId = v;
    }

    usedNodeCount++;
    
    return node;
    
  }

  void update(){

    for( uint8_t i=0; i<usedNodeCount; ++i ){
      Node &node = nodeList[i];

      if( node.update )
	node.update( node );

      if( node.flags & FLAG_HIDDEN )
	continue;

      Matrix &mat = node.transform;
      mat = camera;

      mat.translate( node.x, node.y, node.z );

      //if( !node.rotX && !node.rotZ && node.rotY )
      if( node.rotY )
	  mat.rotateY( node.rotY );
      else if( node.rotZ )
	  mat.rotate( node.rotX, node.rotY, node.rotZ );

    }

    Fixed prevZ;
    uint8_t prevI;

    for( uint8_t i=0; i<usedPointCount; ++i ){
      if( zBuffer[i].nodeId == 0xFF )
	continue;
      
      auto zbi = zBuffer[i];

      Node &node = nodeList[ zbi.nodeId ];

      Point3D tmp;
	   
      tmp.load( node.mesh[ zbi.pointId ] );
      
      if( node.scale != 1 ){
	  tmp.x *= node.scale;
	  tmp.y *= node.scale;
	  tmp.z *= node.scale;
	  if( !(node.flags&FLAG_EXPLODE) )
	      tmp.size = (tmp.size * node.scale).getInteger();
      }
      
      tmp *= node.transform;

      node.screenX = tmp.x.getInteger();
      node.screenY = tmp.y.getInteger();

      if( tmp.z > 0 ){

	if( i && prevZ < tmp.z ){
	  zBuffer[i] = zBuffer[prevI];
	  zBuffer[prevI] = zbi;
	}
	prevZ = tmp.z;
	prevI = i;

	tmp.render( node.flags );
	     
      }

    }

  }

};


typedef Scene<40,11> Scene36_3;

const uint8_t JUMP_COST = 12;
const uint8_t BOOST_COST = 20;


class ShipCalc {
public:
  Fixed position, startPosition, speed, ySpeed;
  uint16_t charge, shield;
  uint16_t maxCharge, maxShield;
  Fixed acceleration;
  uint16_t chargeSpeed, shieldRegen;
  uint8_t jumping, maxJump;
  uint8_t laps, dead;
  
  void die(){
    dead = 0x7F;
  }

  void boost(){
    speed += acceleration;
    charge -= BOOST_COST;    
  }

  void jump(){
    jumping = maxJump;
    charge -= JUMP_COST;
  }

  int8_t collides( ShipCalc &other, uint8_t radius, bool adjust ){
    int16_t a = uint8_t(other.position.getInteger());
    int16_t b = uint8_t(position.getInteger());
    uint8_t ret = 1;
    int16_t delta = a - b;

    if( delta > 128 )
      delta = 256-delta;
    if( delta < -128 )
      delta = -256+delta;

    int16_t adelta = delta;
    if( delta < 0 )
      adelta = -adelta;

    if( adelta < radius ){
      if( adjust ){
	position -= delta;
	other.position += delta;
      }
      return delta < 0 ? 1 : -1;
    }
    
    return 0;
  }

} racers[3];

class ShipUpgrades {
public:
  uint8_t dspeed, jump, charge, dcharge, shield, dshield;
  
  void apply( ShipCalc &sc ){
    sc.dead = 0;

    sc.acceleration = Fixed::fromInternal(dspeed)*300;
    sc.speed = 0;
    sc.ySpeed = 0;

    sc.charge = 0;
    sc.maxCharge = charge*20;
    sc.chargeSpeed = dcharge;

    sc.shield = sc.maxShield = shield*5;
    sc.shieldRegen = dshield;

    sc.jumping = 0;
    sc.maxJump = jump;

    sc.laps = 0;
  }
  
};

// dspeed, jump, charge, dcharge, shield, dshield;
const ShipUpgrades opponents[] PROGMEM = {
  {2, 1, 1,1, 1,1},
  {1, 2, 2,1, 1,1},
  {1, 1, 2,1, 1,1},
  {2, 1, 1,1, 3,1},
  {2, 3, 1,1, 2,2},
  {1, 1, 3,1, 2,1},
  {4, 1, 1,2, 1,1},
  {4, 2, 1,1, 3,1},
  {8, 4, 1,5, 1,1},
  {4, 4, 2,6, 5,1}
};

ShipUpgrades playerUpgrades = {
  1,1, 1,1, 1,1
};

uint32_t playerMoney = 100;
uint16_t raceCount, winCount, loseCount;
