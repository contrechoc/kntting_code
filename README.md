kntting_code
============
The first undocumented version of an Arduino Script and a Processing script I use 
for making my Brother KH-940 knit my datavisualisations in different knitting techniques.

see:
http://myfablab.wordpress.com/2014/10/11/writing-my-own-software-for-the-kh-940/
http://myfablab.wordpress.com/2014/10/11/doing-invisible-knitted-datavisualisation/


Be careful: the communications are probably device dependent, the knitic code didn't work for my machines. 
That's why i wrote my own. This software could just not work on yours because of the same reason!

The connections/board are in principle the http://www.knitic.com/ board (but for one PIN):

for needle PIN's: (my PIN 2 is fried.)
  digital: 3,4,5
Solenoids array:
  digital: 22,24,26,28,30,32,34,36,37,35,33,31,29,27,25,23
right/left solenoid sensors:
  analog 0,1
  
Programming concept:
All pixels are sent in CHAR's one pixel is one bit.
Send as less information as possible beteen the Arduino and Processing

Preparation:
Processing: 
  step 1: sample image and to be send to Arduino in char code that is 8 bits of information (pixels) in one char.
    step 1a: add 4 lines in fron of the image - see detail 2
  step 2: waiting for the arduino signalling start sending image
  step3: sending image line by line after a signal from the arduino
  step 4: receiving the control image and displaying this image, waiting for knitting to start

Arduino: 
  step 1: asking for the image after some start up time
  step 2: asking for img width and height
  step 3: asking for image line by line (tours) in char code.
  step 4: sending back a check of the image
  
Knitting: (starts at right only at the moment)
Arduino:
  step 1: reaction on right sensor: send one line to knitter
  step 2: count stitches from needle sensor information 1,2,3,4 rithm, done by interrupt functions
  step 3: react om passing the middle
  step 4: changing direction (deduction from 1,2,3,4 reversal)
  step 5: send line to processing
  
Processing:
  step 1: receive knitted line, displaying as a transparent overlay in the image, no information of needle count is displayed
  step 2: sound signal after direction reversal
  step 2: after the knitting is done: sound signal to indicate end of image
  
Detail 1:
critical in the Arduino code are the (hard coded delays)
eg in the loop to receive the image:

  delay(10);
  Serial.println("w");///sending code to ask for image width
  delay(7);//necessary, otherwise loss of data
  
change these values and information can be lost

Detail 2:
The first 4 lines transferred contained random errors, from the 4th line on information was processed without errors.
This was solved in a rude manner: insert 4 extra lines at the start of the image and ignore these 4 lines in knitting.

Detail 3:
The image is kept in the memory of the Arduino sketch. This was not possible in INTEGER format. In CHAR format it is, but of course the image can in principle have an infinite length (while width has a max of 200). Also the CHAR array has boundaries. At the moment the maximum value is: 200 tours (lines of knitting).

Detail 4: the KH-940 needle sensor information.
With interrupts the needle information is processed. There are two signals alternating for each needle. These were given a code 1,2,3,4 and from this the direction is deduced. When reversing direction counting is done backwards, not losing any needles.

Detail 5: The right and left sensor.
The right sensor is used not for putting the needle count on 0, but for deducing the distance between the needle 0 and the position wehre the carriage combines with the belt - solenoids offset. By trial and error was deduced that there are 4 significant combining positions, which can be reduced even further, with a special offset.
After introducing these belt offsets, the image didn't shift anymore.

Detail 6: does this mean that I exclude knitting direct sensor information? No, because a sensor connected to the Arduino can influence the row to be knitted. But because anyway, you have to correct for direction of knitting when doing eg a graph, you have to program this case individually independent of the background image sent from the laptop.


    
  
  
  
  




