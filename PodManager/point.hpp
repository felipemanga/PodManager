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

Fixed COSfp( Fixed a ){
  return Fixed::fromInternal( COS( a.getInteger() ) );
}

Fixed SINfp( Fixed a ){
  return Fixed::fromInternal( SIN( a.getInteger() ) );
}


template<typename Number>
class Matrix4 {
public:
  Number a[16];

  Matrix4() = default;

  Matrix4<Number> &identity(){
    auto p = a;
    *p++ = 1; *p++ = 0; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 1; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 1; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 1;
    return *this;
  }

  template<typename ONumber>
  Matrix4<Number> &operator =(const Matrix4<ONumber> &o ){
    auto p = a, b=o.a;
    *p++ = *b++; *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++; *p++ = *b++;
    *p++ = *b++; *p++ = *b++; *p++ = *b++; *p++ = *b++;
    return *this;
  }


  Matrix4<Number> &setTranslation( const Number &x, const Number &y, const Number &z ){
    Number *p = a;
    *p++ = 1; *p++ = 0; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 1; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 1; *p++ = 0;
    *p++ = x; *p++ = y; *p++ = z; *p++ = 1;
    return *this;
  }

  Matrix4<Number> &translate( const Number &x, const Number &y, const Number &z ){
    a[12] = a[0] * x + a[4] * y + a[8]  * z + a[12];
    a[13] = a[1] * x + a[5] * y + a[9]  * z + a[13];
    a[14] = a[2] * x + a[6] * y + a[10] * z + a[14];
    a[15] = a[3] * x + a[7] * y + a[11] * z + a[15];
    return *this;
  }
  

  Matrix4<Number> &setScale( const Number &x, const Number &y, const Number &z ){
    auto p = a;
    *p++ = x; *p++ = 0; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = y; *p++ = 0; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = z; *p++ = 0;
    *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 1;
    return *this;
  }

  Matrix4<Number> &scale( const Number &x, const Number &y, const Number &z ){
    a[0] *= x; a[4] *= y; a[8]  *= z;
    a[1] *= x; a[5] *= y; a[9]  *= z;
    a[2] *= x; a[6] *= y; a[10] *= z;
    a[3] *= x; a[7] *= y; a[11] *= z;
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

    p[4] = -bd * e + a * f;
    p[5] =  bd * f + a * e;
    p[6] = -b * c;

    p[8] =  ad * e + b * f;
    p[9] = -ad * f + b * e;
    p[10] =  a * c;

    p[3] = p[7] = p[11] = p[12] = p[13] = p[14] = 0;
    p[15] = 1;
    
    return *this;
  }

  Matrix4<Number> &setRotationX( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);

    p[0] = 1;    p[4] = 0;    p[8] =  0;
    p[1] = 0;    p[5] = c;    p[9] = -s;
    p[2] = 0;    p[6] = s;    p[10] = c;

    p[3] = p[7] = p[11] = p[12] = p[13] = p[14] = 0;
    p[15] = 1;
    
    return *this;
  }


  Matrix4<Number> &rotateY( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);
    const auto &ae = this->a;
    auto &te = this->a;

    auto a11 = ae[ 0 ], a12 = ae[ 4 ], a13 = ae[ 8 ],  a14 = ae[ 12 ];
    auto a21 = ae[ 1 ], a22 = ae[ 5 ], a23 = ae[ 9 ],  a24 = ae[ 13 ];
    auto a31 = ae[ 2 ], a32 = ae[ 6 ], a33 = ae[ 10 ], a34 = ae[ 14 ];
    auto a41 = ae[ 3 ], a42 = ae[ 7 ], a43 = ae[ 11 ], a44 = ae[ 15 ];

    te[ 0 ] = a11 * c + a13 * -s;
    te[ 4 ] = a12;
    te[ 8 ] = a11 * s + a13 * c;
    te[ 12 ] = a14;

    te[ 1 ] = a21 * c + a23 * -s;
    te[ 5 ] = a22;
    te[ 9 ] = a21 * s + a23 * c;
    te[ 13 ] = a24;

    te[ 2 ] = a31 * c + a33 * -s;
    te[ 6 ] = a32;
    te[ 10 ] = a31 * s + a33 * c;
    te[ 14 ] = a34;

    te[ 3 ] = a41 * c + a43 * -s;
    te[ 7 ] = a42;
    te[ 11 ] = a41 * s + a43 * c;
    te[ 15 ] = a44;
    
    
    return *this;
  }        

  Matrix4<Number> &setRotationY( const Number &x ){
    auto p = this->a;
    Number c = COSfp(x), s = SINfp(x);

    p[0] = c;    p[4] = 0;    p[8] =  s;
    p[1] = 0;    p[5] = 1;    p[9] =  0;
    p[2] =-s;    p[6] = 0;    p[10] = c;

    p[3] = p[7] = p[11] = p[12] = p[13] = p[14] = 0;
    p[15] = 1;
    
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

    auto a11 = ae[ 0 ], a12 = ae[ 4 ], a13 = ae[ 8 ],  a14 = ae[ 12 ];
    auto a21 = ae[ 1 ], a22 = ae[ 5 ], a23 = ae[ 9 ],  a24 = ae[ 13 ];
    auto a31 = ae[ 2 ], a32 = ae[ 6 ], a33 = ae[ 10 ], a34 = ae[ 14 ];
    auto a41 = ae[ 3 ], a42 = ae[ 7 ], a43 = ae[ 11 ], a44 = ae[ 15 ];
    auto b11 = be[ 0 ], b12 = be[ 4 ], b13 = be[ 8 ],  b14 = be[ 12 ];
    auto b21 = be[ 1 ], b22 = be[ 5 ], b23 = be[ 9 ],  b24 = be[ 13 ];
    auto b31 = be[ 2 ], b32 = be[ 6 ], b33 = be[ 10 ], b34 = be[ 14 ];
    auto b41 = be[ 3 ], b42 = be[ 7 ], b43 = be[ 11 ], b44 = be[ 15 ];

    te[ 0 ] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
    te[ 4 ] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
    te[ 8 ] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
    te[ 12 ] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

    te[ 1 ] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
    te[ 5 ] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
    te[ 9 ] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
    te[ 13 ] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

    te[ 2 ] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
    te[ 6 ] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
    te[ 10 ] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
    te[ 14 ] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

    te[ 3 ] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
    te[ 7 ] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
    te[ 11 ] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
    te[ 15 ] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;
    
    return *this;
  }

};


typedef Matrix4<Fixed> Matrix;

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
    Fixed x = this->x, y = this->y, z = this->z, w = 1; // this->w;

    this->x = e[ 0 ] * x + e[ 4 ] * y + e[ 8 ] * z + e[ 12 ] * w;
    this->y = e[ 1 ] * x + e[ 5 ] * y + e[ 9 ] * z + e[ 13 ] * w;
    this->z = e[ 2 ] * x + e[ 6 ] * y + e[ 10 ] * z + e[ 14 ] * w;
    // this->w = e[ 3 ] * x + e[ 7 ] * y + e[ 11 ] * z + e[ 15 ] * w;

    return *this;
  }

  void render( bool square ){

    Fixed fovz = 64 + z;
    int16_t x = ((64 * this->x) / fovz + 64).getInteger();
    int16_t y = (32 - (64 * this->y) / fovz).getInteger();	   
    auto size = ((64 * this->size) / fovz).getInteger();
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

    if( square ){
       ey -= sy;     
       for( sx=0; sx<=hsize; ++sx ){
	   arduboy.drawSymmetricVLine( x, -sx, sy, ey, color );
       }
    }else{
	arduboy.fillCircle( x, y, hsize, this->color );
    }
    
  }
  
};

typedef Point3D *Point3Dp;
typedef Point3D const *cPoint3Dp;

const Point3D hero_mesh[] PROGMEM = {
  { 4, 8, -47, 0xFF, 10},
  { 0, 8, -45, 0xFF, 11},
  {-4, 8, -47, 0xFF, 10},
  { 0,11, -53, 0xFF, 10},

  { 5, 8, -28, 0x01, 4},
  {-5, 8, -28, 0x01, 4},

  { 8, 4, -18, 0x01, 4},
  {-8, 4, -18, 0x01, 4},

  { 11, 2, -8, 0x01, 4},
  {-11, 2, -8, 0x01, 4},
  
  { 16, 8, 40, 0xAA, 8},
  { 24, -4, 40, 0xAA, 8},
  {  8, -4, 40, 0xAA, 8},
  {-16, 8, 40, 0xAA, 8},
  {-24, -4, 40, 0xAA, 8},
  { -8, -4, 40, 0xAA, 8},

  { 16, 0, 33, 0xFF, 18},
  {-16, 0, 33, 0xFF, 18},

  { 16, 0, 27, 0xFF, 18},
  {-16, 0, 27, 0xFF, 18},
  
  { 16, 0, 20, 0xFF, 18},
  {-16, 0, 20, 0xFF, 18},
  
  { 16, 0, 9, 0x1, 10},
  {-16, 0, 9, 0x1, 10},
  
  { 5, 2, 28, 0x01, 4},
  { 0, 2, 28, 0x01, 6},
  {-5, 2, 28, 0x01, 4}
};

const Point3D mesh[] PROGMEM = {
  { 5,10, -37, 1, 14},
  {-5, 8, -37, 1, 14},
  { 0,12, -47, 1, 14},
  
  { 16,10, 35, 1, 24},
  {-16, 0, 35, 1, 24},

  { 16,10, 20, 1, 24},
  {-16, 0, 20, 1, 24},
  
  { 16,10, 9, 0xFF, 15},
  {-16, 0, 9, 0xFF, 15},
  
  {0, 5, 28, 0xFF, 12}
};

const Point3D player_mesh[] PROGMEM = {
  { 5,10, -37, 0xFF, 14},
  {-5, 8, -37, 0xFF, 14},
  { 0,12, -47, 0xFF, 14},
  
  { 16,10, 35, 0xFF, 24},
  {-16, 0, 35, 0xFF, 24},

  { 16,10, 20, 0xFF, 24},
  {-16, 0, 20, 0xFF, 24},
  
  { 16,10, 9, 0x1, 15},
  {-16, 0, 9, 0x1, 15},
  
  {0, 5, 28, 0xFF, 12}
};

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

  int8_t flags, id, visible;
  int8_t screenX, screenY;

  void init( cPoint3Dp mesh, uint8_t vc ){

    x=0;   y=0;  z=0;
    scale = 1;
    visible = 1;
    rotX=0; rotY=0; rotZ=0;
    update = NULL;
    vertexCount = vc;
    this->mesh = mesh;
    
  }

  Node &setPosition( Fixed nx, Fixed ny, Fixed nz ){

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
  
  void init(){

    usedPointCount = 0;
    usedNodeCount = 0;
    
    for( int i=0; i<pointCount; ++i ){
      zBuffer[i].nodeId = 0xFF;
    }
    
  }

  Node &initNode( cPoint3Dp mesh, uint8_t pc ){
    
    Node &node = nodeList[usedNodeCount];
    node.init( mesh, pc );
    node.id = usedNodeCount;
    
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

      if( !node.visible )
	continue;

      Matrix &mat = node.transform;
      mat = camera;
      mat.translate( node.x, node.y, node.z );

      //if( !node.rotX && !node.rotZ && node.rotY )
      if( node.rotY )
	  mat.rotateY( node.rotY );
	  //else
//	  mat.rotate( node.rotX, node.rotY, node.rotZ );

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

      tmp.x *= node.scale;
      tmp.y *= node.scale;
      tmp.z *= node.scale;
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

	tmp.render( tmp.z > 250 );
	     
      }

    }

  }

};


typedef Scene<50,3> Scene36_3;

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

const ShipUpgrades opponents[] PROGMEM = {
  {2, 1, 1,1, 1,1},
  {1, 2, 2,1, 1,1},
  {1, 1, 2,1, 1,1},
  {2, 1, 1,1, 3,1},
  {2, 3, 1,1, 2,2},
  {1, 1, 3,1, 2,1},
  {4, 1, 1,1, 1,1},
  {10,10,100,10,99,99}
};

ShipUpgrades playerUpgrades = {
  1,1, 1,1, 1,1
};

uint32_t playerMoney = 100;
uint16_t raceCount, winCount, loseCount;
