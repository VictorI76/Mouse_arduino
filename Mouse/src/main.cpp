#include <Arduino.h>
#include <string.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define BUTTON_PIN 2

void init_gyro();
void getGyroAngles();
void generateMouseCommand();
void changeBtnState();

Adafruit_MPU6050 mpu;
float anglePitch = 0.0;
float angleRoll = 0.0;
unsigned long lastTimestamp = 0;

int xVal = 0;
int yVal = 0;
volatile bool btnState = 1;
volatile bool ledState = 0;
volatile bool event = false;

volatile unsigned long lastClick = 0;
const unsigned long timeDebounce = 50; // 50 ms debounce time

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), changeBtnState, FALLING);
  pinMode(LED_BUILTIN, OUTPUT);

  init_gyro();
}

void loop() {
  getGyroAngles();
  generateMouseCommand();

  if (event) {
    Serial.print(xVal);
    Serial.print(",");
    Serial.print(yVal);
    Serial.print(",");
    Serial.println(btnState);

    // if (btnState == 0) {
    //   btnState = 1;
    // }
  }

  delay(30);
  event = false;
}


void init_gyro() {
  Serial.println("Inițializare test MPU6050...");

  if (!mpu.begin()) {
    Serial.println("Eroare MPU6050");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 a fost găsit cu succes!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("");
  delay(100);
}


void getGyroAngles() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  if (lastTimestamp == 0) 
  { 
    lastTimestamp = currentTime;
  } 
  
  float dt = (currentTime - lastTimestamp) / 1000.0;
  lastTimestamp = currentTime;

  float accPitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  float accRoll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;

  float gyroX_deg = g.gyro.x * 180.0 / PI;
  float gyroY_deg = g.gyro.y * 180.0 / PI;

  float alpha = 0.96;
  
  anglePitch = alpha * (anglePitch + gyroY_deg * dt) + (1.0 - alpha) * accPitch;
  
  angleRoll  = alpha * (angleRoll  + gyroX_deg * dt) + (1.0 - alpha) * accRoll;
  // Serial.print("Pitch (Fata/Spate): "); 
  // Serial.print(anglePitch);
  // Serial.print(" grade | Roll (Stanga/Dreapta): "); 
  // Serial.print(angleRoll);
  // Serial.println(" grade");
}


void generateMouseCommand() {
  const float threshold = 5.0;
  const float sensitivity = 2.0;
  const int screenLimitX = 1023;
  const int screenLimitY = 1023;

  if (angleRoll > threshold) {
    xVal += (angleRoll - threshold) * sensitivity;
    if (xVal > screenLimitX) xVal = screenLimitX;

    event = true;
  } else if (angleRoll < -threshold) {
    xVal += (angleRoll + threshold) * sensitivity;
    if (xVal < 0) xVal = 0;

    event = true;
  }

  if (anglePitch > threshold) {
    yVal -= (anglePitch - threshold) * sensitivity;
    if (yVal < 0) yVal = 0;

    event = true;
  } else if (anglePitch < -threshold) {
    yVal -= (anglePitch + threshold) * sensitivity;
    if (yVal > screenLimitY) yVal = screenLimitY;

    event = true;
  }

  
}

void changeBtnState() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastClick > timeDebounce) {
    btnState ^= 1;
    event = true;
    lastClick = currentTime;
  }

  ledState ^= 1;
  digitalWrite(LED_BUILTIN, ledState);
}