STATE( RaceMode,
       
       {
	 Scene36_3 scene;
	 uint8_t tick;
	 uint8_t slowTick;
	 uint8_t liveCount;
       },
       
       {

	 scope.scene.init();
	 scope.scene.camera.setRotationX( -20 ).translate( 0, -70, 50 );
	 scope.tick = 0;
	 scope.slowTick = 0;
	 
	 ShipUpgrades tmp;
	 ShipUpgrades *sup;
	 const uint8_t racerCount = 3-(winCount&1);
	 scope.liveCount = racerCount;
	 uint8_t i;
	 for( i=0; i<racerCount; ++i ){
	   
	   Node &node = scope.scene.initNode(
					     i ? mesh : player_mesh,
					     sizeof(mesh)/sizeof(mesh[0])
					     );
	   
	   node.data = i;
	   if( i==0 ){
	     node.update = updatePlayer;
	     sup = &playerUpgrades;
	   }else{
	     node.update = updateAI;
	     pgm_read_struct( &tmp, &opponents[ min( raceCount+i, 8 ) ] );
	     sup = &tmp;
	   }

	   sup->apply( racers[i] );
	   racers[i].startPosition = racers[i].position = i;
	   node.z = i*160;
	   node.x = random(int8_t(-100), int8_t(100));
	 }

	 for( ; i<sizeof(scope.scene.nodeList)/sizeof(scope.scene.nodeList[0]); ++i ){
	     Node &node = scope.scene.initNode(
		 rock_mesh,
		 sizeof(rock_mesh)/sizeof(rock_mesh[0])
		 );
	     
	     node.update = updateObstacle;
	     node.flags |= FLAG_SQUARE;
	     node.z = (i-racerCount)*137;
	     node.x = Fixed(random(int8_t(-100), int8_t(100))) * 5;
	     node.rotY = random(int8_t(-100), int8_t(100));
	 }
 
	 clearScreen = CLEAR_NONE;
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

	 scope.scene.update();

	 updateHUD();
	 
       },

       void updateHUD(){
/*	 
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
*/	 
       }
       
       bool updatePhysics( Node &racer );
      
       void updateDead( Node &racer ){
	 auto &ship = racers[ racer.data ];
	 ship.dead--;
	 if( !ship.dead ){
	   racer.update = NULL;
	   ship.dead = 1;
	   racer.flags |= FLAG_HIDDEN;
	   scope.liveCount--;
	 }else{
	   racer.scale *= Fixed(1.25f);
	   racer.y = Fixed::fromInternal(SIN(ship.dead))*10;
	 }
       }

       void updateObstacle( Node &rock ){
	 rock.z -= 10;
	 if( rock.z < -80 ){
	     rock.z += 700;
	     rock.x = Fixed(random(int8_t(-100), int8_t(100))) * 5;
	     rock.rotY = random(int8_t(-128), int8_t(127));
	     rock.scale = Fixed::fromInternal(random(int8_t(10),int8_t(20))) * 100;
	 }
       }
       
       void updateAI( Node &racer ){
	 auto &ship = racers[ racer.data ];
	 racer.z -= 5;
	 if( racer.z < -80 ){
	     racer.z += 700;
	     racer.x = random(int8_t(-100), int8_t(100));
	 }
	 /*

	 if( ship.charge >= BOOST_COST ){
	   ship.boost();
	 }
	 bool shouldJump = updatePhysics( racer ) && !ship.jumping;

	 if( shouldJump && ship.charge >= JUMP_COST && random(int8_t(0), int8_t(0x7F)) < 20 ){
	   ship.jump();
	 }
	 */
       }

       void updatePlayer( Node &player ){
	 auto &ship = racers[ player.data ];

	 Fixed xdelta = 0;

	 if( isPressed(LEFT_BUTTON) ){
	     xdelta = -0.5;
	 }else if( isPressed(RIGHT_BUTTON) ){
	     xdelta = 0.5;
	 }

	 if( player.x < -110 ) xdelta = 0.5;
	 else if( player.x > 110 ) xdelta = -0.5;

	 ship.ySpeed *= 0.98;
	 ship.ySpeed += xdelta;
	 ship.ySpeed = min(10, max(-10, ship.ySpeed));

	 player.x += ship.ySpeed;
	 player.y = Fixed::fromInternal(SIN(arduboy.frameCount*3))*7;
	 player.rotZ = (ship.ySpeed * 3).getInteger();

	 // player.x = min( 110, max(-110, player.x ));
	       
	 if( justPressed(A_BUTTON) ){
	 }else if( justPressed(B_BUTTON) ){
	 }
	   
	 updatePhysics( player );
	 
       }

       bool updatePhysics( Node &node ){
	 auto &ship = racers[ node.data ];
	 
	 if( !scope.tick ){	   
	   ship.charge = min( ship.charge+ship.chargeSpeed, ship.maxCharge );
	   ship.speed *= Fixed(0.98f);
	 }

	 if( !scope.slowTick ){
	   ship.shield = min( ship.shield+ship.shieldRegen, ship.maxShield );
	 }

	 return false;
	 
       }
       
)
