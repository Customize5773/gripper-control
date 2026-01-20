#define DECODE_NEC         
#include <Arduino.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>

// Pin definitions
const int EN = 9;
const int MOTOR_PIN1 = 4;
const int MOTOR_PIN2 = 5;
const int MOTOR_SPEED = 200;
const int MOTOR_STOP = 0;

// IR command codes
const uint16_t CMD_CLOCKWISE = 0x44;
const uint16_t CMD_COUNTERCLOCKWISE = 0x40;
const uint16_t CMD_STOP = 0x43;

void setup() {
    Serial.begin(115200);
    pinMode(MOTOR_PIN1, OUTPUT);
    pinMode(MOTOR_PIN2, OUTPUT);
    pinMode(EN, OUTPUT);

    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals at pin " STR(IR_RECEIVE_PIN)));
}

void motorControl(int pin1State, int pin2State, int speed) {
    digitalWrite(MOTOR_PIN1, pin1State);
    digitalWrite(MOTOR_PIN2, pin2State);
    analogWrite(EN, speed);
}

void loop() {
    if (IrReceiver.decode()) {
        IrReceiver.printIRResultShort(&Serial);
        
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or unknown protocol"));
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        } else {
            switch (IrReceiver.decodedIRData.command) {
                case CMD_CLOCKWISE:
                    motorControl(LOW, HIGH, MOTOR_SPEED);
                    Serial.println(F("Rotating clockwise"));
                    break;
                case CMD_COUNTERCLOCKWISE:
                    motorControl(HIGH, LOW, MOTOR_SPEED);
                    Serial.println(F("Rotating counterclockwise"));
                    break;
                case CMD_STOP:
                    motorControl(HIGH, LOW, MOTOR_STOP);
                    Serial.println(F("Motor stopped"));
                    break;
            }
        }
        IrReceiver.resume();
    }
}
