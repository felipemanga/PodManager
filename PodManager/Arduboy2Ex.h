#include <Arduboy2.h>

class Arduboy2Ext : public Arduboy2Base {
public:    
  Arduboy2Ext() : Arduboy2Base() { }
    
  void drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color, bool mirror=false );


  
  void fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color=0xFF)
  {
    drawFasterVLine(x0, y0-r, 2*r+1, color);
    fillFastCircleHelper(x0, y0, r, 3, 0, color);
  }

  void fillFastCircleHelper(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta, uint8_t color)
  {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while( x < y ){
      
      if( f >= 0 ){
	y--;
	ddF_y += 2;
	f += ddF_y;
      }

      x++;
      ddF_x += 2;
      f += ddF_x;

      if( sides & 0x1 ){
	drawFasterVLine(x0+x, y0-y, 2*y+1+delta, color);
	drawFasterVLine(x0+y, y0-x, 2*x+1+delta, color);
      }

      if( sides & 0x2 ){
	drawFasterVLine(x0-x, y0-y, 2*y+1+delta, color);
	drawFasterVLine(x0-y, y0-x, 2*x+1+delta, color);
      }
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

      uint16_t row = x + (y / 8) * WIDTH;
      uint8_t offset = y&7;
      
      if( offset ){
	
	uint8_t mask = 0xFF;

	if( h < 8-offset ){
	  mask >>= (8-h);
	  h = 0;
	}else{
	  h -= 8-offset;
	}

	mask <<= offset;
	
	sBuffer[row] = ( sBuffer[row] & ~mask ) | (color & mask);
	row += WIDTH;
	
      }
      
      while( h>=8 ){
	
	sBuffer[row] = color;
	h -= 8;
	row += WIDTH;	  
	  
      }

      if( h>0 ){
	
	uint8_t mask = 0xFF >> (8-h);
	sBuffer[row] = ( sBuffer[row] & ~mask ) | (color & mask);
	
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

