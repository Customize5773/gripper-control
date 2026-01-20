// SimpleGripper - Open-source 2DOF Parallel Gripper
//By Alexander Pedersen in Circuits

#include <Servo.h>

// Servo Control Pins
const int servoPin1 = 9;  // Servo 1 control pin
const int servoPin2 = 10; // Servo 2 control pin

// Servo Objects
Servo myservo1;
Servo myservo2;
int servo1Pos = 0; // Current position of Servo 1
int servo2Pos = 0; // Current position of Servo 2

// Remote Control Channel
int ch5 = 0; // Channel for controlling the claw
int ch7 = 0; // Channel for controlling the wrist

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate

  // Attach servos to their respective pins
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);

  // Initialize servos to starting positions
  myservo1.write(servo1Pos); // Start at neutral position
  myservo2.write(servo2Pos); // Start at neutral position
}

void loop() {
  // Servo control using ch7 (Wrist)
  if (ch7 == 180) {
    servo1Pos += 10;
    if (servo1Pos > 180) servo1Pos = 180;
    myservo1.write(servo1Pos);
    Serial.print("Wrist Position: ");
    Serial.println(servo1Pos);
  } else if (ch7 == 0 ) {
    servo1Pos -= 10;
    if (servo1Pos < 0) servo1Pos = 0;
    myservo1.write(servo1Pos);
    Serial.print("Wrist Position: ");
    Serial.println(servo1Pos);
  }

    // Servo control using ch5 (Claw). servo2Pos = 63 is the limit of how much the claw can close so that it doesn't break.
    if (ch5 == 180) {
        servo2Pos += 4;
        if (servo2Pos > 63) servo2Pos = 63;
        myservo2.write(servo2Pos);
        Serial.print("Claw Position: ");
        Serial.println(servo2Pos);
    } else if (ch5 == 0 ) {
        servo2Pos -= 4;
        if (servo2Pos < 0) servo2Pos = 0;
        myservo2.write(servo2Pos);
        Serial.print("Claw Position: ");
        Serial.println(servo2Pos);
    }
}