#include <Keypad.h>

byte clock = 7;
byte latch = 8;
byte input = 13;

byte perseg = 5;
byte secseg = 12;
byte thirseg = 6;

byte delayTime = 5;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {2,3,4,A5};
byte colPins[COLS] = {A4,A3,A2};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int output[] = {63,6,91,79,102,109,125,7,127,103};

void setup(){
  Serial.begin(9600);
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(input, OUTPUT);
  pinMode(perseg, OUTPUT);
  pinMode(secseg, OUTPUT);
  pinMode(thirseg, OUTPUT);
  
  digitalWrite(perseg, LOW);
  digitalWrite(secseg, LOW);
  digitalWrite(thirseg, LOW);
}

int segment_count = 0;
int pers = 0;
int sec = 0;
int thir = 0;
byte b_input = 0;

void loop(){
  while(Serial.available())
  {
    b_input = Serial.read();
  }
  
  char key = keypad.getKey();
  
  if(key != NO_KEY){
   if(segment_count == 2 && key != '*'){
    segment_count = 3;
    thir = key-'0';
   }
   if(segment_count == 1 && key != '*'){
    segment_count = 2;
    sec = key-'0';
   }
   if(segment_count == 0 && key != '*'){
    segment_count = 1;
    pers = key-'0';
   }
  }
  
  if(b_input > 47 && b_input < 58)
   {
    if(segment_count == 2 && b_input != '*')
    {
      segment_count = 3;
      thir = b_input-'0';
    }
    if(segment_count == 1 && b_input != '*')
    {
      segment_count = 2;
      sec = b_input-'0';
    }
    if(segment_count == 0 && b_input != '*')
    {
      segment_count = 1;
      pers = b_input-'0';
    }
   }
  
  if(key == '#' || b_input == '#'){
    digitalWrite(perseg,LOW);
    digitalWrite(secseg,LOW);
    digitalWrite(thirseg,LOW);
    pers = 0;
    sec = 0;
    thir = 0;
    segment_count = 0;
   }
  
  if(key== '*' || b_input == '*'){
    if(segment_count == 1){
     digitalWrite(perseg,LOW);
     pers = 0;
     segment_count = 0;
    }
    if(segment_count == 2){
     digitalWrite(secseg,LOW);
     sec = 0;
     segment_count = 1;
    }
    if(segment_count == 3){
     digitalWrite(thirseg,LOW);
     thir = 0;
     segment_count = 2;
    }
   }
  
  if(segment_count == 3){
    WriteNumber(input, clock, latch, output[pers]);
    digitalWrite(perseg,HIGH);
    digitalWrite(secseg,LOW);
    digitalWrite(thirseg,LOW);
    delay(delayTime);
    WriteNumber(input, clock, latch, output[sec]);
    digitalWrite(perseg,LOW);
    digitalWrite(secseg,HIGH);
    digitalWrite(thirseg,LOW);
    delay(delayTime);
    WriteNumber(input, clock, latch, output[thir]);
    digitalWrite(perseg,LOW);
    digitalWrite(secseg,LOW);
    digitalWrite(thirseg,HIGH);
    delay(delayTime);
  }
  if(segment_count == 2){
    WriteNumber(input, clock, latch, output[pers]);
    digitalWrite(perseg,HIGH);
    digitalWrite(secseg,LOW);
    digitalWrite(thirseg,LOW);
    delay(delayTime);
    WriteNumber(input, clock, latch, output[sec]);
    digitalWrite(perseg,LOW);
    digitalWrite(secseg,HIGH);
    digitalWrite(thirseg,LOW);
    delay(delayTime);
  }
  if(segment_count == 1){
    WriteNumber(input, clock, latch, output[pers]);
    digitalWrite(perseg,HIGH);
    digitalWrite(secseg,LOW);
    digitalWrite(thirseg,LOW);
  }
  
  b_input = 0;
}

void WriteNumber(int in, int cl, int lh,int pinum){
  digitalWrite(lh, LOW);
  for(int i = 0; i < 8 ; i++){ //8 clock bits
    digitalWrite(in, !!(pinum & (1 << (7 - i))));
    digitalWrite(cl, LOW);
    digitalWrite(cl, HIGH);
  }
  digitalWrite(lh, HIGH);
}
