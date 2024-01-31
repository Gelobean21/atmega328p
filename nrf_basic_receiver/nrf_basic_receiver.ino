//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(2,4);  // CE, CSN

int rotVal = 0;

//address through which two modules communicate.
const byte address[6] = "00001";

void setup()
{
  while (!Serial);
    Serial.begin(9600);
  
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  
  //Set module as receiver
  radio.startListening();
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    Serial.println(text);
    String strmsg = String(text);

    if(strmsg.startsWith("*") && strmsg.endsWith("#"))
    {
       rotVal = strmsg.substring(1,strmsg.length() - 1).toInt();
    }
  }
}
