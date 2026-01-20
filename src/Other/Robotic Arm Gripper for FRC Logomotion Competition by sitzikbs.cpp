#include <Arduino.h>
#include <PS3Controller.h>
#include <ESP32Servo.h>

// Robotic Arm Gripper for FRC Logomotion Competition (by sitzikbs) - ESP32 Control System
// Platform: ESP32 | Controller: PS3 Wireless Controller | Motor: Denso Window Motor

// PIN DEFINITIONS
#define GRIPPER_MOTOR_PIN_IN1    25
#define GRIPPER_MOTOR_PIN_IN2    26
#define GRIPPER_MOTOR_PWM_PIN    27
#define MOTOR_ENABLE_PIN         14
#define SERVO_PIN_1              32
#define SERVO_PIN_2              33

// PWM CONFIGURATION
#define PWM_FREQUENCY            1000
#define PWM_RESOLUTION           8
#define PWM_CHANNEL_MOTOR        0
#define PWM_CHANNEL_SERVO_1      1
#define PWM_CHANNEL_SERVO_2      2
#define MAX_MOTOR_SPEED          255
#define MOTOR_ACCELERATION_STEP  5

// CONTROLLER SENSITIVITY
#define STICK_DEAD_ZONE          30
#define TRIGGER_THRESHOLD        50
#define CONTROLLER_TIMEOUT       1000

// SERVO LIMITS
const int MIN_SERVO_ANGLE = 0;
const int MAX_SERVO_ANGLE = 180;

// GLOBAL VARIABLES
ESP32Servo servo1, servo2;

int currentMotorSpeed = 0;
int targetMotorSpeed = 0;
boolean motorDirection = true;  // true = open, false = close

int servo1Position = 90;
int servo2Position = 90;

boolean PS3_Connected = false;
unsigned long lastControllerUpdate = 0;

enum SystemState {
  IDLE,
  OPERATING,
  ERROR
};
SystemState systemState = IDLE;

// FUNCTION DECLARATIONS
void setupPins();
void setupMotor();
void setupServos();
void setupPS3Controller();
void updateMotorSpeed();
void updateServoPositions();
void handlePS3Input();
void setGripperMotor(int speed, boolean direction);
void setServoPosition(int servoId, int angle);
void emergencyStop();
void printSystemStatus();

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\nFRC Gripper Control System - ESP32\n");
  
  setupPins();
  setupMotor();
  setupServos();
  setupPS3Controller();
  
  systemState = IDLE;
  Serial.println("System initialization complete!");
  Serial.println("Waiting for PS3 controller connection...\n");
}

void loop() {
  if (PS3.isConnected()) {
    if (!PS3_Connected) {
      PS3_Connected = true;
      Serial.println("PS3 Controller Connected!");
      systemState = OPERATING;
    }
    
    lastControllerUpdate = millis();
    handlePS3Input();
    updateMotorSpeed();
    updateServoPositions();
    
  } else {
    if (PS3_Connected) {
      PS3_Connected = false;
      Serial.println("PS3 Controller Disconnected!");
      emergencyStop();
      systemState = IDLE;
    }
  }
  
  if (PS3_Connected && (millis() - lastControllerUpdate > CONTROLLER_TIMEOUT)) {
    Serial.println("Controller timeout - Emergency stop activated");
    emergencyStop();
    systemState = ERROR;
  }
  
  delay(50);
}

void setupPins() {
  pinMode(GRIPPER_MOTOR_PIN_IN1, OUTPUT);
  pinMode(GRIPPER_MOTOR_PIN_IN2, OUTPUT);
  pinMode(GRIPPER_MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  
  digitalWrite(GRIPPER_MOTOR_PIN_IN1, LOW);
  digitalWrite(GRIPPER_MOTOR_PIN_IN2, LOW);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
  
  Serial.println("[SETUP] Pins configured");
}

void setupMotor() {
  ledcSetup(PWM_CHANNEL_MOTOR, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(GRIPPER_MOTOR_PWM_PIN, PWM_CHANNEL_MOTOR);
  ledcWrite(PWM_CHANNEL_MOTOR, 0);
  
  Serial.println("[SETUP] Motor PWM configured");
}

void setupServos() {
  servo1.setPeriodHertz(50);
  servo1.attach(SERVO_PIN_1, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
  servo1.write(90);
  
  servo2.setPeriodHertz(50);
  servo2.attach(SERVO_PIN_2, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
  servo2.write(90);
  
  servo1Position = 90;
  servo2Position = 90;
  
  Serial.println("[SETUP] Servos configured and initialized to 90°");
}

void setupPS3Controller() {
  PS3.begin("00:1A:7D:DA:71:13");  // Update MAC address to match your controller
  
  Serial.println("[SETUP] PS3 Controller initialized");
  Serial.println("Press PS button on your controller to connect...\n");
}

void handlePS3Input() {
  // L1: Open gripper
  if (PS3.getButtonClick(L1)) {
    Serial.println("Opening gripper");
    targetMotorSpeed = MAX_MOTOR_SPEED;
    motorDirection = true;
  }
  
  // L2: Close gripper
  if (PS3.getButtonClick(L2)) {
    Serial.println("Closing gripper");
    targetMotorSpeed = MAX_MOTOR_SPEED;
    motorDirection = false;
  }
  
  // Circle: Stop motor
  if (PS3.getButtonClick(CIRCLE)) {
    Serial.println("Motor stopped");
    targetMotorSpeed = 0;
  }
  
  // Right stick Y-axis: Variable motor speed
  int rightStickY = PS3.getAnalogButton(R_Y);
  if (abs(rightStickY - 128) > STICK_DEAD_ZONE) {
    int stickInput = map(rightStickY, 0, 255, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    targetMotorSpeed = abs(stickInput);
    motorDirection = (stickInput > 0) ? true : false;
  }
  
  // Left stick X-axis: Servo 1 control
  int leftStickX = PS3.getAnalogButton(L_X);
  if (abs(leftStickX - 128) > STICK_DEAD_ZONE) {
    servo1Position = map(leftStickX, 0, 255, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
  }
  
  // Left stick Y-axis: Servo 2 control
  int leftStickY = PS3.getAnalogButton(L_Y);
  if (abs(leftStickY - 128) > STICK_DEAD_ZONE) {
    servo2Position = map(leftStickY, 0, 255, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
  }
  
  // Triangle: Emergency stop
  if (PS3.getButtonClick(TRIANGLE)) {
    Serial.println("EMERGENCY STOP ACTIVATED");
    emergencyStop();
  }
  
  // Square: Print system status
  if (PS3.getButtonClick(SQUARE)) {
    printSystemStatus();
  }
  
  // Select: Reset to default positions
  if (PS3.getButtonClick(SELECT)) {
    Serial.println("Resetting to default positions");
    servo1Position = 90;
    servo2Position = 90;
    targetMotorSpeed = 0;
  }
}

void updateMotorSpeed() {
  if (currentMotorSpeed < targetMotorSpeed) {
    currentMotorSpeed = min(currentMotorSpeed + MOTOR_ACCELERATION_STEP, targetMotorSpeed);
  } else if (currentMotorSpeed > targetMotorSpeed) {
    currentMotorSpeed = max(currentMotorSpeed - MOTOR_ACCELERATION_STEP, targetMotorSpeed);
  }
  
  setGripperMotor(currentMotorSpeed, motorDirection);
}

void updateServoPositions() {
  setServoPosition(1, servo1Position);
  setServoPosition(2, servo2Position);
}

void setGripperMotor(int speed, boolean direction) {
  speed = constrain(speed, 0, MAX_MOTOR_SPEED);
  
  if (direction) {  // Forward (Open)
    digitalWrite(GRIPPER_MOTOR_PIN_IN1, HIGH);
    digitalWrite(GRIPPER_MOTOR_PIN_IN2, LOW);
  } else {  // Reverse (Close)
    digitalWrite(GRIPPER_MOTOR_PIN_IN1, LOW);
    digitalWrite(GRIPPER_MOTOR_PIN_IN2, HIGH);
  }
  
  ledcWrite(PWM_CHANNEL_MOTOR, speed);
  
  if (speed > 0) {
    digitalWrite(MOTOR_ENABLE_PIN, HIGH);
  } else {
    digitalWrite(MOTOR_ENABLE_PIN, LOW);
  }
}

void setServoPosition(int servoId, int angle) {
  angle = constrain(angle, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);
  
  if (servoId == 1) {
    servo1.write(angle);
  } else if (servoId == 2) {
    servo2.write(angle);
  }
}

void emergencyStop() {
  targetMotorSpeed = 0;
  currentMotorSpeed = 0;
  setGripperMotor(0, true);
  
  servo1.write(servo1Position);
  servo2.write(servo2Position);
  
  Serial.println("All motors stopped - System in safe state");
}

void printSystemStatus() {
  Serial.println("\n--- FRC GRIPPER SYSTEM STATUS ---");
  Serial.print("PS3 Connected: ");
  Serial.println(PS3_Connected ? "YES" : "NO");
  Serial.print("System State: ");
  switch (systemState) {
    case IDLE:
      Serial.println("IDLE");
      break;
    case OPERATING:
      Serial.println("OPERATING");
      break;
    case ERROR:
      Serial.println("ERROR");
      break;
  }
  Serial.print("Motor Speed: ");
  Serial.print(currentMotorSpeed);
  Serial.println(" / 255");
  Serial.print("Motor Direction: ");
  Serial.println(motorDirection ? "OPEN" : "CLOSE");
  Serial.print("Servo 1 Position: ");
  Serial.print(servo1Position);
  Serial.println("°");
  Serial.print("Servo 2 Position: ");
  Serial.print(servo2Position);
  Serial.println("°");
  Serial.println("-----------------------------------\n");
}
