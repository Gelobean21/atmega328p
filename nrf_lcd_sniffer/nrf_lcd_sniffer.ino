#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>
#include <AxisJoystick.h>
#include <MyButton.h>

#define JOYSTICK_SW A2
#define JOYSTICK_VRX A0
#define JOYSTICK_VRY A1

#define LEFT_BUTTON A3
#define RIGHT_BUTTON A4

#define LCD_RS 7
#define LCD_EN 6
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define RADIO_CE 10
#define RADIO_CSN 9

Joystick* joystic;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
RF24 radio(RADIO_CE, RADIO_CSN);
MyButton btnLeft(LEFT_BUTTON,0,1,120);
MyButton btnRight(RIGHT_BUTTON,0,1,120);

byte radioAddress[6] = "00000";
const byte address[6] = "00001";
byte radIndex = 0;
bool radMode = 0; // 0 - rx , 1 - tx
String radModeName[] = {"RECEIVER", "SENDER"};
bool namingMode = 0;
bool blink = 0; // 0 - no display, 1 - has char

String MenuItems[] = {  // Your menu items
  "Serial",
  "Radio",
  "menuitem 3",
  "menuitem 4",
  "menuitem 5",
  "menuitem 6" 
  // and so on... 
};

long baudRate[] = {0,115200, 9600, 57600, 38400, 31250, 28800, 19200, 14400, 4800, 2400, 1200, 600, 300};
long selectedBaud = 0;
byte menuId = 0;
byte jobId = 0;

template< typename T, size_t NumberOfSize > 
size_t MenuItemsSize(T (&) [NumberOfSize]){ return NumberOfSize; }
byte numberOfMenuItems = MenuItemsSize(MenuItems) - 1;
byte currentMenuItem = 0;
byte previousMenuItem = 1;
byte button_flag = 0;
unsigned long previousMillis = millis();
unsigned long blinkMillis;
byte fIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  lcd.begin(16, 2);
  joystic = new AxisJoystick(JOYSTICK_SW, JOYSTICK_VRX, JOYSTICK_VRY);
  lcd.clear();
  lcd.print("WELCOME!");
  delay(700);

  //radio.setDataRate(RF24_250KBPS);

  /*radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(0, address);
  radio.startListening();*/
}

void loop() 
{
  if(jobId != 0)
  {
    if(jobId == 1)
    {
      while(Serial.available())
      {
        lcd.write(Serial.read());
      }
    }
    if(jobId == 2)
    {
      if (radio.available())
      {
        char text[32] = {0};
        radio.read(&text, sizeof(text));
        lcd.write(text);
      }
    }
    if(jobId == 3)
    {

    }
  }
  if(namingMode)
  {
    unsigned long currentMillis = millis();
    
    if(currentMillis - blinkMillis >= 300)
    {
      blink = !blink;
      blinkMillis = currentMillis;
      lcd.setCursor(radIndex, 1);
      
      if(blink)
      {
        lcd.print((char)radioAddress[radIndex]);
      }
      else
      {
        lcd.print(" ");
      }
    }
  }
  if(joystic->isLeft() && button_flag == 0)
  {
    menuFunctions(currentMenuItem + 1, 1, 0);
    button_flag = 1;
    previousMillis = millis();
  }
  if(joystic->isRight() && button_flag == 0)
  {
    menuFunctions(currentMenuItem + 1, 0, 1);
    button_flag = 1;
    previousMillis = millis();
  }
  if(joystic->isUp() && button_flag == 0)
  {
    if(!namingMode)
    {
      lcd.clear();
      lcd.noAutoscroll();
      jobId = 0;

      if(currentMenuItem == numberOfMenuItems )
      {
        currentMenuItem = 0;
      }
      else
      {
        currentMenuItem++;
      }
    }
    else 
    {
      byte temp = (int)radioAddress[radIndex] - 1;
      radioAddress[radIndex] = temp;
      
      lcd.clear();
      lcd.print("SET ADDRESS");
      lcd.setCursor(0,1);
      for(int i = 0 ; i < 6 ; i++)
      {
      	lcd.print((char)radioAddress[i]);
      }
    }

    button_flag = 1;
    previousMillis = millis();
  }
  if(joystic->isDown() && button_flag == 0)
  {
    if(!namingMode)
    {
      lcd.clear();
      lcd.noAutoscroll();
      jobId = 0;

      if(currentMenuItem == 0)
      {
        currentMenuItem = numberOfMenuItems;
      }
      else
      {
        currentMenuItem--;
      }
    }
    else
    {
      byte temp = (int)radioAddress[radIndex] + 1;
      radioAddress[radIndex] = temp;
      
      lcd.clear();
      lcd.print("SET ADDRESS");
      lcd.setCursor(0,1);
      for(int i = 0 ; i < 6 ; i++)
      {
      	lcd.print((char)radioAddress[i]);
      }
    }

    button_flag = 1;
    previousMillis = millis();
  }
  if(joystic->isPress() && button_flag == 0)
  {
    if(menuId == 1)
    {
      if(selectedBaud != 0)
      {
      	Serial.end();
        Serial.begin(selectedBaud);
        lcd.clear();
        lcd.print("SERIAL READ MODE");
	 	    lcd.setCursor(0,1);
		    lcd.print("BAUD : ");
		    lcd.print(selectedBaud);
        lcd.autoscroll();
        jobId = 1;
      }
    }
    if(menuId == 2)
    {
      if(!namingMode)
      {
        namingMode = 1;
        lcd.clear();
        lcd.print("SET ADDRESS");
      }
      else
      {
        namingMode = 0;
        menuId = 0;
        lcd.clear();
        lcd.print(radModeName[radMode]);
        
        if(radio.begin())
        {
          if(!radMode) // read
          {
            radio.begin();
            radio.setPALevel(RF24_PA_LOW);
            radio.openReadingPipe(0, radioAddress);
            radio.startListening();
            lcd.setCursor(0,1);
            lcd.print("RADIO OK!");
            lcd.autoscroll();
            delay(500);
            jobId = 2;
          }
          else // write
          {
            jobId = 3;
          }
        }      
        else 
        {
          lcd.print("RADIO ERR!");
        }
      }
    }

    lcd.setCursor(0, 1);
    for(int i = 0 ; i < 6 ; i ++)
    {
      lcd.print((char)radioAddress[i]);
    }

    button_flag = 1;
    previousMillis = millis();
  }
  if(currentMenuItem != previousMenuItem)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menu Item:");
    lcd.setCursor(0, 1);
    lcd.print(MenuItems [currentMenuItem]);
    menuFunctions(currentMenuItem + 1, 0, 0);
    previousMenuItem = currentMenuItem;
  }
  if(millis() - previousMillis >= 400) 
  {
    previousMillis = millis();
    button_flag = 0;
  }
}

void menuFunctions(int menu, byte right, byte left)  // Your menu functions
{
  if(menu == 1) // example function for 1. menu item
  {
    if(right == 1)
    {
      int temp = (sizeof(baudRate)/sizeof(baudRate[0])) - 1;

      if(temp > fIndex)
      {
        fIndex++;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(baudRate[fIndex]);
        selectedBaud = baudRate[fIndex];
      }
      else
      {
        fIndex = 1;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(baudRate[fIndex]);
        selectedBaud = baudRate[fIndex];
      }
    }
    if(left == 1)
    {
      int temp = (sizeof(baudRate)/sizeof(baudRate[0])) - 1;

      if(fIndex == 1)
      {
        fIndex = temp;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(baudRate[fIndex]);
        selectedBaud = baudRate[fIndex];
      }
      else
      {
        fIndex--;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(baudRate[fIndex]);
        selectedBaud = baudRate[fIndex];
      }
    }

    menuId = 1;
  }
  if(menu == 2)
  {
    if(!namingMode)
    {
      if(right == 1 || left == 1)
    	{
        if(radio.begin())
        {
      		radMode = !radMode;
      		lcd.clear();
      		lcd.print("SELECT RAD. MODE");
      		lcd.setCursor(0,1);
      		lcd.print(radModeName[radMode]);
      		menuId = 2;
        }
        else
        {
          lcd.clear();
          lcd.print("RADIO ERR!");
          delay(700);
          menuId = 0;
        }
    	}
    }
    else
    {
      lcd.setCursor(radIndex, 1);
      lcd.print((char)radioAddress[radIndex]);
      if(right == 1)
      {
        if(radIndex == 4)
        {
          radIndex = 0;
        }
        else
        {
          radIndex++;
        }
      }
      if(left == 1)
      {
        if(radIndex == 0)
        {
          radIndex = 4;
        }
        else
        {
          radIndex--;
        }
      }
    }
  }
}
