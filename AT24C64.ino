#include  <Wire.h>
#include  <AT24CX.h>

byte i2caddress = 0x50;
//Model     Pagesize
//AT24C32   32
//AT24C64   32
//AT24C128  64
//AT24C256  64
//AT24C512  128
// edit the pagesize based on the chipset you are using
byte pagesize = 32;

AT24CX mem(i2caddress, pagesize);
void setup() {
  Serial.begin(115200);
}
void loop() {
  int write_mem_address = 0; 
  char write_msg[] = "Flux Workshop"; 
  int write_msg_size = sizeof(write_msg); 
  
  mem.writeChars(write_mem_address, write_msg, write_msg_size);
  int read_mem_address = 0; 
  char read_msg[6]; 
  int read_msg_size = sizeof(read_msg); 
  
  mem.readChars(read_mem_address , read_msg, read_msg_size);
  
  Serial.print(read_msg);
  
  while (1==1) {}
}
