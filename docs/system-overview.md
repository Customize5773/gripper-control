# Gripper Control System - System Overview

## 1. System Architecture

### 1.1 High-Level Design
The gripper control system is a **closed-loop servo control system** that translates analog potentiometer input into precise servo motor positioning. The system implements a **direct mapping control strategy** where the user's manual input (via potentiometer) directly determines the gripper's position in real-time.

### 1.2 Control Flow
```
Potentiometer (Analog Input) → Arduino ADC → Mapping Algorithm → Servo PWM Signal → Gripper Position
```

## 2. Hardware Components

### 2.1 Core Components
- **Microcontroller**: Arduino board (Uno/Nano compatible)
- **Actuator**: Standard hobby servo motor (180° rotation range)
- **Input Device**: 10kΩ potentiometer (linear or rotary)
- **Power Supply**: 5V DC (minimum 2A for servo under load)

### 2.2 Pin Configuration
| Component | Arduino Pin | Function | Notes |
|-----------|-------------|----------|-------|
| Servo Signal | Pin 9 | PWM Output | Must support PWM (~490Hz) |
| Potentiometer | A0 | Analog Input | 0-5V range, 10-bit resolution |
| Servo Power | 5V | Power Supply | External power recommended for load |
| Servo Ground | GND | Common Ground | Must share ground with Arduino |

### 2.3 Electrical Specifications
- **ADC Resolution**: 10-bit (0-1023 discrete values)
- **Servo PWM Frequency**: ~50Hz (20ms period)
- **Servo Pulse Width Range**: 1000-2000μs (standard hobby servo)
- **Current Draw**: 
  - Arduino: ~50mA
  - Servo (no load): ~100mA
  - Servo (stall): Up to 2.5A

## 3. Software Architecture

### 3.1 Main Control Loop
The system operates on a **continuous polling architecture** with fixed timing:

```cpp
// Simplified control flow
while(true) {
    read_potentiometer();    // Analog read from A0
    map_value();            // 0-1023 → 0-180°
    update_servo();         // Write PWM to pin 9
    wait_delay();           // 15ms stability delay
}
```

### 3.2 Key Parameters

| Parameter | Value | Purpose |
|-----------|-------|---------|
| Sampling Rate | ~66.67 Hz | Loop frequency (1000ms/15ms) |
| ADC Reference | 5V | Default Arduino reference voltage |
| Deadband | None | Direct 1:1 mapping |
| Resolution | 0.176° | (180°/1024 steps) |

## 4. Functional Specifications

### 4.1 Input Processing
- **Potentiometer Reading**: Continuous analog voltage (0-5V)
- **ADC Conversion**: 10-bit digital value (0-1023)
- **Noise Handling**: No hardware filtering; relies on servo's mechanical damping

### 4.2 Output Control
- **Servo Communication**: PWM position signaling
- **Pulse Width Calculation**: `500μs + (val × 11.11μs)` where val = 0-180
- **Position Holding**: Servo maintains last commanded position until new command

### 4.3 Timing Characteristics
- **Total Loop Time**: ~15ms (dominated by delay())
- **ADC Conversion Time**: ~0.1ms
- **Servo Response Time**: Varies by model (typically 0.1-0.2s/60°)
- **End-to-End Latency**: <20ms

## 5. System Limitations

### 5.1 Current Implementation Constraints
1. **No Error Handling**: No checks for potentiometer disconnection
2. **Fixed Mapping**: Linear mapping may not match mechanical requirements
3. **No Calibration**: Assumes potentiometer and servo have aligned endpoints
4. **Blocking Architecture**: `delay(15)` prevents concurrent operations

### 5.2 Performance Boundaries
- **Maximum Update Rate**: 66.67 Hz (theoretical)
- **Position Accuracy**: ±1° (due to ADC resolution)
- **Repeatability**: Dependent on potentiometer mechanical wear
- **Load Capacity**: Limited by servo torque (typically 1.5-3kg·cm)

## 6. Mechanical Interface

### 6.1 Gripper Integration
The servo horn must be mechanically coupled to the gripper mechanism. Considerations:
- **Linkage Design**: Should provide mechanical advantage if needed
- **Force Transmission**: Minimize backlash in linkage system
- **Endstop Protection**: Mechanical limits to prevent over-rotation

### 6.2 Mounting Requirements
- Servo: Secure mounting to prevent torque-induced movement
- Potentiometer: Fixed mounting aligned with control input mechanism
- Wiring: Strain relief on all connections

## 7. Safety Considerations

### 7.1 Electrical Safety
- **Overcurrent Protection**: None implemented - requires external fuse (2-3A)
- **Brownout Protection**: None - servo may behave unpredictably at low voltage
- **EMI Considerations**: Unshielded wires may pick up noise in industrial environments

### 7.2 Mechanical Safety
- **Pinch Points**: Gripper mechanism creates pinch hazards
- **Runaway Protection**: No software limits on servo rotation
- **Emergency Stop**: No implemented emergency stop function

## 8. Calibration Procedure

Although not implemented in current code, recommended calibration steps:

1. **Potentiometer Range Verification**: 
   - Rotate to minimum, read ADC value (should be ~0)
   - Rotate to maximum, read ADC value (should be ~1023)

2. **Servo Range Calibration**:
   - Command 0°, mark mechanical position
   - Command 180°, mark mechanical position
   - Adjust `map()` function if mechanical limits differ

## 9. Expansion Interfaces

### 9.1 Available I/O
- **Digital I/O**: 13 pins available (excluding pin 9, A0)
- **Analog Inputs**: 5 additional pins (A1-A5)
- **Serial Communication**: TX/RX pins for external control
- **I2C/SPI**: Available for sensor integration

### 9.2 Suggested Enhancements
1. **Limit Switches**: Add endstop detection on digital pins
2. **Force Sensing**: Add pressure sensor on analog pin
3. **Serial Control**: Replace potentiometer with computer commands
4. **Feedback Potentiometer**: Add position feedback on servo

## 10. Power Management

### 10.1 Current Requirements
| Condition | Current | Notes |
|-----------|---------|-------|
| Idle | 150mA | Arduino + servo (no load) |
| Typical Operation | 300-500mA | Moving with light load |
| Stall Condition | Up to 2.5A | Servo attempting to move beyond limits |

### 10.2 Power Supply Recommendations
- **Minimum**: 5V 1A regulated supply
- **Recommended**: 5V 3A regulated supply with overcurrent protection
- **Wiring**: Separate power lines for Arduino and servo recommended

## 11. Troubleshooting Guide

### 11.1 Common Issues
1. **Servo Jitter**: Add capacitor (100-470μF) across servo power lines
2. **Incomplete Rotation**: Adjust `map()` function range
3. **Erratic Movement**: Check potentiometer connections and grounding
4. **Servo Doesn't Move**: Verify 5V power to servo, check PWM signal with oscilloscope

### 11.2 Debugging Points
- **A0 Voltage**: Should vary smoothly 0-5V with potentiometer rotation
- **Pin 9 Signal**: Verify 50Hz PWM with 1-2ms pulse width
- **Loop Timing**: Confirm consistent 15ms loop time

---

## Revision History
| Version | Date | Changes | Author |
|---------|------|---------|--------|
| 1.0 | Current | Initial system overview | [Your Name] |

## References
1. Arduino Servo Library Documentation
2. Hobby Servo Technical Specifications
3. ADC Conversion Characteristics (ATmega328P Datasheet)