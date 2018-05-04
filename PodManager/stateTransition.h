STATE( Transition,
       {
	   unsigned char *b;
	   unsigned char *e;
	   uint8_t clear;
       },
       {
	   scope.clear = clearScreen;
	   clearScreen = CLEAR_NONE;
	   scope.b = arduboy.sBuffer;
	   scope.e = arduboy.sBuffer+HEIGHT*WIDTH/8;
	   after.init();
	   playChiptune([](uint16_t t){
		   return (t>>4)&1;
	       });
       },
       {
	   if( scope.b >= scope.e ){
	       state = nextState;
	       clearScreen = scope.clear;
	       return;
	   }
	   for( uint8_t s=7; s; s-- ){
	       *(scope.b++) = transitionColor;
	       *(--scope.e) = transitionColor;
	   }
       }
    )
