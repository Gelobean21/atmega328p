#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Joystick.h>
#include <AxisJoystick.h>
#include <MyButton.h>
#include <Wire.h>

const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
double x;
double y;

#define RADIO_CE 9 //pb1
#define RADIO_CSN 10 //pb2
#define LEFT_JOYSTICK_X A2
#define LEFT_JOYSTICK_Y A3
#define LEFT_JOYSTICK_SWITCH 2//PD2
#define RIGHT_JOYSTICK_X A0
#define RIGHT_JOYSTICK_Y A1
#define RIGHT_JOYSTICK_SWITCH 8//PB0
#define SWITCH_1 3//PD3
#define SWITCH_2 4//PD4
#define SWITCH_3 7//PD7
#define ROT_CLOCK 6//PD6
#define ROT_DATA 5//PD5
#define ROT_MAX_VAL 30
#define ROT_MIN_VAL 1

RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

int RotPosition = 1;
int rotation;
int RotValue;
boolean LeftRight;

Joystick* ljoystic;
Joystick* rjoystic;

MyButton sw_one(SWITCH_1, 0, 1, 70);
MyButton sw_two(SWITCH_2, 0, 1, 70);
MyButton sw_three(SWITCH_3, 0, 1, 70);
MyButton sw_l(LEFT_JOYSTICK_SWITCH, 0, 1, 70);
MyButton sw_r(RIGHT_JOYSTICK_SWITCH, 0, 1, 70);

bool isMpuFound = false;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  if(Wire.endTransmission(true) == 0)
  {
    isMpuFound = true;
  }

  ljoystic = new AxisJoystick(LEFT_JOYSTICK_SWITCH, LEFT_JOYSTICK_X, LEFT_JOYSTICK_Y);
  rjoystic = new AxisJoystick(RIGHT_JOYSTICK_SWITCH, RIGHT_JOYSTICK_X, RIGHT_JOYSTICK_Y);
  Serial.begin(115200);
  radio.begin();
  radio.openWritingPipe(address);
  radio.stopListening();
  pinMode(ROT_CLOCK, INPUT_PULLUP);
  pinMode(ROT_DATA, INPUT_PULLUP);
  pinMode(LEFT_JOYSTICK_SWITCH, INPUT_PULLUP);
  pinMode(RIGHT_JOYSTICK_SWITCH, INPUT_PULLUP);
  rotation = digitalRead(ROT_CLOCK);
  delay(100);
}

void loop() 
{
  if(isMpuFound)
  {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);

  if(y > 25 && y <= 180)
  {
    radio.write("U", 1);
  }
  if(y > 180 && y < 335)
  {
    radio.write("D", 1);
  }
  if(x > 25 && x <= 180)
  {
    radio.write("L", 1);
  }
  if(x > 180 && x < 335)
  {
    radio.write("R", 1);
  }

  }
  
  RotValue = digitalRead(ROT_CLOCK);
  if (RotValue != rotation)
  { 
    if (digitalRead(ROT_DATA) != RotValue)
    {  // Clockwise
      if(RotPosition < ROT_MAX_VAL)
      {
        RotPosition++;
        SendPosition();
      }
      LeftRight = true;
    } 
    else 
    { //Counterclockwise
      LeftRight = false;
      if(RotPosition > ROT_MIN_VAL)
      {
        RotPosition--;
        SendPosition();
      }
    }
	}
	rotation = RotValue;



  if(ljoystic->isUp())
  {
    radio.write("LU",2);
    Serial.println("LEFT UP");
  }
  if(ljoystic->isDown())
  {
    radio.write("LD", 2);
    Serial.println("LEFT DOWN");
  }
  if(ljoystic->isLeft())
  {
    radio.write("LL", 2);
    Serial.println("LEFT LEFT");
  }
  if(ljoystic->isRight())
  {
    radio.write("LR", 2);
    Serial.println("LEFT RIGHT");
  }
  
  /*if(ljoystic->isPress())
  {
    radio.write("LP", 2);
    Serial.println("LEFT PRESSED");
  }*/

  if(rjoystic->isUp())
  {
    radio.write("RU",2);
    Serial.println("RIGHT UP");
  }
  if(rjoystic->isDown())
  {
    radio.write("RD", 2);
    Serial.println("RIGHT DOWN");
  }
  if(rjoystic->isLeft())
  {
    radio.write("RL", 2);
    Serial.println("RIGHT LEFT");
  }
  if(rjoystic->isRight())
  {
    radio.write("RR", 2);
    Serial.println("RIGHT RIGHT");
  }
  /*
  if(rjoystic->isPress())
  {
    radio.write("RP", 2);
    Serial.println("RIGHT PRESSED");
  }*/

  if(sw_l.isClicked())
  {
    radio.write("LP", 2);
    Serial.println("LEFT PRESSED");
  }
  if(sw_r.isClicked())
  {
    radio.write("RP", 2);
    Serial.println("RIGHT PRESSED");
  }
  if(sw_one.isClicked())
  {
    radio.write("S1", 2);
    Serial.println("SW ONE PRESSED");
  }
  if(sw_two.isClicked())
  {
    radio.write("S2", 2);
    Serial.println("SW TWO PRESSED");
  }
  if(sw_three.isClicked())
  {
    radio.write("S3", 2);
    Serial.println("SW THREE PRESSED");
  }
}

void SendPosition()
{
  Serial.print("RotPosition : ");
  Serial.println(RotPosition);
  String str = "*" + String(RotPosition) + "#";
  int str_len = str.length() + 1;
  char message[str_len];
  str.toCharArray(message, str_len);
  radio.write(message, sizeof(message));
}
