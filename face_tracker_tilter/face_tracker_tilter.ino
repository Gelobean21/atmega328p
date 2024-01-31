#include <Servo.h>

#define SERVO_X 6
#define SERVO_Y 5

int x_min = 0;
int x_max = 180;
int y_min = 39;
int y_max = 150;
int x_start = 90;
int y_start = y_min;
int x = x_start;
int y = y_start;

Servo servo_x;
Servo servo_y;
byte x_incr = 2;
byte y_incr = 2;

void setup()
{
  Serial.setTimeout(30);
  Serial.begin(115200);
  servo_x.attach(SERVO_X);
  servo_y.attach(SERVO_Y);
  servo_x.write(x_start);
  servo_y.write(y_start);
}

bool isXinMin()
{
  bool retval = false;
  
  if(x == x_min)
  {
    retval = true;
  }

  return retval;
}

bool isXinMax()
{
  bool retval = false;
  
  if(x == x_max)
  {
    retval = true;
  }

  return retval;
}

bool isYinMin()
{
  bool retval = false;
  
  if(y == y_min)
  {
    retval = true;
  }

  return retval;
}

bool isYinMax()
{
  bool retval = false;
  
  if(y == y_max)
  {
    retval = true;
  }

  return retval;
}

void loop()
{
  while(Serial.available())
  {
    String cmd = Serial.readString();
    //Serial.println("received : " + cmd);
    
    if(cmd.startsWith("S:"))
    {
      cmd.remove(0,2);
      int temp = cmd.toInt();
      x_incr = temp;
      y_incr = temp;

      Serial.print("servo speed updated to : ");
      Serial.println(temp);
    }
    else if(cmd == "gRUs")
    {
        if(x < x_max  && y < y_max)
        {
          x = x + x_incr;
          y = y + y_incr;
          servo_x.write(x);
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isXinMax() && y < y_max)
        {
          y = y + y_incr;
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isYinMax() && x < x_max)
        {
          x = x + x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        
    } 
    else if(cmd == "gLDs")
    {
        if(x > x_min && y > y_min )
        {
          x = x - x_incr;
          y = y - y_incr;
          servo_x.write(x);
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isXinMin() && y > y_min)
        {
          y = y - y_incr;
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isYinMin() && x > x_min)
        {
          x = x - x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
    }
    else if(cmd == "gLUs")
    {
        if(x > x_min  && y < y_max)
        {
          x = x - x_incr;
          y = y + y_incr;
          servo_x.write(x);
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isXinMin() && y < y_max)
        {
          y = y + y_incr;
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isYinMax() && x > x_min)
        {
          x = x - x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
    }
    else if(cmd == "gRDs")
    {
       if(x < x_max && y > y_min)
       {
          x = x + x_incr; 
          y = y - y_incr;
          servo_x.write(x);
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
        else if(isXinMax() && y > y_min)
        {
          y = y - y_incr;
          servo_y.write(y);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.print("s");
        }
        else if(isYinMin() && x < x_max)
        {
          x = x + x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
        }
    }
    else if(cmd == "gRs")
    {
      if(x >= x_min && x < x_max)
      {
          x = x + x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
      }
    }
    else if(cmd == "gLs")
    {
      if(x > x_min && x <= x_max)
      {
          x = x - x_incr;
          servo_x.write(x);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
      }
    }
    else if(cmd == "gDs")
    {
      if(y > y_min && y <= y_max)
      {
          y = y - y_incr;
          servo_y.write(y);
          delay(5);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
      }
    }
    else if(cmd == "gUs")
    {
      if(y >= y_min && y < y_max)
      {
          y = y + y_incr;
          servo_y.write(y);
          delay(5);
          Serial.print("g");
          Serial.print(x);
          Serial.print(",");
          Serial.print(y);
          Serial.println("s");
      }
    }
  }
}
