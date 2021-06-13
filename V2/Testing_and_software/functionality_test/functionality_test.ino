
#include <Wire.h>
#include <SPI.h>
#include <Client.h>
#include "SparkFunLSM6DS3.h"
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <CayenneArduinoDefines.h>
#include <CayenneArduinoMQTTClient.h>
#include <CayenneHandlers.h>
#include <CayenneMessage.h>
#include <CayenneMQTTESP8266.h>
#include <CayenneMQTTWiFiClient.h>
#include <DetectDevice.h>

// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling. 

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial

LSM6DS3 myIMU(I2C_MODE, 0x6a); //Default constructor is I2C, addr 0x6B
SFEVL53L1X distanceSensor;

unsigned long lastMillis = 0;

// WiFi network info.
char ssid[] = "BearsAndBears"; //Place your SSID within the quotations
char wifiPassword[] = "dudeson666"; //Place your wifi password within the quotations 

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "59d72100-1298-11e8-9f38-9fae3d42ebf0"; //Place username from your account within the quotations
char password[] = "9314ad7b109d3cbb67591777d2f61dde2b549663"; //Place password from your account within the quotations. 
char clientID[] = "74b02990-ea40-11ea-a67f-15e30d90bbf4"; //Place Client ID from your account within the quotations. 




void setup() {
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  Serial.println("RoboSkin Calibrate! ");
   myIMU.begin();
   if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor online!");
  }
}

void loop() {
  Cayenne.loop();
    //View data on serial monitor and then publish data to Cayenne's platform. 
    
  //Get all parameters
  Serial.println("Accelerometer:");
  Serial.print(" X = ");
  float accelx = myIMU.readFloatAccelX();
  Serial.println(accelx, 4);
  Serial.print(" Y = ");
  float accely = myIMU.readFloatAccelY();
  Serial.println(accely, 4);
  Serial.print(" Z = ");
  float accelz = myIMU.readFloatAccelZ();
  Serial.println(accelz, 4);

  Serial.println("Gyroscope:");
  Serial.print(" X = ");
  float gyrox = myIMU.readFloatGyroX();
  Serial.println(gyrox, 4);
  Serial.print(" Y = ");
  float gyroy = myIMU.readFloatGyroY();
  Serial.println(gyroy, 4);
  Serial.print(" Z = ");
  float gyroz = myIMU.readFloatGyroZ();
  Serial.println(gyroz, 4);

  Serial.println("Thermometer:");
  Serial.print(" Degrees C = ");
  float tempc = myIMU.readTempC();
  Serial.println(tempc, 4);
  Serial.print(" Degrees F = ");
  float tempf = myIMU.readTempF();  
  Serial.println(tempf, 4);
  
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady()) {
    delay(1);
  }
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  Serial.println("Distance:");
  Serial.print(" Distance(mm): ");
  Serial.println(distance);

  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;

  Serial.print(" Distance(ft): ");
  Serial.println(distanceFeet, 2);
  Serial.println();
  
  delay(250);

  //Publish data every  60  seconds. Change this value to publish at a different interval.
  if (millis() - lastMillis > 60000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);
    Cayenne.virtualWrite(1, accelx);
    Cayenne.virtualWrite(2, accely);
    Cayenne.virtualWrite(3, accelz);
    Cayenne.virtualWrite(4, gyrox);
    Cayenne.virtualWrite(5, gyroy);
    Cayenne.virtualWrite(6, gyroz);
    Cayenne.virtualWrite(7, tempc);
    Cayenne.virtualWrite(9, tempf);
    Cayenne.virtualWrite(10, distance);
    Cayenne.virtualWrite(11, distanceFeet);
  }
}


//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
