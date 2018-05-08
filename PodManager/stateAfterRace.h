STATE( AfterRace,
       {
	 Scene36_3 scene;
	 uint8_t racerCount;
	 char const *msg;
       },
       {
	 playChiptune([](uint16_t t){
	     return t>>5|t>>6|t>>1;
	   });
	   
	 clearScreen = CLEAR_BLACK;

	 scope.scene.init();
	 scope.scene.camera.setRotation( 24, 10, 0 ).translate( 60, -80, 130 );

	 scope.racerCount = 3-(winCount&1);

	 for( int i=0; i<scope.racerCount; ++i ){
	   int place = 0;
	   auto laps = racers[i].laps+1;
	   if( racers[i].dead )
	     laps--;

	   for( int j=0; j<scope.racerCount; ++j ){
	     if( j==i )
	       continue;
	     
	     auto olaps = racers[j].laps+1;
	     if( racers[j].dead )
	       olaps--;
	     
	     if( olaps > laps )
	       place++;
	   }

	   if( !i ){
	     raceCount++;
	     if( place == 0 ){
	       scope.msg = txtFirstPlace;
	       winCount++;
	       playerMoney += raceCount*raceCount*7+3;
	     }else{
	       scope.msg = txtOtherPlace;
	       loseCount++;
	       playerMoney += raceCount*2+1;
	     }
	   }

	   Node &node = scope.scene.initNode(
					     i?mesh:hero_mesh,
					     i?(sizeof(mesh)/sizeof(mesh[0])):
					     (sizeof(hero_mesh)/sizeof(hero_mesh[0]))
					     );
	   node.y = 0;
	   node.z = (i-1)*80;
	   node.x = -place*100;
	   node.rotY = 64;
	 }

       },
       
       {
	 arduboy.setCursor(40,0);
	 arduboy.print( fstr(scope.msg) );
	  
	 if( justPressed(A_BUTTON) ){
	   changeState( State::RenderMode, 0xAA );
	 }
	 
	 for( int i=0; i<scope.racerCount; ++i )
	   scope.scene.nodeList[i].y = Fixed::fromInternal(SIN(i*27+arduboy.frameCount))*10;

	 scope.scene.update();
	 
       }
       
)
	   
