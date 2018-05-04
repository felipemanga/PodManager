STATE( FlightMode,
       {
	   FlightModePlayer player;
	   Wave wave;
	   TileWindow ground;
       },
       {
	   clearScreen = 0;
	   
	   playChiptune([](uint16_t t){
		   return t>>3|t>>8|t;
	       });

	   after.init();

	   seed = 0x1942;
	   seedSequence = 0;
	   scope.ground.init( tiles, flightModeGroundGen );
	   scope.ground.speedY = 1;

	   scope.player.init()
	       .setPosition( 64, 128+16 )
	       .moveTo( 64, 32 )
	       .setTweenWeight( 4 )
	       .onTweenComplete([]{
		   scope.player.inputEnabled = true;
	       });
	   scope.player.onDamage = playerDamage;

	   scope.wave.init( 50, 20, 5, spawnEnemy );

	   initEnemies();
	   
	   initShots();
	   
       },
       {
	   if( !scope.player.immune && scope.player.hp )
	       checkCollisions();
	   after.update();
	   scope.ground.render();
	   scope.player.update();
	   scope.wave.update();
	   updateEnemies();
	   updateShots();
	   
       },

       void checkCollisions(){
	   scope.player.checkCollision(
	       enemies,
	       MAX_ENEMY_COUNT,
	       []( Actor *a ){
		   auto e = (Enemy *) a;
		   if( e->timeAlive <= 1 || scope.player.immune )
		       return;
		   e->destroy();
		   scope.player.damage( 10 );
	       }
	       );
       }

       const uint8_t sequence[] PROGMEM = {
	   13,
	   5,
	       6,
	       7,
	       8,
	       
	       5,
	       6,
	       1,
	       2,
	       12,
	       
	       6,
	       5,
	       3,
	       4,
	       
	       10,
	       9,
	       2,
	       1,
	       8,
	       12,
	       
	       0,
	       0
       };

       void playerDamage( LiveActor *actor ){
	   auto &player = scope.player;
	   player.immune = 60;
	   cameraShakeY = 10;
	   cameraShakeX = 20;
	   if( player.hp ) return;

	   player.inputEnabled = false;
	   player.actorFlags |= ACTOR_HIDDEN;
	   after.frames(60) = []{
	       state = State::Init;
	   };
       }

       void enemyDamage( LiveActor *actor ){
	   if( actor->hp ) return;
	   ((Enemy *) actor)->destroy();
       }

       void enemyShoot( Enemy *enemy ){
	   Shot *shot = allocShot();
	   if( !shot ) return;
	   shot->init( *enemy, &scope.player, 1, sizeof(scope.player) );

	   shot->dx = random( 1, (int8_t) 3 );
	   if( enemy->x > scope.player.x )
	       shot->dx = -shot->dx;
	   
	   shot->dy = random( 1, (int8_t) 3 );
	   if( enemy->y > scope.player.y )
	       shot->dy = -shot->dy;

       }

       bool spawnEnemy( Enemy &enemy, uint8_t wid ){
	   if( !pgm_read_byte(sequence + wid + 1) )
	       scope.wave.waveDelay = 200;
	   
	   wid = pgm_read_byte(sequence + wid);
	   if( wid )
	       wid--;
	   else if( scope.player.inputEnabled ){
	       scope.player
		   .moveTo( scope.player.xH, -40 )
		   .setTweenWeight( 5 )
		   .onTweenComplete([]{
			   changeState( State::AdvMode, 0 );
		       });
	       scope.player.inputEnabled = false;
	       return false;
	   }else return false;

	   enemy.data = (void *) (patterns+wid);
	   enemy.ai = patternAI;
	   enemy.timeAlive = 1;
	   enemy.setAnimation( &enFly ).setTweenWeight(0);
	   enemy.hp = 30;
	   enemy.shoot = enemyShoot;
	   enemy.onDamage = enemyDamage;
	   
	   return true;
       }
	   
    )
