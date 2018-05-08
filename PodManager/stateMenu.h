STATE( MenuMode,
       
       {
	 Scene36_3 scene;
	 uint8_t option;
       },
       
       {

	 scope.option = 0;
	 scope.scene.init();
	 scope.scene.camera.setRotationX( 0 ).translate( 0, -15, 50 );

	 Node &node = scope.scene.initNode(
					   hero_mesh,
					   sizeof(hero_mesh)/sizeof(hero_mesh[0])
					   );
	   
	 clearScreen = CLEAR_BLACK;
	 playChiptune([](uint16_t t){
	     return (t>>8)&~(t>>5|t>>6|t>>1);
	   });
	 	 
       },
       
       {

	 auto &option = scope.option;
	 const auto maxOpt = sizeof(optionLabels)/sizeof(optionLabels[0]);

	 if( justPressed(B_BUTTON) )
	   state = State::RenderMode;

	 if( justPressed(LEFT_BUTTON) ){
	   if( !option ) option = maxOpt-1;
	   else option--;	   
	 }else if( justPressed(RIGHT_BUTTON) ){
	   option++;
	   if( option == maxOpt )
	     option = 0;
	 }

	 auto label = pgm_read_word(&optionLabels[option]);

	 arduboy.setCursor( 64 - strlen_P(label)*3,0);
	 arduboy.println( fstr( label ) );

	 switch( option ){
	 case 0:
	   arduboy.print(fstr(txtWins));
	   arduboy.println(winCount);
	   arduboy.print(fstr(txtLoses));
	   arduboy.println(loseCount);

	   if( justPressed(A_BUTTON) )
	     state = State::RaceMode;
	   break;
	   
	 default:
	   uint8_t &part = reinterpret_cast<uint8_tp>(&playerUpgrades)[ option-1 ];
	   uint16_t part2 = part;
	   part2 *= part2;
	   uint16_t cost = part2*10+40;
	   uint16_t sell = part2*7+30;

	   if( justPressed(UP_BUTTON) && playerMoney >= cost ){
	     playerMoney -= cost;
	     part++;
	   }else if( justPressed(DOWN_BUTTON) ){
	     playerMoney += sell;
	     part--;
	   }else{
	     if( part < 255 ){
	       arduboy.print( fstr(txtUpCost) );
	       arduboy.println( cost );
	     }
	     if( part > 1 ){
	       arduboy.print( fstr(txtDownCost) );	   
	       arduboy.println( sell );
	     }
	   }
	 }

	 arduboy.setCursor( 0, 64-7 );
	 arduboy.print( fstr(txtMONEY) );
	 arduboy.print( playerMoney );
	 
	 auto &node = scope.scene.nodeList[0];
	 node.y = Fixed::fromInternal(SIN( arduboy.frameCount ))*3;
	 node.rotY -= (node.rotY - 37*option) >> 2;
	 scope.scene.update();
	 
       }
       
)
