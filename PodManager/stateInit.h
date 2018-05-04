STATE( Init,
       {
	 Actor title;
       },
       {
	 playChiptune([](uint16_t t){
	     return t>>5|t>>6|t>>1;
	   });
	   
	 clearScreen = CLEAR_WHITE;

	 scope.title
	   .init()
	   .setPosition( 0, 64 )
	   // .setAnimation( &titleAnim )
	   .moveTo(0, 0)
	   .setTweenWeight(2)
	   .show()
	   .flags |= ANIM_INVERT;

	  
       },
       {
	 state = State::RenderMode;
	  
	 if( justPressed(A_BUTTON) ){
	   changeState( State::RenderMode, 0xAA );
	 }

	 if( justPressed(B_BUTTON) ){
	   Arduboy2Audio::toggle();
	   Arduboy2Audio::saveOnOff();
	 }	   
    
       }
       
)
