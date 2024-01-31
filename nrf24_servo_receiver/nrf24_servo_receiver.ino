#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define SERVO_X 5
#define SERVO_Y 6
#define RADIO_CE 7
#define RADIO_CSN 8

#define MAX_MOVE_DELAY 30
#define MIN_MOVE_DELAY 1

RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

int x_min = 0;               
int x_max = 180;              
int y_min = 41;              
int y_max = 152;              
int x_start = 90;            
int y_start = y_min;
int x = x_start;
int y = y_start;

Servo servo_x;
Servo servo_y;
int x_incr = 1;               
int y_incr = 1;
int moveDelay = 30;

void setup()
{
  //Serial.begin(115200);
  servo_x.attach(SERVO_X);
  servo_y.attach(SERVO_Y);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.startListening();
  servo_x.write(x_start);
  servo_y.write(y_start);
}

void loop()
{
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    String strmsg = String(text);
    
    if(strmsg.compareTo("LR") == 0)
    {
      if(x > x_min){x = x - x_incr; servo_x.write(x); delay(moveDelay); }      
    }
    else if(strmsg.compareTo("LL") == 0)
    {
      if(x < x_max){x = x + x_incr; servo_x.write(x); delay(moveDelay); }
    }
    else if(strmsg.compareTo("LU") == 0)
    {
      if(y < y_max){y = y + y_incr; servo_y.write(y); delay(moveDelay); }
    }
    else if(strmsg.compareTo("LD") == 0)
    {
      if(y > y_min){y = y - y_incr; servo_y.write(y); delay(moveDelay); }
    }

    
    if(strmsg.startsWith("*") && strmsg.endsWith("#"))
    {
      int rotVal = strmsg.substring(1,strmsg.length() - 1).toInt();

      rotVal = map(rotVal, 1, 30, 45, 1);
      moveDelay = rotVal;
    }
  }
}
