STATE( RaceMode,
       
       {
	 Scene36_3 scene;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -15 ).translate( 0, -60, 325 );

	 const uint8_t racerCount = 3;
	 for( uint8_t i=0; i<racerCount; ++i ){
	   
	   Node &node = scope.scene.initNode(
					     i ? mesh : player_mesh,
					     sizeof(mesh)/sizeof(mesh[0])
					     );
	   
	   node.rotY = i*(256/racerCount);

	   if( i==0 )
	     node.update = updatePlayer;
	   else
	     node.update = updateAI;
	   
	 }	 

	 clearScreen = CLEAR_NONE;
	 playChiptune([](uint16_t t){
	     return (t>>4)|(t>>8)|(t>>12);
	   });
	 	 
       },
       
       {

	 // arduboy.drawBitmap( 0, 0, track_bitmap+2, 128, 64 );
	 memcpy_P( arduboy.sBuffer, track_bitmap+2, 128*64/8 );

	 scope.scene.update();
	 
       },

       void updatePhysics( Node &racer );
       
       void updateAI( Node &racer ){
	 updatePhysics( racer );
       }

       void updatePlayer( Node &player ){
	       
	 if( isPressed(A_BUTTON) ){
	       
	   player.sx=1;
	       
	 }else if( justPressed(B_BUTTON) ){
	       
	   player.sy=15;
	   player.sx=2;
	       
	 }

	 updatePhysics( player );
	 
       }

       void updatePhysics( Node &racer ){

	 if( racer.y <= 0 ){
	   if( racer.sx < 2 )
	     racer.sx++;
	   if( racer.sx > 2 )
	     racer.sx--;
	 }

	 racer.sy -= 2;
	 racer.y += racer.sy;
	 if( racer.y < 0 && racer.sy < 0 ){
	   racer.sy = -(racer.sy>>1);
	   racer.y = 0;
	 }
		 
	 racer.rotY -= racer.sx;
	 racer.x = SINfp( racer.rotY+64 ) * 256;
	 racer.z = COSfp( racer.rotY+64 ) * 256;
	 // racer.rotX = (SINfp( racer.rotY*3 )*5).getInteger();
	 
       }
       
)
