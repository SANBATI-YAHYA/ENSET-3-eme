#include <Servo.h>

// Create servo objects
Servo stillServo;      // This is the servo on pin 7 that will stay still
Servo movingServo;     // This is the servo on pin 6 that will move

void setup() {
  // Attach the servos to their pins
  stillServo.attach(7);
  movingServo.attach(6);
  
  // Set the position for the servo on pin 7 and leave it there
  // 90 degrees is the middle position, you can change it if you need
}

void loop() {
 movingServo.write(90); // likitela3 wihbat zagi f 9oo finma bghitih yhbat wela ytlaa3 
  // Wait for 300 milliseconds
    stillServo.write(180); // dyal catch kiched f 0 rah m7ol f 180 kitsaad 

  delay(600); 
  
  // Move the servo on pin 6 to the 180-degree position (down)
  movingServo.write(0);  // hnaya kirej3o l position dyalhom lh9i9i li m9aad 
  // Wait for 300 milliseconds
  stillServo.write(0); // hnaya kirej3o l position dyalhom lh9i9i li m9aad 

  delay(600);
}