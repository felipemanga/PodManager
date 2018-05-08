STATE( RenderMode,
       
       {
	 Scene36_3 scene;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -15 ).rotate(0,100,0).translate( -40, -30, -40 );

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

	 scope.scene.nodeList[0].y = Fixed::fromInternal(SIN( arduboy.frameCount ))*3;
	 scope.scene.update();
	 
       }
       
)
