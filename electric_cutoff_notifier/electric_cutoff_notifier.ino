#include <SoftwareSerial.h>
#include <MyButton.h>

#define LED_ARMING A2
#define LED_MONITOR_POWER A3
#define BATTERY_OUTPUT A5

MyButton tactile_arming(A0, 0, 1, 110);
MyButton tactile_send_sample(A1, 0, 1, 110);
MyButton MONITOR_1(5, 1, 1, 2000);

//09272843461
//09285023831 - ma'am Sarah
//09175953012 - sir Ernest
//09064449966 - sir Chad
String phoneNumbers[] = { "+639064449966","+639166688563", "+639323492355", "+639236592251", "+639953139884", "+639176916040", "+639285023831", "+639175953012" };
String numberForSample[] = { "+639064449966","+639166688563", "+639323492355", "+639236592251", "+639953139884", "+639176916040" };

unsigned long previousMillis = 0;
const long interval = 1000;

SoftwareSerial sim800l(10, 9);

boolean isArmed = true;
boolean isMessageSent = false;
boolean isLowVoltageBatSent = false;

float last_battery_voltage = 0;
String cutoff_message = "The electricity supply went out!, don't reply to this number!";
String sample_message = "This is a test message !, will be followed by call, please don't answer";
String return_message = "Power has been restored!, don't reply to this number!";
String low_voltage_message = "Low Battery!, don't reply to this number!";

void setup() {
  Serial.begin(9600);
  sim800l.begin(9600);

  pinMode(LED_ARMING, OUTPUT);
  pinMode(LED_MONITOR_POWER, OUTPUT);

  digitalWrite(LED_ARMING, HIGH);
  digitalWrite(LED_MONITOR_POWER, LOW);
}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    updateBatteryVoltage();
  }

  if (tactile_send_sample.isClicked()) {
    sampleText();
    ////Serial.println("Sent sample message");
    sampleCall();
    //Serial.println("Called testing number/s");
  }

  if (tactile_arming.isClicked()) {
    isArmed = !isArmed;
    if (isArmed) {
      digitalWrite(LED_ARMING, HIGH);
      //Serial.println("ARMED!");
      isMessageSent = false;
    } else {
      digitalWrite(LED_ARMING, LOW);
      //Serial.println("DISARMED!");
    }
  }

  if (MONITOR_1.stat()) {
    digitalWrite(LED_MONITOR_POWER, HIGH);
    //Serial.println("MONITOR ON!");

    if (isMessageSent) {  // from blackout
      sendMessage(return_message + "\n" + "battery voltage : " + last_battery_voltage);
      //Serial.println("SENT RETURN MESSAGE");
      delay(5000);
      callTeam();
      //Serial.println("CALLED TEAM");
      isMessageSent = false;  //reset from start
    }
  } else {
    digitalWrite(LED_MONITOR_POWER, LOW);
    ////Serial.println("MONITOR OFF!");
  }

  if (MONITOR_1.isClicked()) {
    digitalWrite(LED_MONITOR_POWER, LOW);  // POWER INTERUPTED
    if (isArmed) {
      if (!isLowVoltageBatSent) {
        if (last_battery_voltage < 3.80) {
          sendMessage(low_voltage_message + "\n" + "battery voltage : " + last_battery_voltage);
          //Serial.println("SENT LOW VOLTAGE MESSAGE");
          isLowVoltageBatSent = true;
        }
      }
      if (!isMessageSent) {
        sendMessage(cutoff_message + "\n" + "battery voltage : " + last_battery_voltage);
        //Serial.println("TEXTED TEAM");
        delay(5000);
        callTeam();
        //Serial.println("CALLED TEAM");
        isMessageSent = true;
      }
    }
  }
}

void sendMessage(String message) {
  for (int i = 0; i < sizeof(phoneNumbers) / sizeof(phoneNumbers[0]); i++) {
    sim800l.println("AT+CMGF=1");
    delay(500);
    sim800l.println("AT+CMGS=\"" + phoneNumbers[i] + "\"\r");
    delay(500);
    sim800l.print(message);
    delay(500);
    sim800l.write(26);
    //Serial.println("message sent to : " + phoneNumbers[i]);
    delay(5000);
  }
}

void callTeam() {
  for (int i = 0; i < sizeof(phoneNumbers) / sizeof(phoneNumbers[0]); i++) {
    sim800l.print(F("ATD "));
    sim800l.print(phoneNumbers[i]);
    sim800l.print(F(";\r\n"));
    delay(20000);
    sim800l.println("ATH");
    delay(500);
    //Serial.println("called : " + phoneNumbers[i]);
    delay(5000);
  }
}

void sampleCall() {
  for (int i = 0; i < sizeof(numberForSample) / sizeof(numberForSample[0]); i++) {
    sim800l.print(F("ATD "));
    sim800l.print(numberForSample[i]);
    sim800l.print(F(";\r\n"));
    delay(20000);
    sim800l.println("ATH");
    delay(500);
    //Serial.println("called : " + phoneNumbers[i]);
    delay(5000);
  }
}

void sampleText() {
  for (int i = 0; i < sizeof(numberForSample) / sizeof(numberForSample[0]); i++) {
    sim800l.println("AT+CMGF=1");
    delay(500);
    sim800l.println("AT+CMGS=\"" + numberForSample[i] + "\"\r");
    delay(500);
    sim800l.print(sample_message);
    delay(500);
    sim800l.write(26);
    delay(5000);
  }
}

void updateBatteryVoltage() {
  int sensorValue = analogRead(BATTERY_OUTPUT);
  float voltage = sensorValue * (5.00 / 1023.00) * 2;  //convert the value to a true voltage.
  last_battery_voltage = voltage;
}
/* 
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available()) 
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}*/
