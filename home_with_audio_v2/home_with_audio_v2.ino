#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <MyButton.h>
#include <DHT.h>

SoftwareSerial mySoftwareSerial(7, 8); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

const byte maxbuff = 10;
const byte ender = '\n';
char request[maxbuff];
byte index = 0;

#define light_l1 A5
#define light_l2 A4
#define light_l3 A3
#define light_l4 A2
//#define door_status 9
//#define light_wifi A0;
#define light_mqtt 5

MyButton tactile_l1(A1,0,1,120); //pin 5 , ACTIVE LOW , INPUT PULLUP , 120ms
MyButton tactile_l2(2,0,1,120);
MyButton tactile_l3(3,0,1,120);
MyButton tactile_l4(4,0,1,120);

bool mqtt_switch_l1 = 0;
bool mqtt_switch_l2 = 0;
bool mqtt_switch_l3 = 0;
bool mqtt_switch_l4 = 0;

bool status_light_l1 = 0;
bool status_light_l2 = 0;
bool status_light_l3 = 0;
bool status_light_l4 = 0;

bool status_door = 0;

const unsigned int duration = 60000;
int startmillis = 0, currentmillis = 0;

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  pinMode(light_l1, OUTPUT);
  pinMode(light_l2, OUTPUT);
  pinMode(light_l3, OUTPUT);
  pinMode(light_l4, OUTPUT);

  pinMode(light_mqtt, OUTPUT);
  //pinMode(door_status, INPUT_PULLUP);

  digitalWrite(light_mqtt, LOW);
  digitalWrite(light_l1, LOW);
  digitalWrite(light_l2, LOW);
  digitalWrite(light_l3, LOW);
  digitalWrite(light_l4, LOW);

  dht.begin();
  startmillis = millis();

  if (!myDFPlayer.begin(mySoftwareSerial)) 
  {
  /*while(true){
      delay(0); // HANG IF NOT FOUND
  }*/
  }

  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(15);  //Set volume value. From 0 to 30
  myDFPlayer.playMp3Folder(1); // welcome mp3 - 0001.mp3
}

void loop() {
currentmillis = millis();
if(currentmillis - startmillis >= duration){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float hic = dht.computeHeatIndex(t, h, false);

  if(!isnan(h) || !isnan(t)){
  Serial.print("H!"); //humidity
  Serial.print(h);
  Serial.print("E");
  
  Serial.print("H@"); //temperature
  Serial.print(t);
  Serial.print("E");

  Serial.print("H$"); //heat index
  Serial.print(hic);
  Serial.print("E");
  }
  
  startmillis = millis();
}
/*
if(digitalRead(door_status) == HIGH) // high == bukas - low = sara
{
  if(status_door == 1) //sd = 0 = bukas, 1 = sara
  {
    status_door = 0;
    Serial.print("HKE");
    myDFPlayer.playMp3Folder(14);  // Ang pinto ay bumukas
    delay(150);
  }
}

if(digitalRead(door_status) == LOW) // high == bukas - low = sara
{
  if(status_door == 0) //sd = 0 = bukas, 1 = sara
  {
    status_door = 1;
    Serial.print("HLE");
    myDFPlayer.playMp3Folder(15);  // Ang pinto ay sumara
    delay(150);
  }
}
*/
while(Serial.available() > 0){
    byte incoming = Serial.read();
    
 if(incoming == ender){ //found ender
  searchIsOver();
 }
 else{ //end not found
   request[index] = incoming;
   index++;
   
  //max buff
  if(index == maxbuff - 1){
   searchIsOver();
  }
 }
}

if(mqtt_switch_l1 == 0 || tactile_l1.isClicked()){ //TURN ON
 if(status_light_l1 == 1){
  digitalWrite(light_l1, LOW);
  Serial.print("HAE");
  myDFPlayer.playMp3Folder(2);                                     // KUSINA BUHAY.MP3
  status_light_l1 = 0; mqtt_switch_l1 = 0;
  delay(150);
 }
}

if(mqtt_switch_l1 == 1 || tactile_l1.isClicked()){  //TURN OFF
 if(status_light_l1 == 0){
  digitalWrite(light_l1, HIGH);
  Serial.print("HBE");
  myDFPlayer.playMp3Folder(3);                                     // KUSINA PATAY.MP3
  status_light_l1 = 1; mqtt_switch_l1 = 1;
  delay(150); 
 }
}

if(mqtt_switch_l2 == 0 || tactile_l2.isClicked()){ //TURN ON
 if(status_light_l2 == 1){
  digitalWrite(light_l2, LOW);
  Serial.print("HCE");                                    // l3 BUHAY.MP3
  myDFPlayer.playMp3Folder(4);
  status_light_l2 = 0; mqtt_switch_l2 = 0;
  delay(150);
 }
 
}

if(mqtt_switch_l2 == 1 || tactile_l2.isClicked()){ //TURN OFF
 if(status_light_l2 == 0){
  digitalWrite(light_l2, HIGH);
  Serial.print("HDE");                                   // l3 PATAY.MP3
  myDFPlayer.playMp3Folder(5);
  status_light_l2 = 1; mqtt_switch_l2 = 1;
  delay(150);
 }
}

if(mqtt_switch_l3 == 0 || tactile_l3.isClicked()){  //TURN ON
 if(status_light_l3 == 1){
  digitalWrite(light_l3, LOW);
  Serial.print("HFE");                                  // l1 BUHAY.MP3
  myDFPlayer.playMp3Folder(6);
  status_light_l3 = 0; mqtt_switch_l3 = 0;
  delay(150);
 }
}

if(mqtt_switch_l3 == 1 || tactile_l3.isClicked()){  //TURN OFF
 if(status_light_l3 == 0){
  digitalWrite(light_l3, HIGH);
  Serial.print("HGE");                                 // l1 PATAY.MP3
  myDFPlayer.playMp3Folder(7);
  status_light_l3 = 1; mqtt_switch_l3 = 1;
  delay(150);
 }
}

if(mqtt_switch_l4 == 0 || tactile_l4.isClicked()){  //TURN ON
 if(status_light_l4 == 1){
  digitalWrite(light_l4, LOW);
  Serial.print("HIE");                                  // LABAS BUHAY.MP3!!!
  myDFPlayer.playMp3Folder(8);
  status_light_l4 = 0; mqtt_switch_l4 = 0;
  delay(150);
 }
}

if(mqtt_switch_l4 == 1 || tactile_l4.isClicked()){ //TURN OFF
 if(status_light_l4 == 0){
  digitalWrite(light_l4, HIGH);
  Serial.print("HJE");
  myDFPlayer.playMp3Folder(9);                                     // LABAS PATAY.MP3!!!
  status_light_l4 = 1; mqtt_switch_l4 = 1;
  delay(150);
 }
}
}

void searchIsOver(){
  request[index++] = '\0';
  
  String search = String(request);
  if(search == "L1OFF"){                         // KUSINA PATAY!!!
    mqtt_switch_l1 = 1;
  }
  if(search == "L1ON"){
    mqtt_switch_l1 = 0;
  }
  if(search == "L2OFF"){                       // KWARTO PATAY!!!
    mqtt_switch_l2 = 1;
  }
  if(search == "L2ON"){
    mqtt_switch_l2 = 0;
  }
  if(search == "L3OFF"){                       // SALA PATAY!!!
    mqtt_switch_l3 = 1;
  }
  if(search == "L3ON"){
    mqtt_switch_l3 = 0;
  }
  if(search == "L4OFF"){                       // LABAS PATAY!!! 
    mqtt_switch_l4 = 1;
  }
  if(search == "L4ON"){
    mqtt_switch_l4 = 0;
  }/*
  if(search == "ISUCC"){
    //digitalWrite(light_wifi, HIGH);
  }
  if(search == "IERR"){
    //digitalWrite(light_wifi, LOW);
  }*/
  if(search == "MSUCC"){
    digitalWrite(light_mqtt, HIGH);
  }
  if(search == "MERR"){
    digitalWrite(light_mqtt, LOW);
  }
  if(search == "LS"){
    print_lightstatus();
  }
  if(search == "SP")                  //SHOPEE
  {
    myDFPlayer.playMp3Folder(13);
  }
  if(search.startsWith("SV"))        //SEE VOLUME
  {
    search.remove(0,2);
    int vol = search.toInt();
    myDFPlayer.volume(vol);
    if(vol == 1){
      myDFPlayer.playMp3Folder(10);   // volume set to minimum
    }
    else if(vol == 30){
      myDFPlayer.playMp3Folder(11);  // volume set to maximum
    }
    else{
      myDFPlayer.playMp3Folder(12);  // volume adjusted
    }
  }
  
  index = 0;
}

void print_lightstatus(){
  if(status_light_l1){
    Serial.print("HBE");  //SEND STATUS PATAY KUSINA
  }
  else{
    Serial.print("HAE"); //SEND STATUS BUHAY KUSINA
  }

  if(status_light_l2){
    Serial.print("HDE");  //SEND STATUS PATAY l3
  }
  else{
    Serial.print("HCE");  //SEND STATUS BUHAY l3
  }

  if(status_light_l3){
    Serial.print("HGE");  //SEND STATUS PATAY l1
  }
  else{
    Serial.print("HFE");  //SEND STATUS BUHAY l1
  }

  if(status_light_l4){
    Serial.print("HJE");  //SEND STATUS PATAY LABAS
  }
  else{
    Serial.print("HIE");  //SEND STATUS BUHAY LABAS
  }

  Serial.print("HOLE");
}
