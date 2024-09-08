#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>

// Adafruit GFX Library - Version: Latest
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

// Adafruit SSD1306 - Version: Latest
#include <Adafruit_SSD1306.h>
#include <splash.h>

// DHT sensor library - Version: Latest
#include <DHT.h>
#include <DHT_U.h>

// Define DHT22 Parameters
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define variables for Temperature and Humidity
float temp;
float hum;

#define RELAY_OUT 21

#define SOIL_SERVO 34
#define SENSOR_SERVO 43

// Constant for dry sensor
const int DryValue = 1023;

// Constant for wet sensor
const int WetValue = 680;

// Variables for soil moisture
int soilMoistureValue;
int soilMoisturePercent;

// Pump Status Text
String pump_status_text = "OFF";

 bool pump_Status;

// Analog input port
#define SENSOR_IN A0

#define SERVO_PIN 43
#define ULTRASONIC_SENSOR_TRIG 47
#define ULTRASONIC_SENSOR_ECHO 48
#define MAX_REGULAR_MOTOR_SPEED 75
#define MAX_MOTOR_ADJUST_SPEED 150
#define DISTANCE_TO_CHECK 30

//Right motor
int enableRightMotor=24;
int rightMotorPin1=22;
int rightMotorPin2=23;

//Left motor
int enableLeftMotor=25;
int leftMotorPin1=26;
int leftMotorPin2=27;

// Set OLED size in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Set OLED parameters
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

NewPing mySensor(ULTRASONIC_SENSOR_TRIG, ULTRASONIC_SENSOR_ECHO, 400);
Servo myServo;
Servo sensServo;

// Variable for pump trigger
int pump_trigger = 30;

void setup() {
  // put your setup code here, to run once:

  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
  
// Initialize I2C display using 3.3 volts
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  // Initialize DHT22
  dht.begin();

  // Set Relay as Output
  pinMode(RELAY_OUT, OUTPUT);

  // Turn off relay
  digitalWrite(RELAY_OUT, LOW);

  // Set Pump Status to Off
  pump_Status = false;
  
  pinMode(enableRightMotor,OUTPUT);
  pinMode(rightMotorPin1,OUTPUT);
  pinMode(rightMotorPin2,OUTPUT);
  
  pinMode(enableLeftMotor,OUTPUT);
  pinMode(leftMotorPin1,OUTPUT);
  pinMode(leftMotorPin2,OUTPUT);

  myServo.attach(SOIL_SERVO);
  sensServo.attach(SENSOR_SERVO);
  
  myServo.write(90);
  sensServo.write(80);
  
  rotateMotor(0,0);   
}

void loop() {
  // put your main code here, to run repeatedly:
// Get temperature and Humidity
  temp = dht.readTemperature();
  hum = dht.readHumidity();

   // Get soil moisture value
  soilMoistureValue = analogRead(SENSOR_IN);

  // Determine soil moisture percentage value
  soilMoisturePercent = map(soilMoistureValue, DryValue, WetValue, 0, 100);

  // Keep values between 0 and 100
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  // Print raw value to serial monitor for sensor calibration
  Serial.println(soilMoistureValue);

  // Cycle values on OLED Display
  // Pump Status
  printOLED(35, "PUMP", 40, pump_status_text, 2000);
  // Temperature
  printOLED(35, "TEMP", 10, String(temp) + "C", 2000);
  // Humidity
  printOLED(30, "HUMID", 10, String(hum) + "%", 2000);
  // Moisture
  printOLED(35, "MOIST", 30, String(soilMoisturePercent) + "%", 2000);

  int distance = mySensor.ping_cm();

  //If distance is within 30 cm then adjust motor direction as below
  if (distance > 0 && distance < DISTANCE_TO_CHECK)
  {
    //Stop motors
    rotateMotor(0, 0);
    delay(500);  
    
    sensServo.write(180);
      delay(500);
    
    if (soilMoisturePercent <= pump_trigger) {
      // Turn pump on
      pumpOn();
    } 
  
    else {
      // Turn pump off
      pumpOff();
    }
  
    //Reverse motors
    rotateMotor(-MAX_MOTOR_ADJUST_SPEED, -MAX_MOTOR_ADJUST_SPEED);        
    delay(200);
    
    //Stop motors
    rotateMotor(0, 0);
    delay(500);
    
    //Rotate servo to left    
    myServo.write(180);
    delay(500);

    //Read left side distance using ultrasonic sensor
    int distanceLeft = mySensor.ping_cm();    

    //Rotate servo to right
    myServo.write(0);    
    delay(500);    

    //Read right side distance using ultrasonic sensor   
    int distanceRight = mySensor.ping_cm();

    //Bring servo to center
    myServo.write(90); 
    delay(500);        
    
    if (distanceLeft == 0 )
    {
      rotateMotor(MAX_MOTOR_ADJUST_SPEED, -MAX_MOTOR_ADJUST_SPEED);
      delay(200);
    }
    else if (distanceRight == 0 )
    {
      rotateMotor(-MAX_MOTOR_ADJUST_SPEED, MAX_MOTOR_ADJUST_SPEED);
      delay(200);
    }
    else if (distanceLeft >= distanceRight)
    {
      rotateMotor(MAX_MOTOR_ADJUST_SPEED, -MAX_MOTOR_ADJUST_SPEED);
      delay(200);
    }
    else
    {
      rotateMotor(-MAX_MOTOR_ADJUST_SPEED, MAX_MOTOR_ADJUST_SPEED);
      delay(200);      
    }
    rotateMotor(0, 0);    
    delay(200);     
  }
  else
  {
    rotateMotor(MAX_REGULAR_MOTOR_SPEED, MAX_REGULAR_MOTOR_SPEED);
  }
}

void pumpOn() {
  // Turn pump on
  digitalWrite(RELAY_OUT, HIGH);
  pump_status_text = "ON";
  pump_Status = true;

}

void pumpOff() {
  // Turn pump off
  digitalWrite(RELAY_OUT, LOW);
  pump_status_text = "OFF";
  pump_Status = false;

}

void printOLED(int top_cursor, String top_text, int main_cursor, String main_text, int delay_time){
  // Prints to OLED and holds display for delay_time
  display.setCursor(top_cursor, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(top_text);

  display.setCursor(main_cursor, 40);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(main_text);
  display.display();

  delay(delay_time);
  display.clearDisplay();
 
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed >= 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed >= 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }

  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));    
}
