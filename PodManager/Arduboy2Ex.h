#include <Arduboy2.h>

typedef Arduboy2 AB_BASE;

// extern const uint8_t bitshift_left[] PROGMEM;
const uint8_t mask_bits[] PROGMEM = {
  0xFF,
  0xFE,
  0xFC,
  0xF8,
  0xF0,
  0xE0,
  0xC0,
  0x80,
  0x00
};

const uint8_t rmask_bits[] PROGMEM = {
  0x00,
  0x01,
  0x03,
  0x07,
  0x0F,
  0x1F,
  0x3F,
  0x7F,
  0xFF
};

class Arduboy2Ext : public AB_BASE {
public:    
  Arduboy2Ext() : AB_BASE() { }
    
  void drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color, bool mirror=false );


  
  void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color=0xFF)
  {
    drawFasterVLine(x0, y0-r, 2*r+1, color);
    
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    while( x < y ){
      
      if( f >= 0 ){
	y--;
	ddF_y += 2;
	f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      drawSymmetricVLine(x0, x, y0-y, 2*y+1, color);
      drawSymmetricVLine(x0, y, y0-x, 2*x+1, color);
	
    }
  }

  void fillHalfCircleUpper(int16_t x0, int16_t y0, uint8_t r, uint8_t color=0xFF)
  {
    drawFasterVLine(x0, y0-r, r+1, color);
    drawSymmetricVLine(x0, 1, y0-r, r+1, color );
    drawSymmetricVLine(x0, 2, y0-r, r+1, color );
    drawSymmetricVLine(x0, 3, y0-r, r+1, color );

    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;
    

    while( x < y ){
      
      if( f >= 0 ){
	y--;
	ddF_y += 2;
	f += ddF_y;
      }

      x++;
      ddF_x++;
      f += ddF_x;

      /* */
      drawSymmetricVLine4(x0, x, y0-y, y+1, color);
      drawSymmetricVLine4(x0, y, y0-x, x+1, color);      
	 /*/
      int8_t x2 = x<<2, y2 = y<<2;
      drawSymmetricVLine(x0, x2++, y0-y, y+1, color);
      drawSymmetricVLine(x0, x2++, y0-y, y+1, color);
      drawSymmetricVLine(x0, x2++, y0-y, y+1, color);
      drawSymmetricVLine(x0, x2++, y0-y, y+1, color);
      drawSymmetricVLine(x0, y2++, y0-x, x+1, color);
      drawSymmetricVLine(x0, y2++, y0-x, x+1, color);
      drawSymmetricVLine(x0, y2++, y0-x, x+1, color);
      drawSymmetricVLine(x0, y2++, y0-x, x+1, color);
      /* */
      
    }
  }
  
    void drawSymmetricVLine( int16_t x, int8_t xoff, int16_t y, uint8_t h, uint8_t color=0xFF )
    {

	if( static_cast<uint16_t>(x+xoff)>=WIDTH || !xoff ){
	    drawFasterVLine( x-xoff, y, h, color );
	    return;
	}
	if( static_cast<uint16_t>(x-xoff)>=WIDTH ){
	    drawFasterVLine( x+xoff, y, h, color );
	    return;
	}

	if( y>=HEIGHT )
	    return;

	if( xoff < 0 )
	    xoff = -xoff;
	x -= xoff;
	xoff = xoff << 1;

	if( y < 0 ){
	    if( y+h < 0 ) return;
	    h += y;
	    y = 0;
	}

	if( y+h > HEIGHT )
	    h = HEIGHT - y;

	if( color==1 ){
	    uint8_t c=(uint8_t(x)+uint8_t(frameCount))&1;
	    if( c ) color = 0xAA;
	    else color = 0x55;
	}
	

      uint8_t *pixel = &sBuffer[x + (static_cast<uint8_t>(y) & 0xF8) * 16];
      uint8_t offset = y&7;
      
      if( offset ){
	
	uint8_t mask = 0xFF;

	if( h < 8-offset ){
	  mask = pgm_read_byte( &rmask_bits[h+offset] );
	  h = 0;
	}else{
	  h -= 8-offset;
	}

	mask &= pgm_read_byte( &mask_bits[ offset ] );
	
	pixel[0] = ( pixel[0] & ~mask ) | (color & mask);
	pixel[xoff] = ( pixel[xoff] & ~mask ) | (color & mask);
	
	pixel += WIDTH;
	
      }
      
      while( h>=8 ){
	
	pixel[0] = pixel[xoff] = color;
	pixel += WIDTH;	  
	h -= 8;
	  
      }

      if( h>0 ){
	
	uint8_t mask = 0xFF >> (8-h);
	pixel[0] = ( pixel[0] & ~mask ) | (color & mask);
	pixel[xoff] = ( pixel[xoff] & ~mask ) | (color & mask);
	
      }
      
    }

    void drawSymmetricVLine4( int16_t x, int8_t xoff, int16_t y, uint8_t h, uint8_t color=0xFF )
    {
        xoff <<= 2;
	if( !xoff ){
	    drawFasterVLine( x, y, h, color );
	    return;
	}
	
	if( static_cast<uint16_t>(x+xoff)>=WIDTH || static_cast<uint16_t>(x-xoff)>=WIDTH ){
	    drawFasterVLine( x - xoff + 0, y, h, color );
	    drawFasterVLine( x - xoff + 1, y, h, color );
	    drawFasterVLine( x - xoff + 2, y, h, color );
	    drawFasterVLine( x - xoff + 3, y, h, color );
	    drawFasterVLine( x + xoff - 0, y, h, color );
	    drawFasterVLine( x + xoff - 1, y, h, color );
	    drawFasterVLine( x + xoff - 2, y, h, color );
	    drawFasterVLine( x + xoff - 3, y, h, color );
	    return;
	}

	if( y>=HEIGHT )
	    return;

	if( xoff < 0 )
	    xoff = -xoff;
	x -= xoff;
	xoff = xoff << 1;

	if( y < 0 ){
	    if( y+h < 0 ) return;
	    h += y;
	    y = 0;
	}

	if( y+h > HEIGHT )
	    h = HEIGHT - y;

	uint8_t acolor = color;

	if( color==1 ){
	    uint8_t c=(uint8_t(x)+uint8_t(frameCount))&1;
	    if( c ){
	      color = 0xAA;
	      acolor = 0x55;
	    }else{
	      color = 0x55;
	      acolor = 0xAA;
	    }
	    
	}
	

      uint8_t *pixel = &sBuffer[x + (static_cast<uint8_t>(y) & 0xF8) * 16];
      uint8_t offset = y&7;
      uint8_t txoff;
      
      if( offset ){
	
	uint8_t mask = 0xFF;

	if( h < 8-offset ){
	  mask = pgm_read_byte( &rmask_bits[h+offset] );
	  h = 0;
	}else{
	  h -= 8-offset;
	}

	mask &= pgm_read_byte( &mask_bits[ offset ] );
	
	pixel[0] = ( pixel[0] & ~mask ) | (color & mask);
	pixel[1] = ( pixel[1] & ~mask ) | (acolor & mask);
	pixel[2] = ( pixel[2] & ~mask ) | (color & mask);
	pixel[3] = ( pixel[3] & ~mask ) | (acolor & mask);

	txoff = xoff;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (color & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (acolor & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (color & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (acolor & mask); txoff--;
	
	pixel += WIDTH;
	
      }
      
      while( h>=8 ){
	
	pixel[0] = pixel[2] = color;
	pixel[1] = pixel[3] = acolor;
	txoff = xoff;
	pixel[txoff] = color; txoff--;
	pixel[txoff] = acolor; txoff--;
	pixel[txoff] = color; txoff--;
	pixel[txoff] = acolor; txoff--;
	
	pixel += WIDTH;	  
	h -= 8;
	  
      }

      if( h>0 ){
	
	uint8_t mask = 0xFF >> (8-h);
	pixel[0] = ( pixel[0] & ~mask ) | (color & mask);
	pixel[1] = ( pixel[1] & ~mask ) | (acolor & mask);
	pixel[2] = ( pixel[2] & ~mask ) | (color & mask);
	pixel[3] = ( pixel[3] & ~mask ) | (acolor & mask);
	
	txoff = xoff;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (color & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (acolor & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (color & mask); txoff--;
	pixel[txoff] = ( pixel[txoff] & ~mask ) | (acolor & mask); txoff--;
	
      }
      
    }
    
    void drawFasterVLine( int16_t x, int16_t y, uint8_t h, uint8_t color=0xFF )
    {

      if( static_cast<uint16_t>(x)>=WIDTH )
	return;

      if( y>=HEIGHT )
	return;

      if( y < 0 ){
	if( y+h < 0 ) return;
	h += y;
	y = 0;
      }

      if( y+h > HEIGHT )
	h = HEIGHT - y;

      if( color & color != 0xFF ){
	uint8_t c=(uint8_t(x)+uint8_t(frameCount))&1;
	if( c ) color = 0xAA;
	else color = 0x55;
      }
	

      uint8_t *pixel = &sBuffer[x + (static_cast<uint8_t>(y) / 8) * WIDTH];
      uint8_t offset = y&7;
      
      if( offset ){
	
	uint8_t mask = 0xFF;

	if( h < 8-offset ){
	  mask = pgm_read_byte( &rmask_bits[h+offset] );
	  h = 0;
	}else{
	  h -= 8-offset;
	}

	mask &= pgm_read_byte( &mask_bits[ offset ] );
	
	*pixel = ( *pixel & ~mask ) | (color & mask);
	pixel += WIDTH;
	
      }
      
      while( h>=8 ){
	
	*pixel = color;
	pixel += WIDTH;	  
	h -= 8;
	  
      }

      if( h>0 ){
	
	uint8_t mask = 0xFF >> (8-h);
	*pixel = ( *pixel & ~mask ) | (color & mask);
	
      }
      
    }
    
};

struct BitStreamReader {

  const uint8_t *source;
  uint16_t sourceIndex;
  uint8_t bitBuffer;
  uint8_t byteBuffer;
  
  BitStreamReader(const uint8_t *source)
    : source(source), sourceIndex(), bitBuffer(), byteBuffer()
  {
  }
  
  uint16_t readBits(uint16_t bitCount)
  {
    uint16_t result = 0;
    for (uint16_t i = 0; i < bitCount; i++)
    {
      if (this->bitBuffer == 0)
      {
        this->bitBuffer = 0x1;
        this->byteBuffer = pgm_read_byte(&this->source[this->sourceIndex]);
        ++this->sourceIndex;
      }
      
      if ((this->byteBuffer & this->bitBuffer) != 0)
        result |= (1 << i); // result |= bitshift_left[i];
        
      this->bitBuffer <<= 1;
    }
    return result;
  }
  
};


void Arduboy2Ext::drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color, bool mirror)
{
  // set up decompress state

  BitStreamReader cs = BitStreamReader(bitmap);

  // read header

  int width = (int)cs.readBits(8) + 1;
  int height = (int)cs.readBits(8) + 1;
  uint8_t spanColour = (uint8_t)cs.readBits(1); // starting colour

  // no need to draw at all if we're offscreen
  if ((sx + width < 0) || (sx > WIDTH - 1) || (sy + height < 0) || (sy > HEIGHT - 1))
    return;

  // sy = sy - (frame * height);

  int yOffset = abs(sy) % 8;
  int startRow = sy / 8;
  if (sy < 0) {
    startRow--;
    yOffset = 8 - yOffset;
  }
  int rows = height / 8;
  if ((height % 8) != 0)
    ++rows;

  int rowOffset = 0; // +(frame*rows);
  int columnOffset = 0;
  
  if (mirror) {
    columnOffset = width - 1;
  }
  else {
    columnOffset = 0;
  }

  uint8_t byte = 0x00;
  uint8_t bit = 0x01;
  while (rowOffset < rows) // + (frame*rows))
  {
    uint16_t bitLength = 1;
    while (cs.readBits(1) == 0)
      bitLength += 2;

    uint16_t len = cs.readBits(bitLength) + 1; // span length

    // draw the span
    for (uint16_t i = 0; i < len; ++i)
    {
      if (spanColour != 0)
        byte |= bit;
      bit <<= 1;

      if (bit == 0) // reached end of byte
      {
        // draw
        int bRow = startRow + rowOffset;

        //if (byte) // possible optimisation
        if ((bRow <= (HEIGHT / 8) - 1) && (bRow > -2) && (columnOffset + sx <= (WIDTH - 1)) && (columnOffset + sx >= 0))
        {
          int16_t offset = (bRow * WIDTH) + sx + columnOffset;
          if (bRow >= 0)
          {
            int16_t index = offset;
            uint8_t value = byte << yOffset;
            
            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
          if ((yOffset != 0) && (bRow < (HEIGHT / 8) - 1))
          {
            int16_t index = offset + WIDTH;
            uint8_t value = byte >> (8 - yOffset);
            
            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
        }


        // iterate
        if (!mirror) {
          ++columnOffset;
          if (columnOffset >= width)
          {
            columnOffset = 0;
            ++rowOffset;
          }
        }
        else {
          --columnOffset;
          if (columnOffset < 0) {
            columnOffset = width - 1;
            ++rowOffset;
          }
        }


        // reset byte
        byte = 0x00;
        bit = 0x01;
      }
    }

    spanColour ^= 0x01; // toggle colour bit (bit 0) for next span
  }
}

