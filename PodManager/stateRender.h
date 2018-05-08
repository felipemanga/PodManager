STATE( RenderMode,
       
       {
	 Scene36_3 scene;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -15 ).translate( 0, -30, 60 );

	 Node &node = scope.scene.initNode(
					   hero_mesh,
					   sizeof(hero_mesh)/sizeof(hero_mesh[0])
					   );
	   
	 clearScreen = CLEAR_NONE;
	 playChiptune([](uint16_t t){
	     return (t>>4)|(t>>8)|(t>>12);
	   });
	 	 
       },
       
       {
	 memcpy_P( arduboy.sBuffer, track_bitmap+2, 128*64/8 );

	 if( justPressed(A_BUTTON) )
	   changeState( State::RaceMode, 0xAA );

	 scope.scene.nodeList[0].rotY = COS( (arduboy.frameCount) ) >> 2;
	 scope.scene.update();
	 
       }
       
)
