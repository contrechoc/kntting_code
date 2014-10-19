void highlightActiveTour(int whichTour) {
  int c = 0;
  int d = 3;
  fill(255, 255, 0, 100);
  rect( imageOffSetX+ c*d + d, imageOffSetY+ (whichTour+1)* d, imageWidth*d, d );


  if ( whichTour>imageHeight-2)
    end.trigger();
  else
    snare.trigger();
}


void drawTestImage(String inputString) {
  //println ("drawTestImage "+ inputString);
  int c = 1;
  int d = 3;
  
  for ( int i = 1; i < 200 ; i++) {
    //indicators
    if ( (i-imageWidth/2)%10 == 0) {
      if ( i < imageWidth) {
        fill (200, 0, 0);
        rect ( imageOffSetX+ i*5, imageOffSetY+ 5 * d - 40, d, d*2 );
        if ( (i-imageWidth/2) >= 0)
        text ( (int ((i-imageWidth/2)/10))*10, imageOffSetX+ i*5, imageOffSetY+ 5 * d - 44);
     else
      text ( (int ((i-imageWidth/2)/10))*10, imageOffSetX+ i*5 - 7, imageOffSetY+ 5 * d - 44);
    
 }
    }
    
     if ( i == imageWidth / 2) {
    
        fill (200, 200, 0);
        rect ( imageOffSetX+ i*5, imageOffSetY+ 5 * d - 40, d, d*3 );
      
    }
    
//indicators
    if ( (rowNumberForImage)%10 == 0) {
      fill (200, 0, 0);
      rect ( imageOffSetX - 10, imageOffSetY+ rowNumberForImage * d, d*2, d );
       text (rowNumberForImage, imageOffSetX - 25, imageOffSetY+ rowNumberForImage * d + 5);
    }

    int whichNumer = int(inputString.substring(i, i+1));
    if ( whichNumer == 1) {
      fill(0, 0, 200);
      rect( imageOffSetX+ c*d, imageOffSetY+ rowNumberForImage * d, d, d );
      c++;
    }
    if ( whichNumer == 0) {
      fill(255);
      rect( imageOffSetX+ c*d, imageOffSetY+ rowNumberForImage * d, d, d );
      c++;
    }
  }
}

void drawKnitImage(String inputString, int whichSide) {
  println ("drawKnitImage "+ inputString);
  int c = 0;
  int d = 3;
  for ( int i = 0; i < 200 ; i++) {
    int whichNumer = int(inputString.substring(i, i+1));
    if ( whichNumer == 0) {
      fill(0);
      rect(  imageOffSetX+ c*d, imageOffSetY+ rowNumberForImage * d, d, d );
      c++;
    }
    if ( whichNumer == 1) {
      fill(255);
      if ( whichSide == 0 ) fill ( 255, 0, 0 );
      if ( whichSide == 1 ) fill ( 255, 255, 0 );
      rect(  imageOffSetX+ c*d, imageOffSetY+ rowNumberForImage * d, d, d );
      c++;
    }
  }
}

