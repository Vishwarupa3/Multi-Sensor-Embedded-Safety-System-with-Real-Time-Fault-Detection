# Multi-Sensor-Embedded-Safety-System-with-Real-Time-Fault-Detection

## Overview
Designed and implemented a real-time embedded safety system using Arduino Uno that monitors environmental and motion parameters and triggers automated responses under hazardous conditions.

The system integrates multiple sensors to detect temperature, gas leakage, tilt, and obstacle proximity, and classifies system state into NORMAL, WARNING, and CRITICAL with corresponding actions.

---

## System Architecture

Input → Processing → Output

Sensors:
- DHT11 (Temperature & Humidity)
- MQ-7 (Gas Sensor)
- MPU6050 (Tilt Detection)
- HC-SR04 (Ultrasonic Distance)

Processing:
- Threshold-based decision logic
- Multi-sensor data fusion

Output:
- LCD (Status Display)
- LEDs (State Indication)
- Relay + DC Motor (Actuation)

---

## Hardware Components

- Arduino Uno (ATmega328P)
- DHT11 Sensor
- MQ-7 Gas Sensor
- MPU6050 IMU
- HC-SR04 Ultrasonic Sensor
- 16x2 LCD (I2C)
- LED Indicators (Red, Green)
- Relay Module + DC Motor (Fan)

---

## Working Principle

The system continuously monitors sensor data and applies threshold-based classification:

- NORMAL → Safe conditions
- WARNING → Threshold nearing unsafe levels
- CRITICAL → Hazard detected, immediate action triggered

### Key Logic:

- Temperature > 50°C → CRITICAL
- Gas > 200 ppm → CRITICAL (fan activated)
- Distance < 5 cm → CRITICAL
- Tilt > 20° → CRITICAL

When a critical condition is detected:
- Red LED is activated
- LCD displays fault parameter
- Relay triggers DC motor (fan)

---

## Software Implementation

- Low-level register manipulation for DHT11 communication
- I2C communication for MPU6050 and LCD
- Modular sensor reading functions:
  - readDHT11()
  - measureDistance()
  - getTilt()
  - readGasPPM()

- Real-time decision logic implemented in main loop

---

## Output & Results

### System States:

| State     | Condition Trigger | Output |
|----------|------------------|--------|
| NORMAL   | All sensors safe | Green LED ON |
| WARNING  | Any threshold nearing | LEDs OFF |
| CRITICAL | Any sensor critical | Red LED + Fan ON |

### Example Output:
- LCD displays parameter causing alert (Temp / Gas / Tilt / Distance)
- Serial Monitor logs real-time sensor data

---

## Key Features

- Real-time multi-sensor monitoring
- Threshold-based fault detection
- Automated safety response (fan activation)
- Low-level embedded programming (register-based DHT11)
- Modular and scalable design

---

## Power & Efficiency Considerations

- Sensors polled at optimized intervals
- Actuator (fan) triggered only in warning/critical states
- Efficient use of GPIO and ADC resources

---

## Future Improvements

- Replace threshold logic with adaptive/AI-based decision system
- Add wireless communication (BLE/Wi-Fi)
- Improve gas sensor calibration accuracy
- Integrate battery-powered low-power modes

---

## Demo

([Demo_Video](https://drive.google.com/file/d/1GoQM784-Yb3l3ZghxaSWp-q1uK8yKl0B/view))

---

