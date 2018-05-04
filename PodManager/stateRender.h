STATE( RenderMode,
       
       {
	 uint16_t zBuffer[32];
	 Node nodeList[5];
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
	 scope.camY = 100;
	 scope.camZ = -100;
	 scope.camRX = -20;
	 scope.camRY = 0;
	 scope.camRZ = 0;

	 uint8_t vertexCount = 0;
	 for( uint8_t i=0; i<5; ++i ){
	   Node &node = scope.nodeList[i];
	   node.init(
		     -100 + i*40, 0, 10+i*50,
		     0, 0, 0,
		     2, mesh
		     );
	   for( uint8_t v=0; v<node.vertexCount; v++ ){
	     scope.zBuffer[vertexCount++] = (v<<8) | i;
	   }
	 }
	 
	 for( int i=vertexCount; i<sizeof(scope.zBuffer)/sizeof(scope.zBuffer[0]); ++i ){
	   scope.zBuffer[i] = 0xFFFF;
	 }

	 clearScreen = CLEAR_GRAY;
	 playChiptune([](uint16_t t){
	     return (t>>4)|(t>>8)|(t>>12);
	   });
	 
       },
       
       {

	 Matrix cam;

	 cam.setRotation( -scope.camRX, -scope.camRY, -scope.camRZ );
	 cam.translate( -scope.camX, -scope.camY, -scope.camZ );

	 scope.angle += 7;
	 scope.angle2++;

	 for( uint8_t i=0; i<sizeof(scope.nodeList)/sizeof(scope.nodeList[0]); ++i ){
	   Node &node = scope.nodeList[i];
	   node.rotY += i+1;
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

	     Fixed fovz = 64 + tmp.z;
	     tmp.x = (64 * tmp.x) / fovz + 64;
	     tmp.y = 32 - (64 * tmp.y) / fovz;

	     if( i && prevZ < tmp.z ){
	       scope.zBuffer[i] = scope.zBuffer[prevI];
	       scope.zBuffer[prevI] = index;
	     }
	     prevZ = tmp.z;
	     prevI = i;
	   
	     auto y = tmp.y.getInteger();
	     auto x = tmp.x.getInteger();
	     auto size = ((64 * tmp.size) / fovz).getInteger(); // ((1024-tmp.z)*tmp.size/1024).getInteger();
	     auto hsize = size>>1;
	     auto sx = x - hsize;
	     auto sy = y - hsize;
	     auto ex = sx + size;
	     auto ey = sy + size;
	     if( sx < 0 ) sx = 0;
	     if( ex > 128 ) ex = 128;
	     if( sy < 0 ) sy = 0;
	     if( ey > 64 ) ey = 64;
	     
	     if( sx>=ex || sy>=ey )
	       continue;

	     /* * /
	       ey -= sy;     
	     for( x=sx; x!=ex; ++x ){
	       arduboy.drawFasterVLine( x, sy, ey, tmp.color );
	     }
	     /*/
	     arduboy.fillCircle( x, y, hsize, tmp.color );
	     /* */
	     
	   }

	 }
	 
       },

       void renderPoint( const Point3D &p ){
 	
	 
       }
       
)
