STATE( RenderMode,
       
       {
	 uint16_t zBuffer[32];
	 Node nodeList[2];
	 Fixed camX;
	 Fixed camY;
	 Fixed camZ;
	 Fixed camRX;
	 Fixed camRY;
	 Fixed camRZ;
	 uint8_t angle;
	 uint8_t angle2;
       },
       
       {

	 
	 scope.angle = 0;
	 scope.angle2 = 0;

	 scope.camX = 0;
	 scope.camY = 150;
	 scope.camZ = -200;
	 scope.camRX = -30;
	 scope.camRY = 0;
	 scope.camRZ = 0;

	 uint8_t vertexCount = 0;
	 for( uint8_t i=0; i<sizeof(scope.nodeList)/sizeof(scope.nodeList[0]); ++i ){
	   Node &node = scope.nodeList[i];
	   node.init(
		     -100 + i*100, 0, 10+i*100,
		     0, 0, 0,
		     sizeof(mesh)/sizeof(mesh[0]), mesh
		     );
	   for( uint8_t v=0; v<node.vertexCount; v++ ){
	     scope.zBuffer[vertexCount++] = (v<<8) | i;
	   }
	   node.rotY = i * 20;
	 }
	 
	 for( int i=vertexCount; i<sizeof(scope.zBuffer)/sizeof(scope.zBuffer[0]); ++i ){
	   scope.zBuffer[i] = 0xFFFF;
	 }

	 clearScreen = CLEAR_NONE;
	 playChiptune([](uint16_t t){
	     return (t>>4)|(t>>8)|(t>>12);
	   });
	 	 
       },
       
       {

	 // arduboy.drawBitmap( 0, 0, track_bitmap+2, 128, 64 );
	 memcpy_P( arduboy.sBuffer, track_bitmap+2, 128*64/8 );

	 const int8_t baseSpeed = 3;
	 const int8_t gravity = 2;

	 for( uint8_t i=0; i<sizeof(scope.nodeList)/sizeof(scope.nodeList[0]); ++i ){
	   Node &racer = scope.nodeList[i];
	   if( !i ){
	     
	     if( isPressed(A_BUTTON) ){
	       
	       racer.sx=1;
	       
	     }else if( justPressed(B_BUTTON) ){
	       
	       racer.sy=15;
	       racer.sx=baseSpeed*2;
	       
	     }
	     
	   }else{
	     // AI
	   }

	   if( racer.y <= 0 ){
	     if( racer.sx < baseSpeed )
	       racer.sx++;
	     if( racer.sx > baseSpeed )
	       racer.sx--;
	   }

	   racer.sy -= gravity;
	   racer.y += racer.sy;
	   if( racer.y < 0 && racer.sy < 0 ){
	     racer.sy = -(racer.sy>>1);
	     racer.y = 0;
	   }
		 
	   racer.rotY -= racer.sx;
	   racer.x = SINfp( racer.rotY+64 ) * 215;
	   racer.z = COSfp( racer.rotY+64 ) * 150;
	   racer.rotX = (SINfp( racer.rotY*3 )*5).getInteger();
	 }	 

	 Matrix cam;

	 cam.setRotation( -scope.camRX, -scope.camRY, -scope.camRZ );
	 cam.translate( -scope.camX, -scope.camY, -scope.camZ );

	 scope.angle += 7;
	 scope.angle2++;

	 for( uint8_t i=0; i<sizeof(scope.nodeList)/sizeof(scope.nodeList[0]); ++i ){
	   Node &node = scope.nodeList[i];
	   Matrix &mat = node.transform;
	   mat = cam;
	   mat.translate( node.x, node.y, node.z );
	   mat.rotate( node.rotX, node.rotY, node.rotZ );
	 }

	 cPoint3Dp prevptr = nullptr;
	 Fixed prevZ;
	 uint8_t prevI;

	 for( uint8_t i=0; i<sizeof(scope.zBuffer)/sizeof(scope.zBuffer[0]); ++i ){
	   uint16_t index = scope.zBuffer[i];
	   if( index == 0xFFFF )
	     continue;

	   Node &node = scope.nodeList[index&0xFF];

	   Point3D tmp;
	   
	   tmp.load( node.mesh[index>>8] );

	   tmp *= node.transform;

	   if( tmp.z > 0 ){

	     if( i && prevZ < tmp.z ){
	       scope.zBuffer[i] = scope.zBuffer[prevI];
	       scope.zBuffer[prevI] = index;
	     }
	     prevZ = tmp.z;
	     prevI = i;

	     tmp.render();
	     
	   }

	 }
	 
       },

       void renderPoint( const Point3D &p ){
 	
	 
       }
       
)
