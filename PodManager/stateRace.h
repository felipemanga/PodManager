STATE( RaceMode,
       
       {
	 Scene36_3 scene;
	 uint8_t tick;
	 uint8_t slowTick;
	 uint8_t liveCount;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -15 ).translate( 0, -60, 325 );
	 scope.tick = 0;
	 scope.slowTick = 0;
	 
	 ShipUpgrades tmp;
	 ShipUpgrades *sup;
	 const uint8_t racerCount = 3-(winCount&1);
	 scope.liveCount = racerCount;
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
	     pgm_read_struct( &tmp, &opponents[ min( raceCount+i, 8 ) ] );
	     sup = &tmp;
	   }

	   sup->apply( racers[i] );
	   racers[i].startPosition = racers[i].position = -i*(256/racerCount);
	   
	 }	 
 
	 clearScreen = CLEAR_BLACK;
	 playChiptune([](uint16_t t){
		 return (t>>5|t>>8|t>>2);
	     });
	 	 
       },
       
       {
	 if( scope.liveCount <= 1 )
	   changeState( State::AfterRace, 0xFF );
	 
	 if( !scope.tick )
	   scope.tick = 11;
	 scope.tick--;
	 if( !scope.slowTick )
	   scope.slowTick = 60;
	 scope.slowTick--;

	 drawBackground();
	 // memcpy_P( arduboy.sBuffer, track_bitmap+2, 128*64/8 );

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

	 for( uint8_t i=0; i<scope.scene.usedNodeCount; ++i ){
	   auto &ship = racers[i];
	   if( ship.position > 256*5 ){
	     changeState( State::AfterRace, 0xFF );
	     return;
	   }
	 }
	 
       }
       
       bool updatePhysics( Node &racer );
      
       void updateDead( Node &racer ){
	 auto &ship = racers[ racer.flags ];
	 ship.dead--;
	 if( !ship.dead ){
	   racer.update = NULL;
	   ship.dead = 1;
	   racer.visible = 0;
	   scope.liveCount--;
	 }else{
	   racer.scale *= Fixed(1.25f);
	   racer.y = Fixed::fromInternal(SIN(ship.dead))*10;
	 }
       }
       
       void updateAI( Node &racer ){
	 auto &ship = racers[ racer.flags ];

	 if( ship.charge >= BOOST_COST ){
	   ship.boost();
	 }

	 
	 bool shouldJump = updatePhysics( racer ) && !ship.jumping;

	 if( shouldJump && ship.charge >= JUMP_COST && (rand()&0xFF) < 30 ){
	   ship.jump();
	 }

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

       bool updatePhysics( Node &node ){
	 auto &ship = racers[ node.flags ];
	 
	 if( !scope.tick ){	   
	   ship.charge = min( ship.charge+ship.chargeSpeed, ship.maxCharge );
	   ship.speed *= Fixed(0.98f);
	 }

	 if( !scope.slowTick ){
	   ship.shield = min( ship.shield+ship.shieldRegen, ship.maxShield );
	 }
	   
	 if( node.y <= 0 || ship.jumping ){
	   	   
	   if( ship.jumping ){
	     ship.ySpeed += 10;
	     ship.jumping--;
	   }else{
	     ship.ySpeed += Fixed(0.75f);
	   }
	   
	 }else
	   ship.ySpeed -= Fixed(0.5f);

	 ship.ySpeed *= Fixed(0.93f);
	 node.y += ship.ySpeed;

	 ship.position -= ship.speed;
	 
	 if( node.y > 10 )
	   ship.position -= 2;
	 
	 node.rotY = ship.position.getInteger()-64;

	 node.x = SINfp( ship.position.getInteger() ) * 256;
	 node.z = COSfp( ship.position.getInteger() ) * 256;

	 bool shouldJump = false;

	 for( uint8_t i=0; i<scope.scene.usedNodeCount; ++i ){
	   if( i == node.id )
	     continue;
	   
	   auto &other = scope.scene.nodeList[i];
	   auto &otherShip = racers[other.flags];

	   if( otherShip.dead )
	     continue;
	   
	   if( !shouldJump && ship.collides(otherShip, 60, false) )
	     shouldJump = true;

	   auto dy = node.y - other.y;
	   if( dy > 10 || dy < -10 )
	     continue;

	   auto collision = ship.collides(otherShip, 10, true);
	   if( !collision )
	     continue;

	   Fixed fds = (ship.speed - otherShip.speed) * collision * Fixed(0.5f);
	   ship.speed -= fds;
	   otherShip.speed += fds;

	   auto ds = (fds*10).getInteger();
	   if( ds < 0 )
	     ds = -ds;
	   
	   if( ship.shield < ds ){
	     ship.die();
	     node.update = updateDead;
	   }else
	     ship.shield -= ds;
	   
	   if( otherShip.shield < ds ){
	     otherShip.die();
	     other.update = updateDead;
	   }else
	     otherShip.shield -= ds;
	   
	 }

	 return shouldJump;
	 
       }
       
)
