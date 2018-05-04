STATE( FightMode,
       {
	   Actor player;
	   bool isIdle;
       },
       {
	   playChiptune([](uint16_t t){
		   return ~(t>>5|t>>6|t)-1;
	       });
	   clearScreen = CLEAR_GRAY;
	   scope.player.init()
	       .show()
	       .actorFlags = ACTOR_PLAY;
	   idle();
       },
       {
	   int8_t walk = !!justPressed(RIGHT_BUTTON) - !!justPressed(LEFT_BUTTON);
	   if( scope.isIdle ){
	       bool mirrored = scope.player.xH > 64;
	   
	       if( mirrored ){
		   scope.player.actorFlags |= ACTOR_MIRROR;
		   walk = -walk;
	       }else
		   scope.player.actorFlags &= ~ACTOR_MIRROR;	  
	   
	       if( walk>0 )
		   setAnimation( &girlWalk );
	       else if( walk<0 )
		   setAnimation( &girlBack );
	       else if( isPressed(DOWN_BUTTON) )
		   scope.player
		       .setAnimation( &girlDuck )
		       .onAnimationComplete( idle );
	       else if( isPressed(A_BUTTON) )
		   changeState( State::FlightMode, 0xFF );
	       else if( isPressed(B_BUTTON) )
		   changeState( State::AdvMode, 0x00 );
	   }else if( scope.player.animation == &girlWalk ){

	       if( walk>0 ){
	       }
	       
	   }
	   
       },

       void idle(){
	   scope.player.setAnimation( &girlIdle )
	       .onAnimationComplete(NULL);
	   scope.isIdle = true;
       }

       void setAnimation( const void *anim ){
	   scope.player.setAnimation( anim )
	       .onAnimationComplete( idle );
	   scope.isIdle = false;
       }
       
    )
