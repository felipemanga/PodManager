STATE( RaceMode,
       
       {
	 Scene36_3 scene;
	 uint8_t tick;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -15 ).translate( 0, -60, 325 );
	 scope.tick = 0;

	 ShipUpgrades tmp;
	 ShipUpgrades *sup;
	 const uint8_t racerCount = 3-(raceCount&1);
	 for( uint8_t i=0; i<racerCount; ++i ){
	   
	   Node &node = scope.scene.initNode(
					     i ? mesh : player_mesh,
					     sizeof(mesh)/sizeof(mesh[0])
					     );
	   
	   node.flags = i;
	   if( i==0 ){
	     node.update = updatePlayer;
	     sup = &playerUpgrades;
	   }else{
	     node.update = updateAI;
	     pgm_read_struct( &tmp, &opponents[raceCount+i] );
	     sup = &tmp;
	   }

	   sup->apply( racers[i] );
	   racers[i].position = i*(256/racerCount);
	   
	 }	 

	 clearScreen = CLEAR_NONE;
	 playChiptune([](uint16_t t){
	     return (t>>4)|(t>>8)|(t>>12);
	   });
	 	 
       },
       
       {
	 if( !scope.tick )
	   scope.tick = 11;
	 scope.tick--;

	 // arduboy.drawBitmap( 0, 0, track_bitmap+2, 128, 64 );
	 memcpy_P( arduboy.sBuffer, track_bitmap+2, 128*64/8 );

	 scope.scene.update();

	 updateHUD();
	 
       },

       void updateHUD(){
	 
	 uint8_tp boostRow = &arduboy.sBuffer[0]; // [ WIDTH*((HEIGHT-1)/8) ];

	 uint16_t fp = (racers[0].charge*128) / racers[0].maxCharge;

	 uint8_t v = fp;
	 uint8_t iv = 128 - v;

	 uint8_t c = (arduboy.frameCount&1) ? 0xA : 0xD;
	 while( v-- ){
	   *boostRow++ |= c;
	   c ^= 0x3;
	 }
	 

	 while( iv-- )
	   *boostRow++ &= 0xF0;
	 
       }

       void updatePhysics( Node &racer );
       
       void updateAI( Node &racer ){
	 auto &ship = racers[ racer.flags ];

	 if( ship.charge >= BOOST_COST ){
	   ship.boost();
	 }

	 
	 updatePhysics( racer );
       }

       void updatePlayer( Node &player ){
	 auto &ship = racers[ player.flags ];
	       
	 if( justPressed(A_BUTTON) && ship.charge >= JUMP_COST ){
	   ship.jump();
	 }else if( justPressed(B_BUTTON) && ship.charge >= BOOST_COST ){
	   ship.boost();
	 }

	 updatePhysics( player );
	 
       }

       void updatePhysics( Node &node ){
	 auto &ship = racers[ node.flags ];
	 
	 if( !scope.tick ){	   
	   ship.charge = min( ship.charge+ship.chargeSpeed, ship.maxCharge );
	 }
	   
	 if( node.y <= 0 || ship.jumping ){
	   	   
	   if( ship.jumping ){
	     ship.ySpeed += 10;
	     ship.jumping--;
	   }else{
	     ship.ySpeed += 0.5f;	     
	   }
	   
	 }else
	   ship.ySpeed -= 0.5;

	 ship.ySpeed *= 0.93f;
	 node.y += ship.ySpeed;
	 
	 ship.position -= ship.speed;
	 node.rotY = ship.position.getInteger()-64;

	 node.x = SINfp( ship.position.getInteger() ) * 256;
	 node.z = COSfp( ship.position.getInteger() ) * 256;
	 
       }
       
)
