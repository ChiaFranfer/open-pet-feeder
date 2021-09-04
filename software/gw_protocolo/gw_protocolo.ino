
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

  //different baudrate for nano-E32 serial comm (recommend value of 9600)
  ESerial.begin(9600);
  Serial.println("Starting GW");

  // this init will set the pinModes for you
  Transceiver.init();

  // all these calls are optional

  // Serial.println(Transceiver.GetAirDataRate());
  // Serial.println(Transceiver.GetChannel());

  Transceiver.SetMode(MODE_NORMAL);
  
  Transceiver.SetUARTBaudRate(UDR_9600);
  Transceiver.SetAirDataRate(ADR_2400);
  Transceiver.SetAddressH(1);
  Transceiver.SetAddressL(0);
  
  Transceiver.SetChannel(LORA_CHANNEL);
  Transceiver.SetTransmitPower(OPT_TP21); //change transceiver tx-rx power
  Transceiver.SetPullupMode(0);
  Transceiver.SetFECMode(1);
  
  // save the parameters to the unit,
  Transceiver.SaveParameters(TEMPORARY);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();

  ebyte_msg.count = 0;

  //Reduced SRAM cause arduino nano -- optimize sram on string shown on Serial.println using "F" function 
  //Example of F(): https://techexplorations.com/guides/arduino/programming/f-macro/
  //https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram
  
  Serial.println(F("Resumen de comandos aptos: status, ping, rellenar_bebedero,"));
  Serial.println(F("rellenar_comedero, reset_comedero, check_levels y update_oled"));
  Serial.println(F("Formato de los mensajes de respuesta: ack;field1;var1"));

  Serial.println("Ready...");
  delay(500);
}

void loop() {

  // arduino serial: write on serial interface and send msg via LoRa to refugio station

  if (Serial.available() > 0) {
    String static dat_rec;

    dat_rec = Serial.readString();
    dat_rec.trim(); //quita espacio en blanco

    Serial.println("[GW] Comando recibido: " + dat_rec);

    sendMessage(dat_rec); //dat_rec will be sent via radio/LoRa
  }
  
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()

  if (ESerial.available()) {

    // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method

    Transceiver.GetStruct(&ebyte_msg, sizeof(ebyte_msg));

    // dump out what was just received
    Serial.print("[Refugio] ID: "); 
    Serial.println(ebyte_msg.count);
    //Serial.print("msg: "); 
    Serial.print("[Refugio] Msg: ");
    Serial.println(ebyte_msg.msg);
    //Serial.println("***");
    // if you got data, update the checker
    Last = millis();
   
  }
  else {
    // if the time checker is over some prescribed amount
    // let the user know there is no incoming data
    if ((millis() - Last) > 1000) {
      //Serial.println("Searching: ");
      Last = millis();
    }

  }
}

//Data received by pc-nano serial comm -- will be transmitted by our gw LoRa transceiver
void sendMessage(String data) {
  Serial.println("[GW] Enviando por LoRa: " + data);

  // Send to radio
  data.toCharArray(ebyte_msg.msg, data.length()+1);
  ebyte_msg.count++; 
  
  Transceiver.SendStruct(&ebyte_msg, sizeof(ebyte_msg));
  Serial.println("[GW] Msg enviado. ID: " + String(ebyte_msg.count));
  //Serial.println("***");
  delay(1000);
}
