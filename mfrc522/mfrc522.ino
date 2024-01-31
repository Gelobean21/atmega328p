#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#define SS_PIN 10  //slave select pin   // SDA IN MODULE
#define RST_PIN 9  //reset pin

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//63, 59, 55, 51, 47, 43, 39, 35, 31, 27, 23, 19, 15, 11, 7, 3// trailer
//maybe broken // 55 and 51
int block = 46;
int opValue = 0;
byte option = 1;  // 1 - read  / 2 - write / 3 - read all / 4 - increment / 5 - decrement
byte parseOption = 1; // 1 - char / 2 - hex / 3 - byte
byte parseBOption = 1; // 1 - char / 2 - hex / 3 - byte
byte vbA = 1; //1 - char / 2 - hex / 3 - byte
byte readOption = 1; // 1 - char / 2 - hex / 3 - byte / 4 - value
bool isAuthKeyA = true;
bool isValueBlock = false;
byte readbackblock[18];
byte blocks[16] = {};
int i = 0;
int j = 0;
MFRC522::StatusCode status;
int32_t value;

void setup() 
{
    Serial.begin(9600);        
    SPI.begin();               
    mfrc522.PCD_Init();    
    //key = { 0x41,0x6E,0x67,0x65,0x6C,0x6F }; //Angelo      //nak with key A and A at bottom
    key = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; //Transport 
    //key = { 0x47,0x61,0x73,0x70,0x61,0x72 }; //Gaspar       //nak with key B and B at bottom
}

String readString = "";

void loop()
{
    while(Serial.available())
    {
        char incoming = Serial.read();
        readString += incoming;
        delay(2);
    }
  
    if(readString != "")
    {
        if(readString.startsWith("BLOCK : "))
        {
            readString.remove(0,8);
            block = readString.toInt();
            Serial.print("SET BLOCK TO : ");
            Serial.print(block);
            Serial.print("\r\n");
        }
        else if(readString.startsWith("VBA : "))
        {
            readString.remove(0,6);

            switch(readString[0])
            {
              case '1' : //char
                vbA = 1;
                Serial.print("VIEW BITS AS : CHAR");
                Serial.print("\r\n");
              break;
              case '2' :
                vbA = 2;
                Serial.print("VIEW BITS AS : HEX");
                Serial.print("\r\n");
              break;
              case '3' :
                vbA = 3;
                Serial.print("VIEW BITS AS : BYTE");
                Serial.print("\r\n");
              break;
              default :
              break;
            }
        }
        else if(readString == "VB") //VIEW BITS
        {
          i = 0;
           
          switch(vbA)
          {
            case 1 :  //char
              for(i = 0 ; i < 16 ; i++)
              {
                Serial.print((char)blocks[i]);
              }
            break;
            case 2 : //hex
              
            break;
            case 3 : //byte
              for(i = 0 ; i < 16 ; i++)
              {
                Serial.print(blocks[i]);
              }
            break;
            default :
            break;
          }
        }
        else if(readString.startsWith("READ OPTION : "))
        {
            readString.remove(0,14);

             switch(readString[0])
             {
                case '1' : // char
                  readOption = 1;
                  Serial.print("SET READ OPTION TO CHAR");
                  break;
                case '2' : // hex
                  readOption = 2;
                  Serial.print("SET READ OPTION TO HEX");
                  break;
                case '3' : // byte
                  readOption = 3;
                  Serial.print("SET READ OPTION TO BYTE");
                  break;
                case '4' : // value
                  readOption = 4;
                  Serial.print("SET READ OPTION TO VALUE");
                  break;
                default : break;
             }
        }
        else if(readString.startsWith("PARSE OPTION : "))
        {
            readString.remove(0, 15);
      
            switch(readString[0])
            {
                case '1' : // char
                    parseOption = 1;
                    Serial.print("SET PARSE OPTION TO CHAR");
                    break;
        
                case '2' : // hex
                    parseOption = 2;
                    Serial.print("SET PARSE OPTION TO HEX");
                    break;
            
                case '3' : // byte
                    parseOption = 3;
                    Serial.print("SET PARSE OPTION TO BYTE");
                    break;
        
            default: break;
            }
        }
        else if(readString.startsWith("PARSE BLOCK OPTION : "))
        {
          readString.remove(0, 21);

          switch(readString[0])
          {
                case '1' : // char
                    parseBOption = 1;
                    Serial.print("BLOCKS WILL BE READ AS CHAR\r\n");
                    break;
        
                case '2' : // hex
                    parseBOption = 2;
                    Serial.print("BLOCKS WILL BE READ AS HEX\r\n");
                    break;
            
                case '3' : // byte
                    parseBOption = 3;
                    Serial.print("BLOCKS WILL BE READ AS BYTE\r\n");
                    break;
        
            default: break;
            }
          
        }
        else if(readString == "CURRENT KEY")
        {
            int i = 0;

            for(i = 0 ; i < 6; i++)
            {
                Serial.print(key.keyByte[i]);
            }
            Serial.println();
        }
        else if(readString.startsWith("OP VALUE : "))
        {
            readString.remove(0, 11);
            opValue = readString.toInt();
            Serial.print("SET OPERATION VALUE TO : ");
            Serial.print(opValue);
            Serial.print("\r\n");
        }
        else if(readString.startsWith("AUTH KEY : "))
        {
            readString.remove(0, 11);
            if(readString == "A")
            {
                isAuthKeyA = true;
                Serial.print("Set Auth Key to A");
                Serial.print("\r\n");
            }
            else if(readString == "B")
            {
                isAuthKeyA = false;
                Serial.print("Set Auth Key to B");
                Serial.print("\r\n");
            }
        }
        else if(readString.startsWith("KEY : "))
        {
            readString.remove(0, 6);
      
            i = 0;
          
            for(i = 0; i < 6; i++)
            {
                int indexOfKey = 0;
                indexOfKey = readString.indexOf(",");
                String intVal = "";
                char hexVal[indexOfKey] = "";

              
                switch(parseOption)
                {
                    case 1 : // char
                    key.keyByte[i] = readString[indexOfKey - 1]; break;
            
                    case 2 : // hex
                    j = 0;
                    for(j = 0 ; j < indexOfKey; j++)
                    {
                        hexVal[j] = hexVal[j] + readString[j];
                    }
                    key.keyByte[i] = hexStringToInt(hexVal); 
                    break;
            
                    case 3 : // int
                    intVal = "";
                    j = 0;
                    for(j = 0 ; j < indexOfKey; j++)
                    {
                        intVal = intVal + readString[j];
                    }
                    key.keyByte[i] = intVal.toInt(); break;
                    
                    default : break;
                }

                if(i == 5)
                {
                    char hixVal[readString.length()] = "";
                    switch(parseOption)
                    {
                        case 1 : // char
                            key.keyByte[i] = readString[0]; break;
            
                        case 2 : // hex
                            j = 0;
                            for(j = 0 ; j < readString.length(); j++)
                            {
                                hixVal[j] = hixVal[j] + readString[j];
                            }
                            key.keyByte[i] = hexStringToInt(hixVal); break;
            
                        case 3 : // int
                            intVal = "";
                            j = 0;
                            for(j = 0 ; j < readString.length() + 1; j++)
                            {
                                intVal = intVal + readString[j];
                            }
                            key.keyByte[i] = intVal.toInt(); break;
              
                         default : break;
                    }
                }
                else
                {
                    switch(parseOption)
                    {
                        case 1 : // char
                        readString.remove(0,2); break;
            
                        case 2 : // hex
                        readString.remove(0,indexOfKey + 1); break;
            
                        case 3 : // int
                        readString.remove(0,indexOfKey + 1); break;
                        break;
                
                        default : break;
                    }
                }
            }
            
            Serial.print("\r\n");
            Serial.print("Set Auth key to : ");
      
            i = 0;
      
            for(i = 0 ; i < 6; i++)
            {
                Serial.print((char)key.keyByte[i]);
            }
            Serial.print("\r\n");
        }
        else if(readString.startsWith("CONTENT : "))
        {
            Serial.print("BLOCKS TO WRITE : ");
            readString.remove(0, 10);
            i = 0;
            int indexOfComma = 0;

                for(i = 0 ; i < 16 ; i++)
                {
                    if(i != 15)
                    {
                        indexOfComma = readString.indexOf(","); // 2 in 65,
                        blocks[i] = readString.substring(0, indexOfComma).toInt();
                        Serial.print(blocks[i]);
                        readString.remove(0, indexOfComma + 1);
                        Serial.print(",");
                    }
                    else
                    {
                       blocks[i] = readString.toInt();
                       Serial.print(blocks[i]);                      
                    }
                }
        }
        else if(readString.startsWith("HELLO MIFARE"))
        {
            Serial.print("OK\r\n");
        }
        else if(readString.startsWith("OPTION : "))
        {
            readString.remove(0, 9);
            char input = readString[0];
            switch(input)
            {
                case '1' : 
                  option = 1;
                  Serial.print("SET TO READ\r\n"); break;
          
                case '2' :
                  option = 2;
                  Serial.print("SET TO WRITE\r\n"); break;
          
                case '3':
                  option = 3;
                  Serial.print("SET TO READ ALL\r\n"); break;
          
                case '4' :
                  option = 4;
                  Serial.print("SET TO INCREMENT\r\n"); break;
          
                case '5' :
                  option = 5;
                  Serial.print("SET TO DECREMENT\r\n"); break;
        
                default : break;
            }
        }
        else if(readString == "SETBLOCKAS : VB")
        {
            isValueBlock = true;
            Serial.print("SET WRITE AS VALUE BLOCK");
        }
        else if(readString == "SETBLOCKAS : DB")
        {
            isValueBlock = false;
            Serial.print("SET WRITE AS DATA BLOCK");
        }
  
        readString = "";
    }
  
  
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() )
    {
        delay(500);
        return;
    }
   
    switch(option)
    {
      case 1 : 
        readBlock(readbackblock);
        Serial.print("BLOCK CONTENT : ");
        
        i = 0;

        if(readOption == 1) //char
        {
          for (i = 0 ; i < 16 ; i++)
          {
             Serial.print((char)readbackblock[i]);
          }
        }
        else if(readOption == 2) //hex
        {
          for (i = 0 ; i < 16 ; i++)
          {
             if(i < 15){
                Serial.print(readbackblock[i], HEX);   
                Serial.print(",");
             }
             else{
                Serial.print(readbackblock[i], HEX);   
             }
          }
        }
        else if(readOption == 3) //byte
        {
          for (i = 0 ; i < 16 ; i++)
          {
             if(i < 15){
                Serial.print(readbackblock[i]);   
                Serial.print(",");
             }
             else{
                Serial.print(readbackblock[i]);   
             }
          }
        }
        else if(readOption == 4) // value
        {
          readValueBlock();
        }
        
        Serial.print("\r\n"); break;
    
      case 2 : 
        writeBlock(blocks); break;
    
      case 3 : 
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); break;
    
      case 4 : //increment
        increment();
        break;
      
      case 5 : //decrement
        decrement();
        break;
    
      default : break;
    }
   
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}


int writeBlock(byte arrayAddress[]) 
{
    int largestModulo4Number=block/4*4;
    int trailerBlock=largestModulo4Number+3;
    //if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}
    
    if(isAuthKeyA)
    {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else
    {
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    }
  
    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 3;
    }
  
    status = mfrc522.MIFARE_Write(block, arrayAddress, 16);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print("MIFARE_Write() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 4;
    }
    Serial.println("block was written");
}


int readBlock(byte arrayAddress[]) 
{
    int largestModulo4Number=block/4*4;
    int trailerBlock=largestModulo4Number+3;
    
    if(isAuthKeyA)
    {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else
    {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    }
  
    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed (read): ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 3;
    }

    byte buffersize = 18;
    status = mfrc522.MIFARE_Read(block, arrayAddress, &buffersize);
    if (status != MFRC522::STATUS_OK) 
    {
        Serial.print("MIFARE_read() failed: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 4;
    }
}

void increment()
{
    int largestModulo4Number=block/4*4;
    int trailerBlock=largestModulo4Number+3;
  
    if(isAuthKeyA)
    {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else
    {
        status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    }
  
    if (status != MFRC522::STATUS_OK) {
        Serial.print("PCD_Authenticate() failed (read): ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return 3;
    }
    
    status = mfrc522.MIFARE_Increment(block, opValue);
    
    if (status != MFRC522::STATUS_OK) 
    {
      Serial.print(F("MIFARE_Increment() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
        
    status = mfrc522.MIFARE_Transfer(block);
    if (status != MFRC522::STATUS_OK) 
    {
      Serial.print(F("MIFARE_Transfer() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
        
    Serial.println("Incremented successfully");
}

void decrement()
{
    int largestModulo4Number=block/4*4;
    int trailerBlock=largestModulo4Number+3;
  
    if(isAuthKeyA)
    {
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else
    {
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    }
  
    if (status != MFRC522::STATUS_OK) 
    {
      Serial.print("PCD_Authenticate() failed (read): ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return 3;
    }
  
    status = mfrc522.MIFARE_Decrement(block, opValue);
    if (status != MFRC522::STATUS_OK) 
    { 
      Serial.print(F("MIFARE_Decrement() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    status = mfrc522.MIFARE_Transfer(block);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Transfer() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
        
    Serial.println("Decremented successfully");
}

void readValueBlock()
{
    int largestModulo4Number=block/4*4;
    int trailerBlock=largestModulo4Number+3;
  
    if(isAuthKeyA)
    {
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else
    {
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    }
  
    if (status != MFRC522::STATUS_OK) 
    {
      Serial.print("PCD_Authenticate() failed (read): ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return 3;
    }
  
   status = mfrc522.MIFARE_GetValue(block, &value);
   if (status != MFRC522::STATUS_OK) {
    Serial.print(F("mifare_GetValue() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
   }
   Serial.println(value);
}

int hexStringToInt(char str[])
{
  return (int)strtol(str, 0, 16);
}
