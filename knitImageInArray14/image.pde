void  nextLineOfImage() {
  inString = "";
  //print( "knitter askes for next line : ");

  if ( currentRow < imageHeight) {

    // needles in one row: imageRows - number of lines in a knitting: imageTours
    int width8 = 1 + int(imageWidth/8);//1 needed for info in last 8 bits character
    float width8float = imageWidth/8;
    //if ( width8float - width8 > 0 ) width8++;
    char vc = 0;
    for (int j = 0; j < 200/8; j++) {
      if ( j < width8 ) {
        char myChar = 0;
        String pS = "";
        for (int i = 0; i < 8; i++) {
          if ( imageData[currentRow][j*8+i] == 0) {
            pS += "1";
            myChar +=1;
          }
          else {
            pS += "0";
          }
          myChar*=2;
          //int nop = 0;//test linech
        }
        myPort.write( (myChar/2) &0xFF);
        // print ( pS );
      }
      else
        myPort.write( 0 );

      //print(" ");
    }

    currentRow++;
  }
  //else
  //  println("imageDone");
}

void sendImageWidth() {
  inString = "";
  print( "knitter askes for image width ");
  int im100 =   (imageWidth/100);
  int im10 =   ((imageWidth - im100*100)/10);
  int im1 =   ((imageWidth - im100*100  - im10*10));

imageHeight+=4;

  int ip100 =   (imageHeight/100);
  int ip10 =   ((imageHeight - ip100*100)/10);
  int ip1 =   ((imageHeight - ip100*100  - ip10*10));

  //checking
  //println ( "Processing sends " + str(im100) + " - " + str(im10) + " - " + str(im1));

  myPort.write(str(im100));
  myPort.write(str(im10));
  myPort.write(str(im1));

  myPort.write(str(ip100));
  myPort.write(str(ip10));
  myPort.write(str(ip1));

  int shiftFactor = 1;
  myPort.write(str(shiftFactor));
}


void image_load() {

  int printImage = 0; //for println in debug window put this on 1

 fill(0, 0, 200, 100);
rect( imageOffSetX - 40, 30 , 600 , 80 );
fill(0);
 text( "Image: " +imageName, imageOffSetX + 50, 20 + 40);
 
  img = loadImage(imageName);
  img.loadPixels();
  loadPixels();

  imageHeight = img.height;
  imageWidth = img.width;
  
  text( "Height x Width: " + imageHeight + " x " + imageWidth, imageOffSetX + 50, 50 + 40);

  textFont(fSmall);
   fill(0, 0, 200, 50);
  rect( imageOffSetX - 40, imageOffSetY - 40 , 600 , 600 );
  
  int cols = imageHeight;
  int rows = imageWidth+ 8;// + (8 - imageWidth%8 ) ;
  imageData = new int[cols+4][rows];
  int[][] imageDataMirror = new int[cols][rows];

  // Two nested loops allow us to visit every spot in a 2D array.   
  // For every column I, visit every row J.
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      imageData[i][j] = 1;
    }
  }

  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      imageDataMirror[i][j] = 1;
    }
  }


  for (int y = 0; y < imageHeight; y++ ) {
    int eightCounter = 0;
    if ( printImage == 1 ) print( y + " " );
    for (int x = 0; x < imageWidth; x++) {
      // Calculate the 1D location from a 2D grid
      int loc = x + y*imageWidth;
      // Get the R,G,B values from image
      float r, g, b;
      r = red (img.pixels[loc]);

      int pixel = 1;
      if ( r < 100) pixel = 0;
      imageDataMirror[y][x] = pixel;
      //if ( eightCounter++ % 8 == 0 ) print (" ");
      //g = green (img.pixels[loc]);
      //b = blue (img.pixels[loc]);
      if ( printImage == 1 ) {
        if ( r < 150 ) print (1); 
        else print (0);
      }
    }
    if ( printImage == 1 ) println("");
    //if ( printImage == 1 ) println("");
  }

  //mirror image
  for (int y = 0; y < imageHeight; y++ ) {
    for (int x = 0; x < imageWidth; x++) {
      imageData[y][x] = imageDataMirror[y][imageWidth -1 - x];
    }
  }
  
 // imageHeight+=4; //sending ssems to disrupt the first 4 lines
  
  if ( printImage == 1 ) println("----");
  if ( printImage == 0 ) println( img.height, img.width);

}

