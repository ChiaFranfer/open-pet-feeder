
#include <SoftwareSerial.h>
#include "EBYTE.h"
#include <avr/io.h>

#define PIN_RX 2
#define PIN_TX 3
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

#define LORA_CHANNEL 5

struct ebyte_struct {
  unsigned long count;
  char msg[64];                                           
};
ebyte_struct ebyte_msg;

unsigned long Last;

SoftwareSerial ESerial(PIN_RX, PIN_TX);
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

void setup() {
  Serial.begin(115200);

  ESerial.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();

  // all these calls are optional but shown to give examples of what you can do

  // Serial.println(Transceiver.GetAirDataRate());
  // Serial.println(Transceiver.GetChannel());

  Transceiver.SetMode(MODE_NORMAL);
  
  Transceiver.SetUARTBaudRate(UDR_9600);
  Transceiver.SetAirDataRate(ADR_2400);
  Transceiver.SetAddressH(1);
  Transceiver.SetAddressL(0);
  
  Transceiver.SetChannel(LORA_CHANNEL);
  Transceiver.SetTransmitPower(OPT_TP21);
  Transceiver.SetPullupMode(0);
  Transceiver.SetFECMode(1);
  
  // save the parameters to the unit,
  Transceiver.SaveParameters(TEMPORARY);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();
}

void loop() {

  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()

  if (ESerial.available()) {

    // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method

    Transceiver.GetStruct(&ebyte_msg, sizeof(ebyte_msg));

    // dump out what was just received
    Serial.print("Count: "); Serial.println(ebyte_msg.count);
    Serial.print("msg: "); Serial.println(ebyte_msg.msg);
    Serial.println("***");
    // if you got data, update the checker
    Last = millis();
   
  }
  else {
    // if the time checker is over some prescribed amount
    // let the user know there is no incoming data
    if ((millis() - Last) > 1000) {
      Serial.println("Searching: ");
      Last = millis();
    }

  }
}
