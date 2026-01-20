/*
   Bluetooth Rover.

   Goal in life...
      Follows your commnds sent magically though the air! Or from USB :)

   Written by Scott Beasley - 2015
   Free to use or modify. Enjoy.
   
   Uses the L9110S library. It works with the L9110S h-bridge.

   Download from https://github.com/jscottb/L9110Driver or clone the zip from
   https://github.com to remove the '-master' from the archive file name to add
   the library
*/
//==============================================================================

#include <L9110Driver.h>
#include <Servo.h>

#define SERVO_Y 2 // Pin gripper Y servo
#define SERVO_CLAW 7 // Pin gripper Gripper servo
#define pinAIN1 5 // define I1 interface
#define pinAIN2 6 // define I2 interface
#define pinBIN1 3 // define I3 interface
#define pinBIN2 11 // define I4 interface

// Speed defines
#define MAXFORWARDSPEED 225 // Max speed we want moving forward
#define MAXBACKWARDSPEED 225 // Max reverse speed
#define TOPSPEED 255 // Used to help turn better on carpet and rougher surfaces.

// Various time delays used for driving and servo
#define TURNDELAY 475
#define TURNDELAY45 235
#define BACKUPDELAY 400
#define SERVOMOVEDELAY 200
#define SERVOSEARCHDELAY 85

/*
   Globals area.
*/

// Create the motor, servo objects to interface with
L9110_Motor motor_left (pinAIN1, pinAIN2); // Create Left motor object
L9110_Motor motor_right (pinBIN1, pinBIN2); // Create Right motor object
Servo grip_y_servo; // Create a servo object for the gripper Y axis
Servo grip_servo; // Create a servo object for the gripper claw

void setup()
{
   // Change the baud rate here if diffrent then 57600
   Serial.begin (57600);

   grip_y_servo.attach (SERVO_Y); // Attach the servo SERVO_LR
   grip_y_servo.write (90);
   grip_servo.attach (SERVO_CLAW); // Attach the servo SERVO_LR
   grip_servo.write (90);

   delay (500);
}

void loop ( )
{
   byte command = 0, val = 0;

   if (Serial.available ( ) > 0) {
      // read the incoming command byte
      command = Serial.read ( );
   }

   switch (command)
   {
      case 'w':
         go_forward ( );
         //Serial.println ("Going Forward");
         break;

      case 'z':
         go_backward ( );
         //Serial.println ("Going Backwards");
         break;

      case 'a':
         go_left ( );
			delay (TURNDELAY);
	      halt ( );
         //Serial.println ("Turning Left");
         break;

      case 's':
         go_right ( );
			delay (TURNDELAY);
	      halt ( );
         //Serial.println ("Turning Right");
         break;

		case 'q':
         go_left ( );
			delay (TURNDELAY45);
	      halt ( );
         //Serial.println ("Turning Left");
         break;

      case 'e':
         go_right ( );
			delay (TURNDELAY45);
	      halt ( );
         //Serial.println ("Turning Right");
         break;

      case 'h':
         halt ( );
         //Serial.println ("Halting");
         break;

      case '>':
         // Gripper X move sends servo set value
         val = Serial.read ( );

         // We limit the value to real movemovent limits of the setup
         grip_servo.write (constrain (val, 64, 179));
         //Serial.println ("GripperX");
         break;

      case '^':
         // Gripper Y move sends servo set value
         val = Serial.read ( );

         // We limit the value to real movemovent limits of the setup
         grip_y_servo.write (constrain (val, 53, 179));
         //Serial.println ("GripperY");
         break;

      case 'c':
         // We limit the value to real movemovent limits of the setup
         grip_y_servo.write (90);
         grip_servo.write (90);
         //Serial.println ("GripperHome");
         break;

      case 255: // Sent after all gripper commands
         Serial.flush ( );
         break;
   }

   Serial.flush ( );
   delay(125);
}

void go_forward ( )
{
   //Serial.println ("Going forward...");

   // Ramp the motors up to the speed.
   // Help with spining out on some surfaces and ware and tare on the GM's
   ramp_it (MAXFORWARDSPEED, FORWARD, FORWARD);

   // Set to all of the set speed just incase the ramp last vat was not all of
   // it.
   motor_left.setSpeed (MAXFORWARDSPEED);
   motor_right.setSpeed (MAXFORWARDSPEED);
   motor_left.run (FORWARD|RELEASE);
   motor_right.run (FORWARD|RELEASE);
}

void go_backward ( )
{
   //Serial.println ("Going backward...");

   // Ramp the motors up to the speed.
   // Help with spining out on some surfaces and ware and tare on the GM's
   ramp_it (MAXBACKWARDSPEED, BACKWARD, BACKWARD);

   // Set to all of the set speed just incase the ramp last vat was not all of
   // it.
   motor_left.setSpeed (MAXBACKWARDSPEED);
   motor_right.setSpeed (MAXBACKWARDSPEED);
   motor_left.run (BACKWARD|RELEASE);
   motor_right.run (BACKWARD|RELEASE);
}

void go_left ( )
{
   //Serial.println ("Going left...");

   // Ramp the motors up to the speed.
   // Help with spining out on some surfaces and ware and tare on the GM's
   ramp_it (TOPSPEED, BACKWARD, FORWARD);

   // Set to all of the set speed just incase the ramp last vat was not all of
   // it.
   motor_left.setSpeed (TOPSPEED);
   motor_right.setSpeed (TOPSPEED);
   motor_left.run (BACKWARD|RELEASE);
   motor_right.run (FORWARD|RELEASE);
}

void go_right ( )
{
   //Serial.println ("Going right...");

   // Ramp the motors up to the speed.
   // Help with spining out on some surfaces and ware and tare on the GM's
   ramp_it (TOPSPEED, FORWARD, BACKWARD);

   // Set to all of the set speed just incase the ramp last vat was not all of
   // it.
   motor_left.setSpeed (TOPSPEED);
   motor_right.setSpeed (TOPSPEED);
   motor_left.run (FORWARD|RELEASE);
   motor_right.run (BACKWARD|RELEASE);
}

void halt ( )
{
   //Serial.println ("Halt!");
   //ramp_it (0, BRAKE, BRAKE);
   motor_left.setSpeed (0);
   motor_right.setSpeed (0);
   motor_left.run (BRAKE);
   motor_right.run (BRAKE);
}

void ramp_it (uint8_t speed, uint8_t lf_dir, uint8_t rt_dir)
{
  uint8_t ramp_val = 0, step_val = 0;

  step_val = abs (speed / 4);
  if (!speed)
     step_val = -step_val;

  for (uint8_t i = 0; i < 4; i++) {
     ramp_val += step_val;
     motor_left.setSpeed (ramp_val);
     motor_right.setSpeed (ramp_val);
     motor_left.run (lf_dir|RELEASE);
     motor_right.run (rt_dir|RELEASE);
     delay (25);
  }
}
