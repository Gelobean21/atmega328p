#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>

#define RADIO_CE 8 // LUMA 7 
#define RADIO_CSN 7 // LUMA 8

RF24 radio(RADIO_CE, RADIO_CSN);
RF24Audio rfAudio(radio,0);	

void setup() {
  rfAudio.begin();
}

void loop() {
  rfAudio.handleButtons();
}