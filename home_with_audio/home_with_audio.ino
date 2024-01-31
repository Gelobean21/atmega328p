#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <MyButton.h>
#include <DHT.h>

SoftwareSerial mySoftwareSerial(A1, A0); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

const byte maxbuff = 10;
const byte ender = '\n';
char request[maxbuff];
byte index = 0;

#define light_sala 13
#define light_kitchen 12
#define light_kwarto 11
#define light_outside 10
//#define door_status 9
//#define light_wifi A0;
//#define light_mqtt A1;

MyButton tactile_sala(5,0,1,120); //pin 5 , ACTIVE LOW , INPUT PULLUP , 120ms
MyButton tactile_kitchen(6,0,1,120);
MyButton tactile_kwarto(7,0,1,120);
MyButton tactile_outside(8,0,1,120);

bool mqtt_switch_sala = 0;
bool mqtt_switch_kitchen = 0;
bool mqtt_switch_kwarto = 0;
bool mqtt_switch_outside = 0;

bool status_light_sala = 0;
bool status_light_kitchen = 0;
bool status_light_kwarto = 0;
bool status_light_outside = 0;

//bool status_door = 0;

const unsigned int duration = 60000;
int startmillis = 0, currentmillis = 0;

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
mySoftwareSerial.begin(9600);
Serial.begin(115200);
Serial.println("ARDUINO IS ONLINE!");
pinMode(light_sala, OUTPUT);
pinMode(light_kitchen, OUTPUT);
pinMode(light_kwarto, OUTPUT);
pinMode(light_outside, OUTPUT);

//pinMode(door_status, INPUT_PULLUP);

digitalWrite(light_sala, LOW);
digitalWrite(light_kitchen, LOW);
digitalWrite(light_kwarto, LOW);
digitalWrite(light_outside, LOW);

dht.begin();
startmillis = millis();

if (!myDFPlayer.begin(mySoftwareSerial)) 
{
  /*while(true){
      delay(0); // HANG IF NOT FOUND
  }*/
}

myDFPlayer.setTimeOut(500);
myDFPlayer.volume(17);  //Set volume value. From 0 to 30
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

if(mqtt_switch_sala == 0 || tactile_sala.isClicked()){ //TURN ON
 if(status_light_sala == 1){
  digitalWrite(light_sala, LOW);
  Serial.print("HAE");
  myDFPlayer.playMp3Folder(2);                                     // KUSINA BUHAY.MP3
  status_light_sala = 0; mqtt_switch_sala = 0;
  delay(150);
 }
}

if(mqtt_switch_sala == 1 || tactile_sala.isClicked()){  //TURN OFF
 if(status_light_sala == 0){
  digitalWrite(light_sala, HIGH);
  Serial.print("HBE");
  myDFPlayer.playMp3Folder(3);                                     // KUSINA PATAY.MP3
  status_light_sala = 1; mqtt_switch_sala = 1;
  delay(150); 
 }
}

if(mqtt_switch_kitchen == 0 || tactile_kitchen.isClicked()){ //TURN ON
 if(status_light_kitchen == 1){
  digitalWrite(light_kitchen, LOW);
  Serial.print("HCE");                                    // KWARTO BUHAY.MP3
  myDFPlayer.playMp3Folder(4);
  status_light_kitchen = 0; mqtt_switch_kitchen = 0;
  delay(150);
 }
 
}

if(mqtt_switch_kitchen == 1 || tactile_kitchen.isClicked()){ //TURN OFF
 if(status_light_kitchen == 0){
  digitalWrite(light_kitchen, HIGH);
  Serial.print("HDE");                                   // KWARTO PATAY.MP3
  myDFPlayer.playMp3Folder(5);
  status_light_kitchen = 1; mqtt_switch_kitchen = 1;
  delay(150);
 }
}

if(mqtt_switch_kwarto == 0 || tactile_kwarto.isClicked()){  //TURN ON
 if(status_light_kwarto == 1){
  digitalWrite(light_kwarto, LOW);
  Serial.print("HFE");                                  // SALA BUHAY.MP3
  myDFPlayer.playMp3Folder(6);
  status_light_kwarto = 0; mqtt_switch_kwarto = 0;
  delay(150);
 }
}

if(mqtt_switch_kwarto == 1 || tactile_kwarto.isClicked()){  //TURN OFF
 if(status_light_kwarto == 0){
  digitalWrite(light_kwarto, HIGH);
  Serial.print("HGE");                                 // SALA PATAY.MP3
  myDFPlayer.playMp3Folder(7);
  status_light_kwarto = 1; mqtt_switch_kwarto = 1;
  delay(150);
 }
}

if(mqtt_switch_outside == 0 || tactile_outside.isClicked()){  //TURN ON
 if(status_light_outside == 1){
  digitalWrite(light_outside, LOW);
  Serial.print("HIE");                                  // LABAS BUHAY.MP3!!!
  myDFPlayer.playMp3Folder(8);
  status_light_outside = 0; mqtt_switch_outside = 0;
  delay(150);
 }
}

if(mqtt_switch_outside == 1 || tactile_outside.isClicked()){ //TURN OFF
 if(status_light_outside == 0){
  digitalWrite(light_outside, HIGH);
  Serial.print("HJE");
  myDFPlayer.playMp3Folder(9);                                     // LABAS PATAY.MP3!!!
  status_light_outside = 1; mqtt_switch_outside = 1;
  delay(150);
 }
}
}

void searchIsOver(){
  request[index++] = '\0';
  
  String search = String(request);
  if(search == "PERS"){                         // KUSINA PATAY!!!
    mqtt_switch_sala = 1;
  }
  if(search == "PERSO"){
    mqtt_switch_sala = 0;
  }
  if(search == "SEKUN"){                       // KWARTO PATAY!!!
    mqtt_switch_kitchen = 1;
  }
  if(search == "SEKUNO"){
    mqtt_switch_kitchen = 0;
  }
  if(search == "TIRD"){                       // SALA PATAY!!!
    mqtt_switch_kwarto = 1;
  }
  if(search == "TIRDO"){
    mqtt_switch_kwarto = 0;
  }
  if(search == "PORT"){                       // LABAS PATAY!!! 
    mqtt_switch_outside = 1;
  }
  if(search == "PORTO"){
    mqtt_switch_outside = 0;
  }
  if(search == "ISUCC"){
    //digitalWrite(light_wifi, HIGH);
  }
  if(search == "IERR"){
    //digitalWrite(light_wifi, LOW);
  }
  if(search == "MSUCC"){
    //digitalWrite(light_mqtt, HIGH);
  }
  if(search == "MERR"){
    //digitalWrite(light_mqtt, LOW);
  }
  if(search == "HS"){
    Serial.print("HOLE");
  }
  if(search == "LS"){
    print_lightstatus();
  }
  if(search == "SP")
  {
    myDFPlayer.playMp3Folder(13);
  }
  if(search.startsWith("SV"))
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
  if(status_light_sala){
    Serial.print("HBE");  //SEND STATUS PATAY KUSINA
  }
  else{
    Serial.print("HAE"); //SEND STATUS BUHAY KUSINA
  }

  if(status_light_kitchen){
    Serial.print("HDE");  //SEND STATUS PATAY KWARTO
  }
  else{
    Serial.print("HCE");  //SEND STATUS BUHAY KWARTO
  }

  if(status_light_kwarto){
    Serial.print("HGE");  //SEND STATUS PATAY SALA
  }
  else{
    Serial.print("HFE");  //SEND STATUS BUHAY SALA
  }

  if(status_light_outside){
    Serial.print("HJE");  //SEND STATUS PATAY LABAS
  }
  else{
    Serial.print("HIE");  //SEND STATUS BUHAY LABAS
  }
}
