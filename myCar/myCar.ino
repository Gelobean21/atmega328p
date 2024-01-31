#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define RADIO_CE 10
#define RADIO_CSN 9
#define SERVO_PIN 5
#define TRIG_PIN A3
#define ECHO_PIN A2

int leftDistance = 0, rightDistance = 0, centerDistance = 0;
byte moveDelay = 10;
int delayToTurn = 200;
byte cmPerMove = 3;
bool isLeftScanned = 0, isRightScanned = 0, 
isCenterScanned = 0, isCarMoved = 0,
isRestricted = 0, isPinged = 0;
int servoturndelay = 750;

unsigned long previousMillis = 0;
unsigned long pingPrevious = 0;

Servo carServo;
#define B_RX_PIN A0
#define B_TX_PIN A1
SoftwareSerial btooth(B_RX_PIN, B_TX_PIN);
RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

#define enA 6
#define enB 3
#define M1_L 7
#define M1_R 8
#define M2_L 2
#define M2_R 4

bool mode = false;  //true for automatic

void setup()
{
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.setTimeout(100);
  Serial.begin(115200);
  btooth.begin(115200);

  carServo.attach(SERVO_PIN);

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  /*pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);*/
  pinMode(M1_L, OUTPUT);
  pinMode(M1_R, OUTPUT);
  pinMode(M2_L, OUTPUT);
  pinMode(M2_R, OUTPUT);
  /*digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);*/
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  carServo.write(90);
  delay(500);
}

void loop()
{
  /*
    while(Serial.available())
    {
    String received = Serial.readString();

    if(received == "C")
    {
      mode = !mode;
      Serial.print("MODE : ");
      Serial.println(mode);
    }
    else if(received == "U")
    {
      MoveForward();
    }
    else if(received == "D")
    {
      MoveBackward();
    }
    else if(received == "L")
    {
      MoveLeft();
    }
    else if(received == "R")
    {
      MoveRight();
    }
    }
  */

  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    String strmsg = String(text);

    if(strmsg.compareTo("LL") == 0)
    {
      MoveRight(moveDelay); 
    }
    else if(strmsg.compareTo("LR") == 0)
    {
      MoveLeft(moveDelay);
    }
    else if(strmsg.compareTo("LU") == 0)
    {
      MoveForward(moveDelay);
    }
    else if(strmsg.compareTo("LD") == 0)
    {
      MoveBackward(moveDelay);
    }
    else if(strmsg.compareTo("S1") == 0)
    {
      mode = !mode;
    }

    
    if(strmsg.startsWith("*") && strmsg.endsWith("#"))
    {
      int rotVal = strmsg.substring(1,strmsg.length() - 1).toInt();

      rotVal = map(rotVal, 1, 30, 1, 255);
      
      analogWrite(enA, rotVal);
      analogWrite(enB, rotVal);
      
    }
  }
  /*
  while (btooth.available())
  {
    String received = Serial.readString();

    if (received == "C")
    {
      mode = !mode;
      if (mode)
      {
        previousMillis = millis();
      }
      Serial.print("MODE : ");
      Serial.println(mode);
    }
    else if (received == "U")
    {
      if(!isRestricted)
      {
        MoveForward(moveDelay);
      }
    }
    else if (received == "D")
    {
      MoveBackward(moveDelay);
    }
    else if (received == "L")
    {
      MoveLeft(moveDelay);
    }
    else if (received == "R")
    {
      MoveRight(moveDelay);
    }
  }
  */

  if (mode)
  {
    unsigned long currentMillis = millis();
    unsigned long result = currentMillis - previousMillis;

    if (result >= (servoturndelay * 4) + 2000 && isCarMoved)
    {
      isRightScanned = false;
      isCenterScanned = false;
      isLeftScanned = false;
      isCarMoved = false;
      carServo.write(10); //car eye go left
      previousMillis = currentMillis;  //reset timer to 0;
    }
    else if (result >= servoturndelay && !isRightScanned)  //after timer reset this should trigger first
    {
      rightDistance = GetDistance();
      isRightScanned = true;
      Serial.print("RIGHT DISTANCE : ");
      Serial.println(rightDistance);
      carServo.write(90); //car eye go center
    }
    else if (result >= servoturndelay * 2 && !isCenterScanned)  //this should trigger after right scan
    {
      centerDistance = GetDistance();
      isCenterScanned = true;
      Serial.print("CENTER DISTANCE : ");
      Serial.println(centerDistance);
      carServo.write(180);  //car eye go left
    }
    else if (result >= servoturndelay * 3 && !isLeftScanned)  //this should trigger after center scan
    {
      leftDistance = GetDistance();
      isLeftScanned = true;
      Serial.print("LEFT DISTANCE : ");
      Serial.println(leftDistance);
      carServo.write(90);  //car eye go center again
    }
    else if (result >= servoturndelay * 4 && isRightScanned && isCenterScanned 
          && isLeftScanned && !isCarMoved)   //this should trigger after left scan
    {
      
      Serial.println("MOVING");
      isCarMoved = true;
      
      if (centerDistance < 20)
      {
        int delayTime = ((20 - centerDistance) / cmPerMove) * 300;
        MoveBackward(delayTime);
        Serial.println("MOVE BACKWARD(" + String(delayTime) + ")");

        if (leftDistance > 20 && rightDistance > 20)
        {
          if (leftDistance > rightDistance)
          {
            delayTime = (leftDistance / cmPerMove) * moveDelay;
            MoveLeft(delayToTurn);
            MoveForward(delayTime);
          }
          else if (rightDistance > leftDistance)
          {
            delayTime = (rightDistance / cmPerMove) * moveDelay;
            MoveRight(delayToTurn);
            MoveForward(delayTime);
          }
          else
          {
            if (random(0, 2) == 0)
            {
              delayTime = (leftDistance / cmPerMove) * moveDelay;
              MoveLeft(delayToTurn);
              MoveForward(delayTime);
            }
            else
            {
              delayTime = (rightDistance / cmPerMove) * moveDelay;
              MoveRight(delayToTurn);
              MoveForward(delayTime);
            }
          }
        }
      }
      else if (leftDistance == rightDistance)
      {
        Serial.println("LEFT DISTANCE == RIGHT DISTANCE");
        if (leftDistance > 20)
        {
          Serial.println("LEFT & RIGHT DISTANCE" + String(leftDistance));
          int delayTime = 0;
          if (random(0, 2) == 0)
          {
            delayTime = (leftDistance / cmPerMove) * moveDelay;
            Serial.println("TURN LEFT AND MOVE FORWARD(" + String(delayTime) + ")");
            MoveLeft(delayToTurn);
            MoveForward(delayTime);
          }
          else
          {
            delayTime = (rightDistance / cmPerMove) * moveDelay;
            Serial.println("TURN RIGHT AND MOVE FORWARD(" + String(delayTime) + ")");
            MoveRight(delayToTurn);
            MoveForward(delayTime);
          }
        }
      }
      else if (centerDistance > leftDistance && centerDistance > rightDistance)
      {
        int delayTime = (centerDistance / cmPerMove) * moveDelay;
        Serial.println("MOVE FORWARD(" + String(delayTime) + ")");
        MoveForward(delayTime);
        Serial.println(delayTime);
      }
      else if (rightDistance > centerDistance && rightDistance > leftDistance)
      {
        int delayTime = (rightDistance / cmPerMove) * moveDelay;
        Serial.println("TURN RIGHT AND MOVE FORWARD(" + String(delayTime) + ")");
        MoveRight(delayToTurn);
        MoveForward(delayTime);
        Serial.println(delayTime);
      }
      else if (leftDistance > centerDistance && leftDistance > rightDistance)
      {
        int delayTime = (leftDistance / cmPerMove) * moveDelay;
        Serial.println("TURN LEFT AND MOVE FORWARD(" + String(delayTime) + ")");
        MoveLeft(delayToTurn);
        MoveForward(delayTime);
        Serial.println(delayTime);
      }
      
    }
  }
  /*
  else
  {
    if(GetDistance() < 20)
    {
      isRestricted = true;
    }
    else
    {
      isRestricted = false;
    }
  }*/
}

int GetDistance()
{
  int retval = 0;
  unsigned long currentMillis = millis();
  unsigned long result = currentMillis - pingPrevious;
  digitalWrite(TRIG_PIN, LOW);
  delay(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float fDistance = pulseIn(ECHO_PIN, HIGH);
  fDistance = fDistance / 58;
  retval = (int)fDistance;
  
  return retval;
}

void MoveForward(int delayTime)
{
  Serial.println("MOVE FORWARD");
    digitalWrite(M1_L, HIGH);
    digitalWrite(M1_R, LOW);
    digitalWrite(M2_L, HIGH);
    digitalWrite(M2_R, LOW);
    delay(delayTime);
    StopMotors();
}

void MoveBackward(int delayTime)
{
  Serial.println("MOVE BACKWARD");
  digitalWrite(M1_L, LOW);
  digitalWrite(M1_R, HIGH);
  digitalWrite(M2_L, LOW);
  digitalWrite(M2_R, HIGH);
  delay(delayTime);
  StopMotors();
}

void MoveLeft(int delayTime)
{
  Serial.println("MOVE LEFT");
  digitalWrite(M1_L, LOW);
  digitalWrite(M1_R, HIGH);
  digitalWrite(M2_L, HIGH);
  digitalWrite(M2_R, LOW);
  delay(delayTime);
  StopMotors();
}

void MoveRight(int delayTime)
{
  Serial.println("MOVE RIGHT");
  digitalWrite(M1_L, HIGH);
  digitalWrite(M1_R, LOW);
  digitalWrite(M2_L, LOW);
  digitalWrite(M2_R, HIGH);
  delay(delayTime);
  StopMotors();
}

void StopMotors()
{
  digitalWrite(M1_L, LOW);
  digitalWrite(M1_R, LOW);
  digitalWrite(M2_L, LOW);
  digitalWrite(M2_R, LOW);
}
