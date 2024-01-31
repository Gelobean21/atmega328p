#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/sleep.h>

#define RADIO_CE 9
#define RADIO_CSN 10

RF24 radio(RADIO_CE, RADIO_CSN);
const byte address[6] = "00001";

#define M1_PWM 6      //PWM
#define M1_DIR 5      //PWM
#define M2_PWM 3      //PWM
#define M2_DIR 4      //NOT PWM

bool modStat = false; //motor 1 dir stat
bool mopStat = false; //motor 1 pwm stat
bool mtdStat = false; //motor 2 dir stat
bool mtpStat = false; //motor 2 pwm stat

#define MOVE_DELAY 15

#define LED 2
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

int rotVal = 100;

void setup()
{
  Serial.begin(115200);
  Serial.println("Welcome!");
  pinMode(LED, OUTPUT);

  if(radio.begin())
  {
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    Serial.println("radio started properly");
    //MoveForward(moveDelay); 
  }
  else
  {
    Serial.println("radio error");
    //MoveBackward(moveDelay);
  }

  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  //pinMode(M1_PWM, OUTPUT);
  //pinMode(M2_PWM, OUTPUT);

  analogWrite(M1_PWM, 0);
  digitalWrite(M1_DIR, LOW);
  analogWrite(M2_PWM, 0);
  digitalWrite(M2_DIR, LOW);
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

   digitalWrite(LED, ledState);
  }
    
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    //Serial.println(text[0]);    
    String strmsg = String(text);

    if(strmsg.compareTo("LL") == 0)
    {
      MoveLeft();
    }
    else if(strmsg.compareTo("LR") == 0)
    {
      MoveRight(); 
    }
    else if(strmsg.compareTo("LU") == 0)
    {
      MoveForward();
    }
    else if(strmsg.compareTo("LD") == 0)
    {
      MoveBackward();
    }

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

   digitalWrite(LED, ledState);
  
    
    if(strmsg.startsWith("*") && strmsg.endsWith("#"))
    {
      rotVal = strmsg.substring(1,strmsg.length() - 1).toInt();

      rotVal = map(rotVal, 1, 30, 1, 255);
    }
  }
}

void MoveForward()
{
    Serial.println("MOVE FORWARD");
    digitalWrite(M1_DIR, LOW);
    analogWrite(M1_PWM, rotVal);
    digitalWrite(M2_DIR, LOW);
    analogWrite(M2_PWM, rotVal);
    StopMotors();
}

void MoveBackward()
{
  Serial.println("MOVE BACKWARD");
  digitalWrite(M1_DIR, HIGH);
  analogWrite(M1_PWM, map(rotVal, 1, 255, 255, 1));
  digitalWrite(M2_DIR, HIGH);
  analogWrite(M2_PWM, map(rotVal, 1, 255, 255, 1));
  StopMotors();
}

void MoveLeft()
{
  Serial.println("MOVE LEFT");
  digitalWrite(M1_DIR, HIGH);
  analogWrite(M1_PWM, map(rotVal, 1, 255, 255, 1));
  digitalWrite(M2_DIR, LOW);
  analogWrite(M2_PWM, rotVal);
  StopMotors();
}

void MoveRight()
{
  Serial.println("MOVE right");
  digitalWrite(M1_DIR, LOW);
  analogWrite(M1_PWM, rotVal);
  digitalWrite(M2_DIR, HIGH);
  analogWrite(M2_PWM, map(rotVal, 1, 255, 255, 1));
  StopMotors();
}

void StopMotors()
{
  delay(MOVE_DELAY);
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
  //digitalWrite(M1_PWM, LOW);
  digitalWrite(M1_DIR, LOW);
  //digitalWrite(M2_PWM, LOW);
  digitalWrite(M2_DIR, LOW);
}
