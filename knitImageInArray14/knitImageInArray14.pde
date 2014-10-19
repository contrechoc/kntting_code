/*

code for KH-940 knitter
still very much work in progress


https://github.com/contrechoc/kntting_code

*/


import ddf.minim.*;

Minim minim;
AudioSample kick;
AudioSample snare;
AudioSample end;

//
PFont f, fSmall;

String imageName = "mppt_weave.jpg";

import processing.serial.*; 

Serial myPort;    // The serial port
String inString = "";  // Input string from serial port
int lf = 10;      // ASCII linefeed 
boolean inputFlag = false;

int imageHeight = 0;
int imageWidth = 0;

int currentRow = 0;
int rowNumberForImage = 0;

PImage img;

int[][] imageData ;


int imageOffSetX = 80;
int imageOffSetY = 160;

void setup() {

  size(640, 1000);
  
  // Create the font
  f = createFont("Arial-BoldMT-24", 18);
  fSmall = createFont("ArialMT-10.vlw", 10);
  textFont(f);
 
  makeSerialConnection();

  minim = new Minim(this);

  // load BD.wav from the data folder
  kick = minim.loadSample( "BD.mp3", 512    );
  if ( kick == null ) println("Didn't get kick!");

  // load SD.wav from the data folder
  snare = minim.loadSample("SD.wav", 512);
  if ( snare == null ) println("Didn't get snare!");

  end = minim.loadSample("end.wav", 512);
  if ( end == null ) println("Didn't get end!");

  image_load();
  

}



void draw() {

  if ( inputFlag ) {
    inputFlag = false;
    inString = inString.replace("\n", "").replace("\r", "");
    //println ( "From knitter " + inString );

    if ( inString.equals("i") )
    {
      sendImageWidth();
    }

    //output giving a row of the image
    if ( inString.equals("w") )
    {
      nextLineOfImage();
    }

    //output giving a row of the image
    if ( inString.equals("^") )
    {
      println( "image bigger than arduino mem array");
      end.trigger();
    }

    //output giving a row of the image
    if ( inString.equals("@") )
    {
      rowNumberForImage = 0; //reset response Image line
    }

    if ( inString.length() > 1)
      if ( inString.substring(0, 1).equals("+") )
      {
        if ( inString.length() == 2)
          highlightActiveTour(int(inString.substring(1, 2)));
        else  if ( inString.length() == 3)
          highlightActiveTour(int(inString.substring(1, 3)));
        else  if ( inString.length() == 4)
          highlightActiveTour(int(inString.substring(1, 4)));

        //println ("highlightActiveTour" + inString);
      }

    //row comes back to check
    if ( inString.length() > 5)
      if ( inString.substring(0, 1).equals("#") )
      {
        rowNumberForImage++;
        drawTestImage(inString);
      } 
      else if ( inString.substring(0, 1).equals("&") ) {
        drawKnitImage(inString, 0);
        rowNumberForImage++;
      }  
      else if ( inString.substring(0, 1).equals("$") ) {
        drawKnitImage(inString, 1);
        rowNumberForImage++;
      }
  }
}

