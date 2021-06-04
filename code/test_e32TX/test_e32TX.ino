// Transmitter code for Arduino x E32 LoRa
#include <SoftwareSerial.h>
#include "LoRa_E32.h"

#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

float ii = 0.0;

SoftwareSerial ESerial(2, 3);
LoRa_E32 Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {
  ESerial.begin(9600);

  Transceiver.init();
  Transceiver.SetMode(MODE_PROGRAM);
  Transceiver.SetTransmitPower(OPT_TP30);
  Transceiver.SetUARTBaudRate(UDR_9600);
  Transceiver.SetAirDataRate(ADR_300);
  Transceiver.SetAddressH(0);
  Transceiver.SetAddressL(0);
  Transceiver.SetChannel(13);

  Transceiver.SaveParameters(PERMANENT);
  Transceiver.Reset();

  Transceiver.PrintParameters();
  Transceiver.SetMode(MODE_NORMAL);
  Transceiver.Reset();
  ESerial.flush();
}

void loop() {
  ESerial.println(ii);
  smartdelay(2000);
  ii+=0.01;  
}

void smartdelay(unsigned long timeout){
  unsigned long t = millis();
  while (digitalRead(PIN_AX)== LOW){
    if ((millis()-t)>timeout){
      break;
    }
  }
  t = millis();
  while ((millis()-t)<20){};
}
