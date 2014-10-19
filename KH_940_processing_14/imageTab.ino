

int getPixelArrayFromProcessing(){

  detachInterrupt(encoderPinA );
  detachInterrupt(encoderPinB );

  if ( imageWidthAskedFor == 0 ){
    //only now ask for image width
    getImageWidth();
    imageWidthAskedFor = 1;
  }

  delay(100);

  pixelArrayCounter = 0;

  if ( imageHeight > imageTours ) {
    imageHeight = imageTours;
    Serial.println("^");
  }
 

  //ask the whole image, each tour
  while( pixelArrayCounter < imageHeight )
  {
    delay(10);
    Serial.println("w");///sending code to ask for image width
    delay(7);//necessary, otherwise loss of data

    while (Serial.available() == 0);
    int counter = 0;
    delay(7);//necessary, otherwise loss of data
    while (Serial.available() > 0  ) {
      // read the incoming byte:
      unsigned char byteBuffer = Serial.read();
      unsigned char byteBufferRev = 0;

      for ( int i = 0 ; i < 8 ; i++)
        if ( (byteBuffer & (B00000001<<i)) == (B00000001<<i) ) byteBufferRev += (B10000000>>i);

      processingImage[counter][pixelArrayCounter] = byteBufferRev;
      counter++;
    }

    pixelArrayCounter++;

  }


  attachInterrupt(encoderPinA, counterAUp, CHANGE);
  attachInterrupt(encoderPinB, counterBUp, CHANGE);

  return imageDone;
}

void  getImageWidth(){

  int serialdata[10];
  for ( int i=0; i<10 ; i++)
  {
    serialdata[i]=0;
  }

  //delay(20);
  //Serial.println("coming asking for imagewidth");
  delay(20);
  Serial.println("i");
  delay(100);

  while (Serial.available() == 0);
  delay(50);
  int counter = 0;
  while (Serial.available() > 0) {
    // read the incoming byte:
    char byteBuffer = Serial.read();
    int value = 0;
    if(byteBuffer >= '0' && byteBuffer <= '9')  
    {     // is ch a number?  
      value = (value*10) +  (byteBuffer - '0'); 
      serialdata[counter++] = value;
    }
  }
  delay(30);
 

  imageWidth = serialdata[0]*100+serialdata[1]*10+serialdata[2];
  imageHeight = serialdata[3]*100+serialdata[4]*10+serialdata[5];
 
  delay(10);
  Serial.print(" image: w x h = ");
  delay(10);
  Serial.print( imageWidth);
  delay(10);
  Serial.print(" x ");
  delay(10);
  Serial.println(imageHeight);
  delay(500);

}



