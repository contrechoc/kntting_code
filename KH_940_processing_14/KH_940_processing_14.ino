/*

 code for KH-940 knitter
 still very much work in progress
 
 https://github.com/contrechoc/kntting_code
 
 */


const int encoderPinA = 5;
const int encoderPinB = 3;
const int encoderPinC = 4; // solenoids

volatile int interruptA = 0;
volatile int interruptB = 0;
volatile int interruptC = 0;

volatile int oldNumCode = 0;
volatile int dirSlider = 0;

volatile int myStitch = -28; //needle counter
volatile int oldMyStitch = -28; //needle counter
volatile int stitch4Counter = 1; //sensor CHANGE counter

volatile int stopStart = 0;

volatile int serialPrint = 0;

volatile int highLow = 0;
volatile int needleDiff = 0;

volatile int stopLimit = 0;

volatile int imageDone = 0;

int oldA = 0;
int oldB = 0;
int oldC = 0;

int shiftFactorL = 0;
int shiftFactorR = 0;

long timer;
long interval = 2000;
int firstTimer = 0;

int internalImage = 0;

int imageWidthAskedFor = 0;

int imageHeight = 0;
int imageWidth = 0;

int pixelArrayCounter = 0;

char turningNeedlePoint = 1;

volatile int arrayCounter = 0; //starting at row 4, somehow the first rows 0,1,2,3 contain strange values

#define totalArrayFromSelenoids 16
volatile int amegaPinsArray[totalArrayFromSelenoids] = {
  22,24,26,28,30,32,34,36,37,35,33,31,29,27,25,23                                                                                                               };

const int imageRows = 25;//200 divided by 8 
const int imageTours = 205;//height
unsigned char processingImage[imageRows][imageTours];// needles in one row: imageRows - number of lines in a knitting: imageTours

// image for testing if in the mode to knit the image from this code only
int pixelTestImage[2][60] = {
  {  
    1,1,1,1,0,0,1,0,1,1,
    0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,0,1,

    1,0,0,0,0,0,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,
    1,1,0,1,0,0,1,1,1,1,                        }
  ,
  {
    1,1,1,1,0,0,1,0,1,1,
    0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,1,1,0,

    0,1,1,0,0,0,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,
    1,1,0,1,0,0,1,1,1,1,                        }
};


// pixels for the tour to be knitted, will be refreshed at turning points
int pixelBin[256] = {

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,

  1,1,1,1,0,0,1,0,1,1,
  0,0,0,0,0,0,0,0,0,0,
  1,1,1,0,0,0,0,0,0,1,

  1,0,0,0,0,0,0,1,1,1,
  0,0,0,0,0,0,0,0,0,0,
  1,1,0,1,0,0,1,1,1,1,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,              

  0,0,0,0,0,0,0,0,

  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};



void setup() {

  Serial.begin(115200);
  Serial.println(" Knitter prep");

  //initializing the picture memory
  for (int i = 0; i<imageRows; i++)
    for (int j = 0; j< imageTours; j++)
      processingImage[i][j] = B00000001;

  //PIN for a speaker attached to give sound signals
  pinMode(7, OUTPUT);

  //needle sensors pinMode
  pinMode(encoderPinA,INPUT);
  pinMode(encoderPinB,INPUT);
  pinMode(encoderPinC,INPUT);

  //solenoids pinMode
  for(int i=0; i<16; i++){
    pinMode(amegaPinsArray[i], OUTPUT);
    digitalWrite(amegaPinsArray[i], 0);
  }

  //button for transfer control
  pinMode(8, INPUT);

  timer = millis()+interval;
  delay(10);
  giveSireneB( 90);
  // Serial.println(" Hello 2");
  delay(10);

  //defining the interrupt functions
  attachInterrupt(encoderPinA, counterAUp, CHANGE);
  attachInterrupt(encoderPinB, counterBUp, CHANGE);
  attachInterrupt(encoderPinC, counterCUp, CHANGE );

  Serial.println(" Prep ready");
}

//function used to knit a circle in the mode of knitting an image from inside this script
int dist(int x, int y, int m, int n , int r){

  if (  ( (x-m)*(x-m) + (y-n)*(y-n) ) < r*r )
    return 0;

  return 1;

}

// interrupt functions
void counterAUp(){
  if ( firstTimer == 1 ){
    if ( digitalRead (encoderPinA)==HIGH )
      printCounters(1);
    else 
      printCounters(3);
  }
}
void counterBUp(){
  if ( digitalRead (encoderPinB)==HIGH )
    printCounters(4);
  else 
    printCounters(2);
}

void counterCUp(){

  if ( stopStart == 0 )
    if (abs( oldMyStitch - myStitch) > 7){
      stopStart = 1;
    }
  oldMyStitch = myStitch;

}


void loop() {

  //start transferring data after some time to settle
  if ( timer < millis() &&  firstTimer == 0){ //needed for avoiding a first interrupt
    giveSound(8000);
    timer = millis()+interval;
    getImageDataFromProcessingAfterWakingUp();
  }

  //to make sure the direction is only reversed after passing the middle 
  if ( abs( myStitch - 100 ) < 5 ) {
    turningNeedlePoint = 1;//passed the middle point
    giveSound(7500);
  }

  //give some sounds at ARduino speaker after finishing the image
  if ( imageDone == 1 ){

    giveSirene(5000);
    delay(100);
    giveSound(5000);
    delay(100);
    giveSound(5000);
    delay(100);
    giveSound(5000);
    delay(100);

  }
}

//
void getImageDataFromProcessingAfterWakingUp(){

  delay(50);
  getPixelArrayFromProcessing();
  delay(50);
  //for showing a picture in Processing
  for ( int i = 0; i < imageHeight - 3; i++){
    delay(5);
    changeArray(0);
  }
  delay(5);
  Serial.println("@");
  delay(5);
  firstTimer = 1;
  arrayCounter = 0;///4th row to start
}



void printCounters(int numCode){ //on an interrrupt

  //left - right pos
  if (  (numCode == 1 ) &&  (analogRead(1) > 500) && (digitalRead(encoderPinB) == HIGH)){
    Serial.print( " right  ");
    myStitch = 0;//wiil be ++ ed
    stopLimit = 1; //start tracking how much there is in the solenoid HIGH or LOW's
    needleDiff = 0;
    if ( digitalRead(encoderPinC) == HIGH) highLow = 1; 
    else  highLow = 0;
  }
  if (  (numCode == 3 ) &&  (analogRead(0) > 500) && (digitalRead(encoderPinB) == HIGH))
  {
    Serial.print( " --> left    ");
    myStitch = 201; //will be -- ed
    stopLimit = 1; //start tracking how much there is in the solenoid HIGH or LOW's
    needleDiff = 0;
    if ( digitalRead(encoderPinC) == HIGH) highLow = 1; 
    else  highLow = 0;
  }

  if ( stopLimit == 1){
    if ( highLow != digitalRead(encoderPinC) ){
      stopLimit = 0;
      Serial.print("d-ep = ");
      Serial.println(needleDiff);
    } 
    else {
      if (  numCode == 1 ) {
        needleDiff++;
        Serial.print("d-ep= ");
        Serial.println(needleDiff);
      }
    }
  }


  if ( numCode == 1 )
  {
    if (   dirSlider == 1){//direction to the left
      if (    (myStitch > 0) )
      {
        {
          if ((needleDiff==1)  &&(highLow == 1) ) digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch  - 8)]);
          if ((needleDiff==1) &&(highLow == 0) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch  - 8 )]);
          if (needleDiff==2 &&(highLow == 1) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch - 8 )]);//8 green with 0
          if ((needleDiff==2) &&(highLow == 0) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch - 16)]);
        }
      }
    }
    else
    { //direction to the right
      if ( ( myStitch < 200)  )
      {
        {
          if ((needleDiff==1) &&(highLow == 0) )   digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch-16)]);//0 ->
          if ((needleDiff==1) &&(highLow == 1) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch+8)]);
          if ((needleDiff==2) &&(highLow == 0) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch + 16)]);//0 was 16 left
          if ((needleDiff==2) &&(highLow == 1) )  digitalWrite(amegaPinsArray[myStitch%16], 1 - pixelBin[(myStitch + 8)]);
        }
      }
    }
  }


  //looking for a pattern 1,2,3,4 or 4,3,2,1
  // if there is a jump then it can be returning the slider
  // but from 4 to 1 it is continuing
  if ( abs( oldNumCode - numCode)>1 ) {//possible return
    if ( abs( oldNumCode - numCode)!=3 ) {//continuing


      if ( dirSlider == 0 ){//right
        dirSlider = 1;
        //  Serial.println( "r" );
        if ( turningNeedlePoint == 1 ) {//passed the middle point to prevent getting new tours when you go to and fro at the side

          changeArray(1);
          turningNeedlePoint = 0;
        }
      }
      else{//left
        dirSlider = 0;
        // Serial.println("l" );
        if ( turningNeedlePoint == 1 ) //passed the middle point to prevent getting new tours when you go to and fro at the side
        {
          changeArray(2);
          turningNeedlePoint = 0;
        }

      }
    }
    else
      stitchCounting();
  }
  else // stitchCounting
  {
    stitchCounting();
  }
  oldNumCode = numCode;


}

void  changeArray(int controlOutput)
{
  if ( internalImage == 1){
    int c = 0;
    for (int i = 100 - 30; i < 100 + 30; i++){
      pixelBin[i] = pixelTestImage[arrayCounter][c++]; 
    }
    arrayCounter++;
    if ( arrayCounter > 1) arrayCounter = 0;
  }
  else
  {
    // image from processing

    int c = 0;//imageWidth
    int d = 0;

    // needles in one row: imageRows - number of lines in a knitting: imageTours

    for (int i = 0; i < 200; i++){
      if ( (i > 100 - imageWidth/2 -1) && (i < 100 + imageWidth/2 )){
        char mychar = processingImage[d][arrayCounter]; 
        pixelBin[i] = ((mychar >> c ) & 1);
        c++;
        if ( c > 7 ) {
          d++;
          c = 0;
        }
      }
      else{
        pixelBin[i] = 7;
      }
    }
    arrayCounter++;
    if ( arrayCounter > imageHeight) {
      if ( controlOutput > 1 ) imageDone = 1;
      arrayCounter = 1;
    }
  }
  if ( firstTimer == 0 ) //blocking too much communications
  {
    //printing pixelbin
    if ( controlOutput == 0)  Serial.print("#");
    if ( controlOutput == 1)  Serial.print("&");
    if ( controlOutput == 2)  Serial.print("$");
    for (int i = 0; i < 200; i++){
      Serial.print(  pixelBin[i] );
    }
  }
  Serial.print( "+" );
  Serial.println(  (arrayCounter - 1) );

}

void stitchCounting(){

  if ( dirSlider == 1 ){
    stitch4Counter++;
    if ( stitch4Counter > 4 ) {
      myStitch++;
      stitch4Counter = 1;
    }
  }
  else{
    stitch4Counter--;
    if ( stitch4Counter < 1 ) {
      myStitch--;
      stitch4Counter = 4;
    }
  }
}

//sound functions for speaker
void giveSound(int beats){

  for (int i=0;i<30;i++){
    digitalWrite(7, HIGH);

    for(int i2 = 0 ; i2<beats ;i2++) //do this 100 times, so lowering this number gives a higher pitch!
      __asm__("nop\n\t"); 

    digitalWrite(7, LOW);
    for(int i2 = 0 ; i2<beats ;i2++) //do this 100 times, so lowering this number gives a higher pitch!
      __asm__("nop\n\t"); 
  }
}

//sound functions for speaker
void giveSirene(int b){
  int beats = 9600;
  for (int i=0;i<100;i++){
    beats-=10;
    digitalWrite(7, HIGH);
    for(int i2 = 0 ; i2<beats ;i2++)  
      __asm__("nop\n\t"); 
    digitalWrite(7, LOW);
    for(int i2 = 0 ; i2<beats ;i2++) 
      __asm__("nop\n\t"); 
  }
}

//sound functions for speaker
void giveSireneB(int b){
  int beats = 9000;
  for (int i=0;i<100;i++){

    beats+=10;
    digitalWrite(7, HIGH);

    for(int i2 = 0 ; i2<beats ;i2++) //do this 100 times, so lowering this number gives a higher pitch!
      __asm__("nop\n\t"); 

    digitalWrite(7, LOW);
    for(int i2 = 0 ; i2<beats ;i2++) //do this 100 times, so lowering this number gives a higher pitch!
      __asm__("nop\n\t"); 
  }
}













