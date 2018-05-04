#include "FixedPoints/FixedPoints.h"

typedef SFixed<23,8> Fixed;

/* Incorrect but goot enough */
template<>
constexpr SFixed<23,8> operator *(const SFixed<23,8> & left, const SFixed<23,8> & right)
{	
  return SFixed<23, 8>::fromInternal(left.getInternal() * right.getInternal() >> 16);
}
/* */

Fixed COSfp( Fixed a ){
  return Fixed::fromInternal( COS16( a.getInteger() ) );
}

Fixed SINfp( Fixed a ){
  return Fixed::fromInternal( SIN16( a.getInteger() ) );
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
  
};

typedef Point3D *Point3Dp;
typedef Point3D const *cPoint3Dp;

const Point3D mesh[] PROGMEM = {
  {0, 0, 0, 0, 32},
  {32, 0, 0, 0xAA, 16}
};

class Node {
public:
  Fixed x, y, z;
  uint8_t rotX, rotY, rotZ;
  uint8_t vertexCount;
  cPoint3Dp mesh;
  Matrix transform;

  void init(
    int16_t x, int16_t y, int16_t z,
    uint8_t rotX, uint8_t rotY, uint8_t rotZ,
    uint8_t vc, cPoint3Dp mesh
	    ){
    this->x = x;
    this->y = y;
    this->z = z;
    this->rotX = rotX;
    this->rotY = rotY;
    this->rotZ = rotZ;
    vertexCount = vc;
    this->mesh = mesh;
  }
};
