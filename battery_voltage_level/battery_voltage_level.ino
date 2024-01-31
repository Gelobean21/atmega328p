#define BATTERY_INPUT A0

void setup() {
Serial.begin(115200);
}

void loop() {
printVolts();
delay(500);
}

void printVolts()
{
  int sensorValue = analogRead(BATTERY_INPUT); //read the A0 pin value
  float voltage = sensorValue * (5.00 / 1023.00) * 2; //convert the value to a true voltage.
  Serial.println(voltage);
}
