# Calibration Guide

## Prerequisites

### Required Tools
- Multimeter
- Small screwdriver
- Serial monitor (Arduino IDE)

### Software Setup
1. Open Arduino IDE
2. Select correct board (Arduino Uno/Nano)
3. Select correct COM port

## 1. Potentiometer Range Calibration

### 1.1 Hardware Check
```cpp
// Upload this test sketch
void setup() {
  Serial.begin(9600);
}

void loop() {
  int potValue = analogRead(A0);
  Serial.print("Pot Value: ");
  Serial.println(potValue);
  delay(500);
}
```

### 1.2 Expected Readings
| Pot Position | Expected Range | Action if Outside Range |
|-------------|---------------|------------------------|
| Fully CCW | 0-10 | Check wiring, replace pot |
| Middle | 500-523 | Acceptable variance |
| Fully CW | 1013-1023 | Check wiring, replace pot |

## 2. Servo Endpoint Calibration

### 2.1 Mechanical Setup
1. Disconnect servo from gripper mechanism
2. Ensure free rotation (no load)
3. Mark 0° and 180° positions on servo horn

### 2.2 Software Calibration Test
```cpp
#include <Servo.h>
Servo myservo;

void setup() {
  myservo.attach(9);
  Serial.begin(9600);
}

void loop() {
  // Test 0° position
  myservo.write(0);
  Serial.println("Position: 0° - Should be MIN");
  delay(3000);
  
  // Test 180° position
  myservo.write(180);
  Serial.println("Position: 180° - Should be MAX");
  delay(3000);
}
```

### 2.3 Calibration Values
| Command | Expected Result | Calibration Factor |
|---------|----------------|-------------------|
| 0° | Fully closed position | Adjust mapping minimum |
| 180° | Fully open position | Adjust mapping maximum |

## 3. Mapping Function Calibration

### 3.1 Current Default Mapping
```cpp
val = map(val, 0, 1023, 0, 180);  // Default 1:1 mapping
```

### 3.2 Custom Calibration Values
Find actual potentiometer range:
```cpp
void setup() {
  Serial.begin(9600);
}

void loop() {
  int potMin = 1023;
  int potMax = 0;
  int reading = analogRead(A0);
  
  // Rotate pot fully CCW
  potMin = min(potMin, reading);
  
  // Rotate pot fully CW  
  potMax = max(potMax, reading);
  
  Serial.print("Min: ");
  Serial.print(potMin);
  Serial.print(" Max: ");
  Serial.println(potMax);
  delay(100);
}
```

### 3.3 Update Mapping Function
```cpp
// Replace with calibrated values
int potMinCalibrated = 15;    // Your measured minimum
int potMaxCalibrated = 1010;  // Your measured maximum
int servoMinCalibrated = 10;  // Servo minimum safe position
int servoMaxCalibrated = 170; // Servo maximum safe position

val = map(val, potMinCalibrated, potMaxCalibrated, 
               servoMinCalibrated, servoMaxCalibrated);
```

## 4. Safety Limit Calibration

### 4.1 Mechanical Limits
Define safe operating range:
```cpp
// Constrain servo movement for safety
val = map(val, potMinCalibrated, potMaxCalibrated, 
               servoMinCalibrated, servoMaxCalibrated);
val = constrain(val, servoMinCalibrated, servoMaxCalibrated);
```

### 4.2 Deadband Calibration
If potentiometer has jitter:
```cpp
int deadband = 5;  // Adjust based on observed jitter
int previousVal = 0;

// Only update servo if change exceeds deadband
if (abs(val - previousVal) > deadband) {
    myservo.write(val);
    previousVal = val;
}
```

## 5. Final Calibration Procedure

### Step-by-Step Process
1. **Initial Setup** (5 minutes)
   - Upload calibration sketch
   - Open serial monitor (9600 baud)
   - Record baseline readings

2. **Potentiometer Test** (3 minutes)
   - Rotate fully CCW, record value
   - Rotate fully CW, record value
   - Check smooth transition

3. **Servo Range Test** (5 minutes)
   - Command 0°, mark position
   - Command 180°, mark position
   - Adjust mapping if needed

4. **Integration Test** (7 minutes)
   - Connect potentiometer to servo
   - Verify full range movement
   - Check for smooth operation

5. **Safety Verification** (3 minutes)
   - Verify constrained movement
   - Test for jitter/stability
   - Confirm no overheating

## 6. Calibration Parameters Table

| Parameter | Default Value | Calibrated Value | Notes |
|-----------|--------------|------------------|-------|
| Pot Min | 0 | ________ | CCW position |
| Pot Max | 1023 | ________ | CW position |
| Servo Min | 0 | ________ | Minimum safe angle |
| Servo Max | 180 | ________ | Maximum safe angle |
| Deadband | 0 | ________ | Reduce jitter |
| Delay | 15ms | ________ | Adjust for stability |

## 7. Troubleshooting Calibration

### Common Issues & Solutions

| Issue | Symptoms | Solution |
|-------|----------|----------|
| Incomplete Range | Servo doesn't reach full rotation | Adjust mapping boundaries |
| Jitter | Servo shakes at certain positions | Increase delay or add deadband |
| Non-linear | Movement not proportional to pot | Check pot linearity, replace if needed |
| Jumping | Servo skips positions | Check wiring, add capacitor |

### Quick Test Commands
```cpp
// Test specific angles
void testAngles() {
    int testAngles[] = {0, 45, 90, 135, 180};
    for (int i = 0; i < 5; i++) {
        myservo.write(testAngles[i]);
        delay(2000);
    }
}
```

## 8. Calibration Verification Checklist

- [ ] Potentiometer reads 0-1023 throughout range
- [ ] Servo moves smoothly 0-180°
- [ ] No mechanical binding at extremes
- [ ] No electrical noise or jitter
- [ ] Proper gripper alignment
- [ ] Safety limits enforced
- [ ] Repeatable positioning
- [ ] Documentation updated

## 9. Maintenance Calibration

### When to Recalibrate
- Every 6 months (regular maintenance)
- After mechanical impact
- When changing gripper attachments
- If noticing positioning errors
- After component replacement

### Quick Recalibration
1. Run endpoint test (Section 2.2)
2. Verify pot range (Section 1.2)
3. Update mapping if needed (Section 3.3)

---