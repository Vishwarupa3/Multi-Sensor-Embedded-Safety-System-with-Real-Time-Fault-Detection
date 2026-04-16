#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// ---------------- PIN DEFINITIONS ----------------
#define DHT11_PIN 2
#define TRIG_PIN 6
#define ECHO_PIN 7
#define RED_LED 4
#define GREEN_LED 3
#define MQ7_PIN A0       // Gas sensor analog pin
#define RELAY_PIN 8      // >>> NEW CODE <<< Relay control pin


LiquidCrystal_I2C lcd(0x27, 16, 2);
uint8_t DHT_Data[5];


// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();


  // LED setup
  DDRD |= (1 << RED_LED) | (1 << GREEN_LED);
  PORTD &= ~((1 << RED_LED) | (1 << GREEN_LED));


  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);       // >>> NEW CODE <<<
  digitalWrite(RELAY_PIN, HIGH);    // Relay OFF initially (Active LOW)


  initMPU6050();


  lcd.setCursor(0, 0);
  lcd.print("Safety Bot Init");
  delay(2000);
  lcd.clear();


  Serial.println("==================================================");
  Serial.println(" SAFETY BOT SENSOR MONITOR (TEMP + DIST + TILT + GAS)");
  Serial.println("==================================================");
  Serial.println("Temp\tDist\tTilt\tCO(ppm)\tSTATE\tRELAY");
  Serial.println("--------------------------------------------------");
}


// ---------------- MAIN LOOP ----------------
void loop() {
  int temperature = 0;
  bool dhtOK = readDHT11();
  if (dhtOK) temperature = DHT_Data[2];


  float distance = measureDistance();
  float tilt = getTilt();
  float gasPPM = readGasPPM();


  // ---- Determine status ----
  bool isWarning = false, isCritical = false;
  String reason = "Normal";
  bool fanOn = false;   // >>> NEW CODE <<<


  // Temperature
  if (temperature >= 40 && temperature <= 50) {
    isWarning = true; reason = "Temp";
    fanOn = true; // Turn fan ON in warning range
  }
  else if (temperature > 50) {
    isCritical = true; reason = "Temp";
  }


  // Ultrasonic
  if (distance <= 10 && distance > 5) { isWarning = true; reason = "Dist"; }
  else if (distance <= 5) { isCritical = true; reason = "Dist"; }


  // Tilt
  if (tilt >= 15 && tilt <= 20) { isWarning = true; reason = "Tilt"; }
  else if (tilt > 20) { isCritical = true; reason = "Tilt"; }


  // Gas Sensor
  if (gasPPM >= 100 && gasPPM <= 200) {
    isWarning = true; reason = "Gas";
    fanOn = true; // Turn fan ON when gas detected
  }
  else if (gasPPM > 200) {
    isCritical = true; reason = "Gas";
    fanOn = true; // Turn fan ON in critical gas
  }


  // ---- LED + State ----
  String stateMsg = "NORMAL";
  if (isCritical) {
    PORTD |= (1 << RED_LED); PORTD &= ~(1 << GREEN_LED);
    stateMsg = "CRITICAL";
  } else if (isWarning) {
    PORTD &= ~((1 << RED_LED) | (1 << GREEN_LED));
    stateMsg = "WARNING";
  } else {
    PORTD |= (1 << GREEN_LED); PORTD &= ~(1 << RED_LED);
    stateMsg = "NORMAL";
  }


  // ---- Relay Logic (Fan Control) ----
  if (fanOn) {
    digitalWrite(RELAY_PIN, LOW);  // Active LOW → Relay ON
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF
  }


  // ---- SERIAL OUTPUT ----
  Serial.print(temperature);
  Serial.print("\t");
  Serial.print(distance, 1);
  Serial.print("\t");
  Serial.print(tilt, 1);
  Serial.print("\t");
  Serial.print(gasPPM, 0);
  Serial.print("\t");
  Serial.print(stateMsg);
  Serial.print("\t");
  Serial.println(fanOn ? "FAN ON" : "FAN OFF");


  // ---- LCD OUTPUT ----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(stateMsg);
  lcd.setCursor(0, 1);


  if (reason == "Temp") lcd.print("T:" + String(temperature) + "C");
  else if (reason == "Dist") lcd.print("D:" + String(distance, 0) + "cm");
  else if (reason == "Tilt") lcd.print("Tilt:" + String(tilt, 0) + (char)223);
  else if (reason == "Gas") lcd.print("CO:" + String(gasPPM, 0) + "ppm");
  else lcd.print("All OK");


  delay(1500);
}


// ---------------- DHT11 FUNCTION ----------------
bool readDHT11() {
  uint8_t i, j = 0;
  for (i = 0; i < 5; i++) DHT_Data[i] = 0;


  DDRD |= (1 << DHT11_PIN);
  PORTD &= ~(1 << DHT11_PIN);
  delay(20);
  PORTD |= (1 << DHT11_PIN);
  delayMicroseconds(40);
  DDRD &= ~(1 << DHT11_PIN);


  unsigned long timeout = micros();
  while (((PIND & (1 << DHT11_PIN)) != 0)) if ((micros() - timeout) > 100) return false;
  while (((PIND & (1 << DHT11_PIN)) == 0)) if ((micros() - timeout) > 200) return false;
  while (((PIND & (1 << DHT11_PIN)) != 0)) if ((micros() - timeout) > 200) return false;


  for (j = 0; j < 5; j++) {
    for (i = 0; i < 8; i++) {
      timeout = micros();
      while (((PIND & (1 << DHT11_PIN)) == 0)) if ((micros() - timeout) > 100) return false;
      unsigned long tStart = micros();
      while (((PIND & (1 << DHT11_PIN)) != 0)) if ((micros() - tStart) > 200) break;
      unsigned long pulseWidth = micros() - tStart;
      if (pulseWidth > 50) DHT_Data[j] |= (1 << (7 - i));
    }
  }
  if ((uint8_t)(DHT_Data[0] + DHT_Data[1] + DHT_Data[2] + DHT_Data[3]) != DHT_Data[4])
    return false;
  return true;
}


// ---------------- ULTRASONIC FUNCTION ----------------
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return (duration * 0.0343) / 2; // cm
}


// ---------------- MPU6050 FUNCTIONS ----------------
void initMPU6050() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}


float getTilt() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);


  int16_t AcX = Wire.read() << 8 | Wire.read();
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();


  float ax = AcX / 16384.0;
  float ay = AcY / 16384.0;
  float az = AcZ / 16384.0;


  float tilt = atan(sqrt(ax * ax + ay * ay) / az) * (180.0 / 3.14159265359);
  return tilt;
}


// ---------------- MQ7 GAS SENSOR ----------------
float readGasPPM() {
  int analogValue = analogRead(MQ7_PIN);
  float ppm = map(analogValue, 0, 1023, 0, 500); // Rough estimate: 0–500 ppm range
  return ppm;
}


