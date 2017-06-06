//Include the Pixy/SPI Libraries for Pixy Cam 
#include <SPI.h> 
#include <Pixy.h> 
#include <Servo.h> 

//Set up the pixy camera and the servo that moves the camera
Pixy pixy;
Servo Pixy_servo;

//Define variables related to the motors 
int Left_Motor = 2;
int Left_Motor_Dir = 3; 
int Right_Motor = 4;
int Right_Motor_Dir = 5; 
int Launch_Motor = 22;
int Conveyor_Motor = 26; 
 
//Define variables related to the input from pixycam 
int signature; 
int x; 
int y; 
float width; 
int height; 
//Pixy Varaibles for finding the dropoff points.
int bigtarget_x;
int bigtarget_y;
int bigtarget_j;
int bigtarget_width; 
int bigtarget_height; 
int big_signature;
int block_signature;
//Pixy variables for finding the blocks
int target_x;
int target_y;
int target_width;
int target_j;

//These variables are initialized for use in the for loops and several if statements, explained below.
int j=0;
int Search = 0;
int a=0;
int b=2;
//-------------------------------------------------------------------------------------------------------------------------------

//This section of the code runs once during powerup or after a reset of the arduino. 
void setup() {

//Begin serial communication with the arduino to allow for feedback if a computer is plugged into serial port
Serial.begin(9600); 
Serial.print("Starting...\n"); 

//Set-up the pins for the motor as outputs, all of these pins are digial and defined in the variable section above. 
pinMode(Left_Motor, OUTPUT); 
pinMode(Left_Motor_Dir, OUTPUT); 
pinMode(Right_Motor, OUTPUT); 
pinMode(Right_Motor_Dir, OUTPUT); 

//Set-up the pins to control the relay that controls the conveyor motor.
pinMode(Conveyor_Motor,OUTPUT); 
digitalWrite(Conveyor_Motor,HIGH); 

//Set-up the pin that controls the tilt servo that is attached to the pixy cam
Pixy_servo.attach(9); 

//Set the initial servo positon to 140, this puts the edge of the camera right at the front of the conveyor 
Pixy_servo.write(140);
delay(100);

//Initialize the pixy camera
pixy.init(); 
}

//------------------------------------------------------------------------------------------------------------------------------------------

//This portiton of the codes runs continously and is where the logic for indentifying, moving to and dropping the blocks is located
void loop() {

//Set the inital motor directions that allow the robot to drive forward when the Left_Motor and Right_Motor pins are brough HIGH
  digitalWrite(Left_Motor_Dir,LOW); 
  digitalWrite(Right_Motor_Dir,HIGH); 
  
//Set-up the variables used by the pixy to store information about the blocks seen   
  static int i=0;
  uint16_t blocks;
  char buf[32];
 
//This portion of the code is what moves the robot out of its corner position. The if statement only allows this logic to run once during the initial start-up of the robot.  
  if(a == 0){
//Output that the robot is driving out of the corner 
    Serial.println("Drive Out of Corner");
//Move the robot 45 degrees by turning on only the right motor and delaying for 1.5 seconds, then turn the motor off
   digitalWrite(Right_Motor,HIGH);
   delay(1500);
   digitalWrite(Right_Motor,LOW);

//Have the robot drive toward the center of the arena by turning on both motors for .2 seconds and then turn them off
   digitalWrite(Left_Motor,HIGH); 
   digitalWrite(Right_Motor,HIGH);
   delay(200);
   digitalWrite(Left_Motor,LOW); 
   digitalWrite(Right_Motor,LOW); 
   
//Set "a" to 1 to keep this loop from running again unless the arduino is reset.   
   a = 1; 
}
 
//The blocks variable stores the number of blocks seen by the pixy cam and is used in several for loops to pull information about the blocks
  blocks = pixy.getBlocks(); 

//This for loop runs through all of the blocks seen by the pixy and stores information about their x/y position, width,height, and signature (1-4) corresponding to the four colors.
for(j=0;j<blocks;j++){
  
signature = pixy.blocks[j].signature;
  x = pixy.blocks[j].x;  
  y = pixy.blocks[j].y; 
  width = pixy.blocks[j].width; 
  height = pixy.blocks[j].height;      

//Checks the "j" blocks information and checks to see if the object detected is a block, if it is the "j" value of the block is stored as "target_j", and the value of "j" is set to the number 
//of blocks to force the above for loop to stop. If the block does not match the widtha and height requirment the for loop continues to look through the objects seen until one matches. 
if(width<40 && height>10){
  target_j=j;
  j = blocks; 
 
//Prints the "J" value seen by the pixy and prints it to the serial port 
 Serial.print("J Value "); 
 Serial.println(target_j);
 
//Pulls the x/y position, width,height, and signature values for the block target so the robot can begin to move towards it.  
   target_x = pixy.blocks[target_j].x;
   target_y = pixy.blocks[target_j].y;
   block_signature = pixy.blocks[target_j].signature; 
   
//This section of the code uses the current position of the block being targeted and forces the robot to turn left, right or drive straight based on the blocks position in the pixy cameras frame. 
// The values used in the if loops were found from experimentation and do the best job of lining the block up in the conveyor. 
   if(target_x>170) {
    digitalWrite(Left_Motor,HIGH);
    delay(100); 
    digitalWrite(Left_Motor,LOW);
    Serial.println("Turn Right");     
        } 
        
        if(target_x<150){
          digitalWrite(Right_Motor,HIGH);
          delay(100); 
          digitalWrite(Right_Motor,LOW); 
          Serial.println("Turn Left"); 
        }
        
   if(target_x>149 && target_x<171) {
    digitalWrite(Left_Motor,HIGH);
    digitalWrite(Right_Motor,HIGH);
       delay(100); 
       digitalWrite(Left_Motor,LOW); 
       digitalWrite(Right_Motor,LOW); 
       Serial.println("Drive Straight");
  
      }
//If the width of the block seen by the robot is greater than 40 then it moves itself right to continue to scan for the blocks. The code below then scans left and right alternating each time
//so that the robot scans a good section of the arena each time to make sure that a block can always be found by the robot. 
if(width>40 && height<10){    

//If the remainder of b/2 is 0 then the robot scans right
  if(b%2 == 0){
  digitalWrite(Right_Motor_Dir,LOW);    
   digitalWrite(Left_Motor,255);
   digitalWrite(Right_Motor,255);
    delay(200); 
     digitalWrite(Left_Motor,LOW);
    digitalWrite(Right_Motor,LOW);    
    digitalWrite(Right_Motor_Dir,HIGH); 
    Serial.println("No Blocks Found - Scan Right");
  }

//If the remainder of b/3 is 0 then the robot scans left.
 if(b%3 == 0){  
     digitalWrite(Left_Motor_Dir,HIGH);    
   digitalWrite(Left_Motor,255);
   digitalWrite(Right_Motor,255);
    delay(200); 
     digitalWrite(Left_Motor,LOW);
    digitalWrite(Right_Motor,LOW);    
    digitalWrite(Right_Motor_Dir,LOW); 
    Serial.println("No Blocks Found - Scan left");
      }
      
//increases the value of b by one each time the if loop runs
    b=b +1;
}  
}
//This section of the code checks to see if the block is close enough to the robot to turn on the conveyor. If the block is centered and suffeciently close in the y direction 
//then the conveyor is turned on for a set delay and then turned back off. The control of the conveyor is done through a relay. 
if(target_y >= 170 && target_x>149 && target_x<171){
 digitalWrite(Conveyor_Motor,LOW);
     Serial.println("Conveyor On");
     delay(1200); 
     digitalWrite(Conveyor_Motor,HIGH);
   
//This sets a variable that forces the code to stay in the section of the code where the bins are found. It is initially set to 0 and is reset to that value once the block has been 
//dropped off in the bin. 
    Search = 1;

//This forces the if loop above to stop working and prevents the conveyor from being turned back on.   
    target_y = 100;
} 

//This section of the code finds the big target to drop the block off in. 
if(Search ==1) {

//The servo tilts the camera up to allow it to see the whole arena no matter where it might be. This allows the robot to spin in place and see where the correct big target is located.   
  Pixy_servo.write(120);
delay(100);
 
//Similar to the for loop used to find the blocks this for loop runs through the objects detected by the pixy camera  
for(j=0;j<blocks;j++){

//prints the value of the signature that is stored for the block that was most recently pick up.   
    Serial.print("Block Signature "); 
  Serial.println(block_signature); 

big_signature = pixy.blocks[j].signature;
  x = pixy.blocks[j].x;  
  y = pixy.blocks[j].y; 
  width = pixy.blocks[j].width; 
  height = pixy.blocks[j].height;      

//Prints the value of the color signature that the pixy camera is currently looking at as it searches for the correct big target
Serial.println(big_signature); 

//If the color signature that the pixy sees is the same as the block that was picked up and the object is larger than a block then the current j value is stored to "bigtarget" so that the 
//robot can begin to move toward the dropoff target. The value of j is also set to blocks to froce the for loop to stop. Otherwise the for loop continues to run. 
if(width>40 && block_signature == big_signature){
   bigtarget_j=j;
   j = blocks; 

//Prints the j value for big target so it can be seen on a computer that a target has been found. 
 Serial.print("Big Target J Value "); 
 Serial.println(bigtarget_j);

//Pulls the x/y position, width,height, and signature values for the dropoff target so the robot can begin to move towards it.  
   bigtarget_x = pixy.blocks[bigtarget_j].x;
   bigtarget_y = pixy.blocks[bigtarget_j].y;
   bigtarget_width = pixy.blocks[bigtarget_j].width; 
   bigtarget_height = pixy.blocks[bigtarget_j].height; 
   Serial.println(bigtarget_x); 

//This section of the code uses the current position of the bigtarget and gets the robot to turn left, right or drive straight based on its position in the pixy cameras frame. 
// The values used in the if loops were found from experimentation and do the best job of lining the robot up with the target.   
      if(bigtarget_x>170) {
    digitalWrite(Left_Motor,HIGH);
    delay(200); 
    digitalWrite(Left_Motor,LOW);
    Serial.println("Turn Right");     
        } 
        
        if(bigtarget_x<150){
          digitalWrite(Right_Motor,HIGH);
          delay(200); 
          digitalWrite(Right_Motor,LOW); 
          Serial.println("Turn Left"); 
        }
        
   if(bigtarget_x>149 && bigtarget_x<171) {
    digitalWrite(Left_Motor,HIGH);
    digitalWrite(Right_Motor,HIGH);
       delay(200); 
       digitalWrite(Left_Motor,LOW); 
       digitalWrite(Right_Motor,LOW); 
       Serial.println("Drive Straight");
      }

//If the robot is sufficently close to the drop off point then the drop off segment of the code starts.     
     if(bigtarget_y >=170) {
      Serial.println("Drop Block"); 
      delay(1000); 

//This inititiates a zero radius turn where the robot turns around in place to drop the block out of the back of the robot. The delay
//was found through experimentation 
    digitalWrite(Right_Motor_Dir,LOW);
    digitalWrite(Right_Motor,HIGH); 
    digitalWrite(Left_Motor,HIGH);
    delay(2500); 
    digitalWrite(Left_Motor,LOW);
    digitalWrite(Right_Motor,LOW); 
    digitalWrite(Right_Motor_Dir,HIGH);
    Serial.println("Turn Around");    
 
 //After turning the robot backs up slightly to make sure that the block is placed in the bin. 
       digitalWrite(Left_Motor_Dir,HIGH); 
       digitalWrite(Right_Motor_Dir,LOW);
       digitalWrite(Left_Motor,HIGH);
       digitalWrite(Right_Motor,HIGH);
       delay(300); 
       digitalWrite(Left_Motor,LOW); 
       digitalWrite(Right_Motor,LOW); 
       Serial.println("Reverse");
       digitalWrite(Left_Motor_Dir,LOW); 
       digitalWrite(Right_Motor_Dir,HIGH);   
         
//After the robot has turned around the conveyor is turned back on and the block rolls out of the back of the robot. The delay was found through experimentation 
//and is long enough to ensure the block has been ejected. The position of the pixy is then reset to its original position and the Search variable used to keep the robot 
//in the bigtarget section of the code is reset to start the search for blocks over.The robot also drives forward slightly to get it close to the blocks in the center. 
      digitalWrite(Conveyor_Motor,LOW); 
      delay(2000);  
      digitalWrite(Conveyor_Motor,HIGH);  
      Pixy_servo.write(140);
      delay(100);
       digitalWrite(Left_Motor,HIGH);
       digitalWrite(Right_Motor,HIGH);
       delay(300); 
       digitalWrite(Left_Motor,LOW); 
       digitalWrite(Right_Motor,LOW); 
       Serial.println("Drive Straight");     
       Search = 0;
       
//The variables associated with the blocks most recently picked up are cleared to allow the search for a new block to start, including j. 
       j = 0;
       block_signature =0;
       big_signature =0;
     } 
}

//If the width of the block seen by the robot is not greater than 40 then it moves itself right to continue to scan for the bigtarget that matches the signature of the block most 
//recently picked up.
if(width<40){
    digitalWrite(Right_Motor_Dir,LOW);    
   digitalWrite(Left_Motor,255);
   digitalWrite(Right_Motor,255);
    delay(200); 
     digitalWrite(Left_Motor,LOW);
    digitalWrite(Right_Motor,LOW);    
    digitalWrite(Right_Motor_Dir,HIGH); 
    Serial.println("Target Not Found - Scan Right"); 
        }
}
}
}
}



