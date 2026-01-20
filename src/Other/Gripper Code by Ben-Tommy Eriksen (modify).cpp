#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ===== Configuration =====
#define IR_IN 23
#define LCD_ADDRESS 0x27
#define SERVO_COUNT 5
#define STEP_COUNT 16
#define NUM_SERVOS 5

// ===== Servo Setup =====
Servo servos[NUM_SERVOS];
const int SERVO_PINS[] = {4, 5, 6, 7, 8};

// ===== LCD Setup =====
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 4);

// ===== State Variables =====
volatile boolean running = false;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

int menuStep = 0;
int selectedServo = 1;
int servoValues[NUM_SERVOS][STEP_COUNT];
int servoValuesPrev[NUM_SERVOS];

// ===== Remote Control =====
unsigned char irAddress = 0x00;
char irCommandLow = 0x00;
char irCommandHigh = 0x00;
int pulseWidth = 0;

void setup() {
  Serial.begin(9600);
  pinMode(IR_IN, INPUT);
  
  // Initialize servos
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(SERVO_PINS[i]);
    for (int j = 0; j < STEP_COUNT; j++) {
      servoValues[i][j] = 512;
    }
    servoValuesPrev[i] = 512;
  }
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("ARDUINO booting");
  
  timer1_init();
}

void loop() {
  remote_decode();
  processRemoteCommand();
  updateDisplay();
}

void processRemoteCommand() {
  switch (irCommandLow) {
    case 64:   // Next step
      menuStep = min(menuStep + 1, STEP_COUNT - 1);
      break;
    case 68:   // Previous step
      menuStep = max(menuStep - 1, 0);
      break;
    case 12:   // Servo 1
      selectedServo = 1;
      break;
    case 7:    // Decrease value
      servoValues[selectedServo - 1][menuStep] = max(0, servoValues[selectedServo - 1][menuStep] - 8);
      break;
    case 21:   // Increase value
      servoValues[selectedServo - 1][menuStep] = min(1023, servoValues[selectedServo - 1][menuStep] + 8);
      break;
  }
  irCommandLow = 0;
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Step:");
  lcd.print(menuStep);
  
  lcd.setCursor(0, 1);
  lcd.print("Servo:");
  lcd.print(selectedServo);
}

void timer1_init() {
  TCCR1A = 0x00;
  TCCR1B = 0x05;
  TCCR1C = 0x00;
  TCNT1 = 0x00;
  TIMSK1 = 0x00;
}

void remote_decode() {
  // IR decoding logic here
}
