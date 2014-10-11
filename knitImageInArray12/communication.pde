void makeSerialConnection(){
  //Serial connection
  println(Serial.list()); 

  myPort = new Serial(this, Serial.list()[2], 115200); 
  myPort.bufferUntil(lf);
}

void serialEvent(Serial p) { 
  inString = p.readString();
  inputFlag = true;
} 
