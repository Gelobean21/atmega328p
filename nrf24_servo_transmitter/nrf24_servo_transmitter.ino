#include <Joystick.h>
#include <AxisJoystick.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(10, 9);
const byte address[6] = "00001";

#define SW_PIN A4
#define VRX_PIN A2
#define VRY_PIN A3

#define ROT_CK A1
#define ROT_DT A0
#define ROT_MAX_VAL 30
#define ROT_MIN_VAL 1

int RotPosition = 1;
int rotation;
int RotValue;
boolean LeftRight;

Joystick* joystic;

// the setup function runs once when you press reset or power the board
void setup() {
  //Serial.begin(9600);
  if(radio.begin())
  {
    Serial.println("RADIO OK!");
  }
  else {
    Serial.println("RADIO ERR!");
  }
  radio.openWritingPipe(address);
  //radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  joystic = new AxisJoystick(SW_PIN, VRX_PIN, VRY_PIN);
  joystic->calibrate(30, 700);

  pinMode(ROT_CK, INPUT);
  pinMode(ROT_DT, INPUT);
  rotation = digitalRead(ROT_CK);
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

void loop() {

  RotValue = digitalRead(ROT_CK);
  if (RotValue != rotation)
  { 
    if (digitalRead(ROT_DT) != RotValue)
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

    //HERE
	}
	rotation = RotValue;
  

  if(joystic->isUp())
  {
    radio.write("U",1);
    Serial.println("UP");  

  }
  if(joystic->isDown())
  {
    radio.write("D", 1);
    Serial.println("DOWN");
  }
  
  if(joystic->isLeft())
  {
    radio.write("L", 1);
    Serial.println("LEFT");
  }

  if(joystic->isRight())
  {
    radio.write("R", 1);
    Serial.println("RIGHT");
  }

  if(joystic->isPress())
  {
    radio.write("C", 1);
    Serial.println("CLICKED");
  } 
}
