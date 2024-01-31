#define clock 7
#define latch 8
#define input 13

byte perseg = 6;
byte secseg = 12;
byte thirseg = 5;

int output[] = {63,6,91,79,102,109,125,7,127,103};

void setup()
{
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(input, OUTPUT);
  pinMode(perseg, OUTPUT);
  pinMode(secseg, OUTPUT);
  pinMode(thirseg, OUTPUT);
  digitalWrite(secseg, LOW);
  digitalWrite(thirseg, LOW);
  digitalWrite(perseg, HIGH);
}

void loop()
{
  for(int led = 0 ; led < 10 ; led++){
    ShitOut(input, clock, latch, output[led]);
  }
}

void ShitOut(int in, int cl, int lh,int pinum){
  digitalWrite(lh, LOW);
  for(int i = 0; i < 8 ; i++){ //8 clock bits
    digitalWrite(in, !!(pinum & (1 << (7 - i))));
    digitalWrite(cl, LOW);
    digitalWrite(cl, HIGH);
  }
  digitalWrite(lh, HIGH);
  delay(500);
}
