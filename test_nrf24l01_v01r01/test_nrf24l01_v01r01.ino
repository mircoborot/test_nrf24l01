/* ---------------------------------------------------------
Package     :  (·K) Leonardo v.1 (R)
Module      :  radio
Copyright   :  (c) 2014 by Antonio Curci 

Copyright 2014 by Antonio Curci. All rights reserved.
Redistribution and use in source and binary forms,
with or without modification, are not permitted.

History     :  
  Version  Description
    01      original
    02      senza lcd
    03      astrazione

-------------------------------------------------------- */
#define DEBUG_PREF    "\r\n  ###"

#define serout(x)    Serial.print(x)

boolean stringOverflow = false;
char charOverflow = 0;
boolean stringComplete = false;  // whether the string is complete
static int dataBufferIndex = 0;

char serialBuffer[31] = "";
char RecvPayload[31] = "";
char SendPayload[31] = "";

void setup(void) {
 
  Serial.begin(9600);

  radsetup();

  Serial.println();
  Serial.println("RF Chat V0.90");    
  delay(500);
}

void loop(void) {radio_receive();serial_receive();} 

void serialEvent() {
  while (Serial.available() > 0 ) {
      char incomingByte = Serial.read();
      
      if (stringOverflow) {
         serialBuffer[dataBufferIndex++] = charOverflow;  // Place saved overflow byte into buffer
         serialBuffer[dataBufferIndex++] = incomingByte;  // saved next byte into next buffer
         stringOverflow = false;                          // turn overflow flag off
      } else if (dataBufferIndex > 31) {
         stringComplete = true;        // Send this buffer out to radio
         stringOverflow = true;        // trigger the overflow flag
         charOverflow = incomingByte;  // Saved the overflow byte for next loop
         dataBufferIndex = 0;          // reset the bufferindex
         break; 
      } 
      else if(incomingByte=='\n' || incomingByte=='\r'){
          serialBuffer[dataBufferIndex] = 0; 
          stringComplete = true;
      } else {
          serialBuffer[dataBufferIndex++] = incomingByte;
          serialBuffer[dataBufferIndex] = 0; 
      }          
  } 
} 

void radio_receive(void) {
  int len = 0;
  bool done = false;
  
  if (radav()) {  
      while (!done) {
        len = radinlen();
        done = radin(&RecvPayload,len);
        delay(5);
      }
  
    RecvPayload[len] = 0; // null terminate string
    
    strout(" radin",RecvPayload,len);

    Serial.print("\r\nR:");
    Serial.print(RecvPayload);
    Serial.println();
    RecvPayload[0] = 0;  // Clear the buffers
  }  

}

void serial_receive(void){
  
  if (stringComplete) { 
        strcat(SendPayload,serialBuffer);      

        radout(&SendPayload,strlen(SendPayload));
        
        strout("radout",SendPayload,strlen(SendPayload));
        
        Serial.print("\r\nS:");
        Serial.print(SendPayload);          
        Serial.println();
        stringComplete = false;

        SendPayload[0] = 0;
        dataBufferIndex = 0;
  } 
} 


    
    
void strout(char *name,char *p,byte n) {
  serout(DEBUG_PREF);serout(' ');
  serout(name);
  serout('[');serout(n);serout("]=<");
  for(byte i=0;i<n;i++) {echoHex(*(p+i));serout(' ');}
  serout(">");
}

char dec2Hex(char x)  {if (0<=x&&x<=9) return '0'+x; else return 'A'+x-10;}
void echoHex(byte x)  {serout(dec2Hex(x/16));serout(dec2Hex(x%16));} 
void echoLineHex(char *p) {while (*p) {echoHex(*p);p++;serout(' ');}}
