/* ---------------------------------------------------------
Package     :  (·K) Leonardo v.1 (R)
Module      :  radio
Copyright   :  (c) 2014 by Antonio Curci 

Copyright 2014 by Antonio Curci. All rights reserved.
Redistribution and use in source and binary forms,
with or without modification, are not permitted.

-------------------------------------------------------- */

// ******************** LIBRARIES
#include "printf.h"  // serve per la stampa iniziale di radio.printDetails();
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// ******************** DEFINES
#define PORT_TX  0xDEDEDEDEE7LL
#define PORT_RX  0xDEDEDEDEE9LL

// ******************** CONSTANTS
// PIN
const byte PIN_RAD_CE  = 8, PIN_RAD_CSN = 10;  // Arduino UNO
// const byte PIN_RAD_CE  = 46, PIN_RAD_CSN = 47;  // Arduino MEGA
  
// ******************** VARIABLES
RF24 radio(PIN_RAD_CE,PIN_RAD_CSN);    // RF24 radio(CE,CSN);

// ******************** FUNCTIONS
void radsetup() {
  printf_begin();
  radio.begin();
  
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  
  radio.enableDynamicPayloads();
  radio.setRetries(15,15);
  radio.setCRCLength(RF24_CRC_16);

  radio.openWritingPipe(PORT_TX);
  radio.openReadingPipe(1,PORT_RX);  
  
  radio.startListening();
  radio.printDetails();
}

boolean radav()     {return radio.available();}
byte radinlen()     {return radio.getDynamicPayloadSize();}
boolean radin(void* p, int n)    {return radio.read(p,n);}

boolean radout(void* p, int n) {
  // swap TX & Rx addr for writing
  radio.openWritingPipe(PORT_RX);
  radio.openReadingPipe(0,PORT_TX);  
  radio.stopListening();
        
// bool ok = radio.write(&SendPayload,strlen(SendPayload));
  bool ok = radio.write(p,n);
        
  // restore TX & Rx addr for reading       
  radio.openWritingPipe(PORT_TX);
  radio.openReadingPipe(1,PORT_RX); 
  radio.startListening();  

  return ok;
}


