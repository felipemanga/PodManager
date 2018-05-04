STATE( AdvMode,
       {
	 TileWindow ground;
	 Actor fu;
	 Actor player;
	 bool inputEnabled;
       },
       {
	 scope.inputEnabled = false;
	 seed = ~0xBEEF;
	 seedSequence = 0;
	 scope.ground.init( tiles, advModeGroundGen );
	 scope.ground.x = scope.ground.y = 8;

	 playChiptune([](uint16_t t){
	     return (t>>8)&~(t>>5|t>>6|t>>1);
	   });

	 initEnemies();
	 initShots();
	   
	 after.init();
	   
	 scope.player
	   .init()
	   .setPosition(64-8,32-8)
	   .setAnimation(&miniGirlWalkS)
	   .show()
	   .actorFlags = ACTOR_HIDDEN;
	   
	 scope.fu
	   .init()
	   .setAnimation( &miniFlightUnit )
	   .show()
	   .setPosition(0, -32)
	   .moveTo(64, 32)
	   .setTweenWeight( 3 )
	   .onTweenComplete([]{
	       scope.inputEnabled = true;
	       cameraShakeY = 20;
	       scope.player.actorFlags = ACTOR_PLAY;
	       scope.fu
		 .moveTo( 127, -32 )
		 .setTweenWeight( 6 )
		 .onTweenComplete([]{
		     scope.fu.hide();
		   });
	     })
	   .flags |= ANIM_INVERT;

       },
       {
	 after.update();	 
	 move();
	 scope.ground.render();
	 updateEnemies();
	   
	 scope.player.checkCollision(
				     enemies,
				     MAX_ENEMY_COUNT,
				     []( Actor *a ){
				       auto e = (Enemy *) a;
				       if( e->timeAlive ){
					 changeState( State::FightMode, 0 );
				       }
				     });
	 // scope.ground.matrix[ 2 * 9 + 4 ] = 0;
	 /*	   
		   auto enemy = &scope.player;
		   enemy->x = scope.ground.tx + 4;
		   enemy->y = scope.ground.ty + 2;
		   scope.ground.tileToScreen( enemy->x, enemy->y );
		   enemy->x = (enemy->x-8) << 8;
		   enemy->y = (enemy->y-8) << 8;
	 */	   
       },

       void move(){
	 if( !scope.inputEnabled || (scope.ground.x-8) & 0xF || (scope.ground.y-8) & 0xF )
	   return;

	 scope.ground.speedY = 0;
	 scope.ground.speedX = 0;
	   
	 if( isPressed(UP_BUTTON) )
	   scope.ground.speedY = 1;
	 else if( isPressed(DOWN_BUTTON) )
	   scope.ground.speedY = -1;
	 else
	   scope.ground.speedY = 0;

	 scope.player.animation = &miniGirlWalkE;
	 scope.player._onAnimationComplete = NULL;
	   
	 if( isPressed(RIGHT_BUTTON) ){
	   scope.ground.speedX = -1;
	   scope.player.actorFlags &= ~ACTOR_MIRROR;
	 }else if( isPressed(LEFT_BUTTON) ){
	   scope.ground.speedX = 1;
	   scope.player.actorFlags |= ACTOR_MIRROR;
	 }else{
	   scope.ground.speedX = 0;
	   scope.player._onAnimationComplete = []{
	     scope.player.actorFlags ^= ACTOR_MIRROR;
	   };
	       
	   if( !scope.ground.speedY ){
	     scope.player._onAnimationComplete = NULL;
	     scope.player.animation = &miniGirlIdle;
	     scope.player.actorFlags &= ~ACTOR_MIRROR;
	   }else if( scope.ground.speedY > 0 )
	     scope.player.animation = &miniGirlWalkN;
	   else
	     scope.player.animation = &miniGirlWalkS;
	       
	 }

	 auto &speedX = scope.ground.speedX;
	 auto &speedY = scope.ground.speedY;

	 bool hit = scope.ground.matrix[
					(2 - speedY) * 9 +
					4 - speedX
					] > 32;
	 bool hitY = scope.ground.matrix[
					 (2 - speedY) * 9 +
					 4
					 ] > 32;
	 bool hitX = scope.ground.matrix[
					 (2) * 9 +
					 4 - speedX
					 ] > 32;
	 
	 if( hit ){
	   if( !hitY ) speedX = 0;
	   else if( !hitX ) speedY = 0;
	   else speedX = speedY = 0;
	 }
	       
       }

       void tileFlierAI( Enemy *enemy ){
	 enemy->x = enemy->dataA;
	 enemy->y = enemy->dataB;
	 scope.ground.tileToScreen( enemy->x, enemy->y );
	 enemy->x = (enemy->x-8) << 8;
	 enemy->y = (enemy->y-12) << 8;
	 enemy->x += SIN( enemy->hp );
	 enemy->y += COS( enemy->hp );
	 enemy->hp++;
	 if( !enemy->hp ) enemy->hp++;

	 if( enemy->xH > -8 && enemy->yH > -8 && enemy->yH < 64 )
	   enemy->timeAlive--;
       }
       
       void tileWalkerAI( Enemy *enemy ){
	 enemy->x = enemy->dataA;
	 enemy->y = enemy->dataB;
	 scope.ground.tileToScreen( enemy->x, enemy->y );
	 enemy->x = (enemy->x-8) << 8;
	 enemy->y = (enemy->y-12) << 8;
	 if( enemy->xH > -8 && enemy->yH > -8 && enemy->yH < 64 )
	   enemy->timeAlive--;
       }       

       void spawnAdvEnemy( int8_t x, int8_t y ){

	 Enemy *enemyp = allocEnemy();
	 if( !enemyp ) return;
	   
	 auto &enemy = *enemyp;
	 enemy.dataA = x;
	 enemy.dataB = y;
	 enemy.timeAlive = 100;
	 enemy.hp = 1;

	 if( random(0, (int8_t)100 ) < 50 ){
	   enemy.ai = tileWalkerAI;	 
	   enemy.setAnimation( &enFly );
	   enemy.flags |= ANIM_INVERT;
	 }else{
	   enemy.ai = tileFlierAI;
	   enemy.setAnimation( &enFly );
	   enemy.flags |= ANIM_GRAY;
	 }

       }

       uint8_t advModeGroundGen( uint8_t x, uint8_t y){
	 uint8_t tileId = noise3( x, y, 50, 80 );
	 uint8_t n=0;

	 uint8_t l = noise3(x-1,y,50,80),
	   r = noise3(x+1,y,50,80),
	   u = noise3(x,y-1,50,80),
	   d = noise3(x,y+1,50,80);
    
	 if( tileId > 0 ){
	   if( l < tileId ) n |= 1;
	   if( r < tileId ) n |= 2;
	   if( u < tileId ) n |= 4;
	   if( d < tileId ) n |= 8;
	   if( !n ){
	     seed ^= 0x42;
	     n = noise1(x, y, 60);
	     seed ^= 0x42;
	     if( n ){
	       return 32+16;
	     }else if( scope.inputEnabled && random(0, (int8_t) 100) < 6 ){
	       spawnAdvEnemy( x, y );
	     }
	   }
	 }else{
	   if( l != tileId ) n |= 1;
	   if( r != tileId ) n |= 2;
	   if( u != tileId ) n |= 4;
	   if( d != tileId ) n |= 8;
	 }

	 uint8_t h = (tileId*2+(n>>3))*8+(n&7);

	 return h;
       }
       
       )
