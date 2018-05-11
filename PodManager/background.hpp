void drawBackground(){
    uint32_t *sbuffer = reinterpret_cast<uint32_t *>(arduboy.sBuffer);
    uint32_t *src = reinterpret_cast<const uint32_t *>(bg_bitmap+2);
    if( arduboy.frameCount & 1 )
	src += 8;

    uint8_t offset = arduboy.frameCount & 2;
    uint8_t rows = 8, cols;
    
    union {
	struct {
	    uint16_t low, high;
	};
	uint32_t data;
    } u;

    if( offset ){
	
	do{
	
	    u.data = pgm_read_dword(src++);
	    auto tmp = u.low;
	    u.low = u.high;
	    u.high = tmp;

	    cols = 32;
	    do{
		*sbuffer++ = u.data;
	    }while( --cols );

	}while( --rows );
	
    }else{
    
	do{
	
	    u.data = pgm_read_dword(src++);

	    cols = 32;
	    do{
		*sbuffer++ = u.data;
	    }while( --cols );

	}while( --rows );
	
    }
}
