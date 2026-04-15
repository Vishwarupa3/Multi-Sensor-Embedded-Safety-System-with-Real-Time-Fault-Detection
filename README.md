# Multi-Sensor-Embedded-Safety-System-with-Real-Time-Fault-Detection
1. Overview

Designed a real-time embedded safety system on Arduino Uno that monitors environmental and motion parameters and performs automated responses under hazardous conditions.

2. System Architecture
Sensors → Processing Logic → Output/Action


Input:

DHT11 (Temperature)
MQ-7 (Gas)
HC-SR04 (Distance)
MPU6050 (Tilt)

Processing:

Threshold-based classification (NORMAL / WARNING / CRITICAL)
Multi-sensor decision logic
Low-level register-based control

Output:

LED indicators
LCD display
Relay-controlled DC motor (fan)
3. Hardware Components
Component	Function
Arduino Uno	Main controller
DHT11	Temperature sensing
MQ-7	Gas detection
HC-SR04	Obstacle detection
MPU6050	Tilt/orientation
LCD (I2C)	Status display
LEDs	State indication
Relay + Motor	Automated response
4. Working Principle
Sensors continuously sampled at defined intervals
Data processed using threshold-based logic
System classified into:
NORMAL
WARNING
CRITICAL
Actions:
LED pattern changes
LCD shows critical parameter
Relay activates motor in hazardous conditions
5. Key Features
Real-time multi-sensor data fusion
Low-level register programming (no standard libraries)
Automated safety response system
Modular embedded architecture
Fault classification logic
6. Results / Output
Successfully detects:
High temperature
Toxic gas levels
Tilt instability
Obstacle proximity
Provides:
Visual + physical response
Real-time monitoring on LCD
Automated actuation via relay
7. Challenges & Engineering Decisions
Implemented register-level programming for precise control
Balanced sensor sampling rates for real-time response
Designed threshold logic to avoid false triggering
Managed noise and sensor inaccuracies
8. Future Improvements
Add STM32-based implementation
Integrate CAN communication (automotive relevance)
Add data logging for analysis
Improve gas calibration accuracy
