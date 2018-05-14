STATE( Init,
       {
	 Scene36_3 scene;
       },
       {
	 playChiptune([](uint16_t t){
	     return t>>5|t>>6|t>>1;
	   });

	 seed = 3489;
	   
	 clearScreen = CLEAR_BLACK;

	 scope.scene.init();
	 scope.scene.camera.setRotation( 12, 0, -10 ).translate( 0, -50, -50 );

	 for( int i=0; i<3; ++i ){
	   Node &node = scope.scene.initNode(
					     mesh,
					     sizeof(mesh)/sizeof(mesh[0])
					     );
	   node.z = i*256;
	   node.x = (int16_t) random( int8_t(-100), int8_t(100) );
	 }
       },
       
       {
   
	 if( justPressed(A_BUTTON) )
	   changeState( State::PodSelect, 0xAA );

	 if( justPressed(B_BUTTON) ){
	   Arduboy2Audio::toggle();
	   Arduboy2Audio::saveOnOff();
	 }	   

	 for( int i=0; i<3; ++i ){
	   Node &node = scope.scene.nodeList[i];
	   node.z += 24;
	   if( node.z > 1024 ){
	     node.z = 0;
	     node.x = (int16_t) random( int8_t(-100), int8_t(100) );	     
	   }
	 }

	 scope.scene.update();
	 arduboy.drawCompressed( 0, 0, title_comp_w, WHITE );
	 
       }
       
)
	   
