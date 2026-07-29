#include <Arduino.h>
#include <string.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define BUTTON_PIN 2
bool USER_SCREEN_SIZE = true;

void init_gyro();
void getGyroAngles();
void generateMouseCommand();
void changeBtnState();

Adafruit_MPU6050 mpu;
float anglePitch = 0.0;
float angleRoll = 0.0;
float angleYaw = 0.0;
unsigned long lastTimestamp = 0;


volatile bool btnState = 0;
volatile bool ledState = 0;
volatile bool event = false;
int maxScreenX = 1023;
int maxScreenY = 1023;
int xVal = 512;
int yVal = 512;

volatile unsigned long lastClick = 0;
const unsigned long timeDebounce = 50; // 50 ms debounce time

int aux = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), changeBtnState, CHANGE);
  pinMode(LED_BUILTIN, OUTPUT);

  init_gyro();
}

void loop() {

  if (USER_SCREEN_SIZE) {
    while (Serial.available() > 0) {
      String mesajPrimit = Serial.readStringUntil('\n');
      
      if (mesajPrimit.startsWith("W")) {
        int indexVirgula = mesajPrimit.indexOf(',');
        
        if (indexVirgula != -1) {
          String strX = mesajPrimit.substring(1, indexVirgula);
          String strY = mesajPrimit.substring(indexVirgula + 2);
          
          maxScreenX = strX.toInt();
          maxScreenY = strY.toInt();

          xVal = maxScreenX / 2;
          yVal = maxScreenY / 2;

          Serial.write("Marginile au fost primite cu succes!\n");
          
        }
      }
    }

    USER_SCREEN_SIZE = false;
  }


  getGyroAngles();
  generateMouseCommand();

  if (event || btnState) {
    Serial.print(xVal);
    Serial.print(",");
    Serial.print(yVal);
    Serial.print(",");
    Serial.println(btnState);

    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }

  delay(30);
  event = false;

  ledState = 0;
}


void init_gyro() {
  if (!mpu.begin()) {
    Serial.println("Error MPU6050");
    while (1) {
      delay(10);
    }
  }

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
  
  anglePitch = alpha * (anglePitch  + gyroY_deg * dt) + (1.0 - alpha) * accPitch;
  
  angleRoll  = alpha * (angleRoll  + gyroX_deg * dt) + (1.0 - alpha) * accRoll;

}


void generateMouseCommand() {
  const float threshold = 30.0;
  const float sensitivity = 0.5;
  const int screenLimitX = maxScreenX;
  const int screenLimitY = maxScreenY;

  if (angleRoll > threshold) {
    xVal += angleRoll * sensitivity;
    if (xVal > screenLimitX) xVal = screenLimitX;

    event = true;
  } else if (angleRoll < -threshold) {
    xVal += angleRoll * sensitivity;
    if (xVal < 0) xVal = 0;

    event = true;
  }

  if (anglePitch > threshold) {
    yVal += anglePitch * sensitivity;
    if (yVal < 0) yVal = 0;

    event = true;
  } else if (anglePitch < -threshold) {
    yVal += anglePitch * sensitivity;
    if (yVal > screenLimitY) yVal = screenLimitY;

    event = true;
  }

  
}

void changeBtnState() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastClick > timeDebounce) {
    if (btnState) {
      btnState = 0;
    } else {
      btnState = 1;
    }
    event = true;
    lastClick = currentTime;
  }

}