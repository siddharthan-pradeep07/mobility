#include <AFMotor.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define left A2
#define right A4
//#define down A5
#define trigPin A0
#define echoPin A1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo myservo1;
Servo myservo2;

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

//SoftwareSerial BTserial(0, 1); // RX, TX for HC-05

int pos = 0; 
int Speed = 230;
//int downVal = analogRead(down);

char value;
long duration;
int distance;

void setup() {
  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //pinMode(13, OUTPUT);
  Serial.begin(9600);
  
  myservo1.attach(9);
  myservo2.attach(10);
  motor1.setSpeed(Speed);
  motor2.setSpeed(Speed);
  motor3.setSpeed(Speed);
  motor4.setSpeed(Speed);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Most common I2C address for 0.96" OLED
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Ready");
  display.display();
  delay(400);
}

void servo1() {
  for (pos = 0; pos <= 180; pos += 1) {
    myservo1.write(pos);
    // myservo2.write(pos);
    delay(15);
  }
    delay(2000);
    forward();
    delay(2000);
    myservo1.write(0);
  // for (pos = 180; pos >= 0; pos -= 1) {
  //   myservo1.write(pos);
  //   //myservo2.write(pos);
  //   delay(15);
  // }
}

void fog() {
  for (pos = 0; pos <= 90; pos += 1) {
    myservo2.write(pos);
    // myservo2.write(pos);
    delay(15);
  }
    //digitalWrite(13, HIGH);
    //delay(7000);
    //digitalWrite(13, LOW);
    //elay(1000);
    //myservo2.write(0);


  // for (pos = 180; pos >= 0; pos -= 1) {
  //   myservo1.write(pos);
  //   //myservo2.write(pos);
  //   delay(15);
  // }
}

void showBluetoothValue(char val) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  //display.print("BT: ");
  display.println(val);
  Serial.println(val);
  display.display();
}


// Measure distance using ultrasonic sensor
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout for ~3.4m
  int distanceCm = duration * 0.034 / 2;
  return distanceCm;
}

void avoidObstacle() {
  // Stop for a moment
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  delay(200);

  // Slight right
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(250);

  // Slight left
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(250);

  // Resume release
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
  delay(50);
}

void obstacleavoid() 
{
   int distance = getDistance(); // Use robust timeout version
   int leftVal = analogRead(left);
   int rightVal = analogRead(right);

   // Simple error check for bad sensor reads
   if (distance <= 0 || distance > 200) {
     // No valid reading, just stop and return
     movestop();
     return;
   }

   Serial.print("Distance: ");
   Serial.println(distance);

   if (distance < 30) // Example: obstacle within 30cm
   {   
      backward();
      delay(500); // Short reverse
      movestop();
      delay(200);
      lookright();
      delay(400); // Short turn
      movestop();
      delay(100);
   }
   else if (distance > 30 && leftVal == 0 && rightVal == 0)
   {
      forward();
      delay(150); // Brief forward, loop will quickly check again
   }
}


void movestop()
{
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void forward()
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
 
}

void backward()
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
 
}
    
void  lookright()
{
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
 
}

void lookleft()
{
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);
 
}


void linefollower() {
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance < 20) { // Obstacle detected within 20cm
    avoidObstacle();
    return; // Skip rest to avoid double commands
  }

  int leftVal = analogRead(left);
  int rightVal = analogRead(right);
  

  if(leftVal == 0 && rightVal == 0) {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
  }
  else if (leftVal == 0 && rightVal != 0) {
    // turn right 
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
  }
  else if (leftVal != 0 && rightVal == 0) {
    // Turn left
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
  }
  else if (leftVal != 0 && rightVal != 0) {
    // Stop
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
  }
}

void loop() {
  //  obstacleavoid();
    // delay(7000);
    // digitalWrite(13, LOW);
    delay(1000);
  if (Serial.available() > 0) {
    value = Serial.read();
    showBluetoothValue(value);
  }
  if (value == 'f') {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
  }//else if (downVal != 0){
    //servo1();
  else if (value == 'b') {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
  } else if (value == 'l') {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
  } else if (value == 'r') {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
  } else if (value == 'h') {
    servo1();
  } else if (value == 'n') {
    linefollower();
  } else if (value == 'o') {
    obstacleavoid();
  } else if (value == 'f') {
    fog();
  } else {
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
  }
}
