#include <SoftwareSerial.h>
#include "EBYTE.h"
#include <avr/io.h>
#include <Servo.h>

// v1.1a PCB
#define PUMP 7
#define LEVEL_SW1 8  // nivel superior
#define LEVEL_SW2 10 // nivel inferior
#define SERVO 9
#define PIN_RX 2
#define PIN_TX 3
#define PIN_M0 4
#define PIN_M1 5
#define PIN_AX 6

#define LORA_CHANNEL 5

#define REFILL_TIME 5000  // rellenarBebedero: 5000 ms == 5s para rellenar bebedero (default)

#define SPEED_SERVO 1500  // default: 1500. Lower value, it goes faster

#define NUM_CICLO 4       // rellenarComedero, mantener movimiento giratorio en un ciclo de rellenado

#define TOTAL_REFILL 20   // rellenarComedero, total de recargas de pienso desde reserva hasta llegar al 80% de vaciado (20% no lo contamos por seguridad)

#define BAUDRATE 115200

int num_refill = 0;       // contador de veces que se ha rellenado el comedero desde la reserva, si es igual a TOTAL_REFILL entonces dejamos de rellenar
Servo servo1;

SoftwareSerial ESerial(PIN_RX, PIN_TX); // "falso" serial para comunicarnos con el E32 (creado en SW)
EBYTE Transceiver(&ESerial, PIN_M0, PIN_M1, PIN_AX);

struct ebyte_struct {
  unsigned long count;
  String msg;
};
ebyte_struct ebyte_msg;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println("Serial initialized");
  delay(300);

  // start the transceiver serial port--i have yet to get a different
  // baud rate to work--data sheet says to keep on 9600
  ESerial.begin(9600);

  // init ebyte
  Serial.println("Init transceiver");
  // this init will set the pinModes for you
  Transceiver.init();

  // all these calls are optional but shown to give examples of what you can do

  //Serial.println(Transceiver.GetAirDataRate());
  //Serial.println(Transceiver.GetChannel());

  Transceiver.SetMode(MODE_NORMAL);
  //Transceiver.SetMode(MODE_WAKEUP);

  Transceiver.SetUARTBaudRate(UDR_9600);
  Transceiver.SetAirDataRate(ADR_2400);
  Transceiver.SetAddressH(2);
  Transceiver.SetAddressL(0);

  Transceiver.SetChannel(LORA_CHANNEL);
  Transceiver.SetTransmitPower(OPT_TP21);
  Transceiver.SetPullupMode(0);
  Transceiver.SetFECMode(1);

  // save the parameters to the unit,
  //Transceiver.SaveParameters(PERMANENT);
  Transceiver.SaveParameters(TEMPORARY);

  // you can print all parameters and is good for debugging
  // if your units will not communicate, print the parameters
  // for both sender and receiver and make sure air rates, channel
  // and address is the same
  Transceiver.PrintParameters();

  ebyte_msg.count = 0;

  // inicializar sensores
  Serial.println("");
  pinMode(LEVEL_SW1, INPUT);
  pinMode(LEVEL_SW2, INPUT);
  pinMode(PUMP, OUTPUT);

  servo1.attach(SERVO);

  // Ejemplo de F(): https://techexplorations.com/guides/arduino/programming/f-macro/
  // Moar: https://learn.adafruit.com/memories-of-an-arduino/optimizing-sram
  Serial.println(F("Resumen de comandos aptos: status, ping, rellenar_bebedero,"));
  Serial.println(F("rellenar_comedero, reset_comedero, check_levels y update_oled"));
  Serial.println(F("Formato de los mensajes de respuesta: ack;field1;var1"));
  
  Serial.println("Ready...");
  delay(500);
}

void loop() {
  if (Serial.available() > 0) {
    String static dat_rec;

    dat_rec = Serial.readString();
    dat_rec.trim(); //quita espacio en blanco

    Serial.println("Received data: " + dat_rec);

    readSensor(dat_rec);
  }

}

void readSensor(String data) {
  String static msg;

  if (data.equals("status")) {
    Serial.println("to do");
    msg = "ack;status;" + String(num_refill);

    sendMessage(msg);
  }

  if (data.equals("ping")) {
    msg = "ack;pong";

    sendMessage(msg);
  }

  if (data.equals("reset_comedero")) {
    num_refill = 0;
    
    msg = "ack;reset_comedero";

    sendMessage(msg);
  }
  
  if (data.equals("rellenar_comedero")) {
    String feeder = rellenarComedero();

    msg = "ack;feeder_refill;" + feeder;

    sendMessage(msg);
    
  }

  if (data.equals("rellenar_bebedero")) {
    String refill = rellenarBebedero();

    msg = "ack;water_refill;" + refill;

    sendMessage(msg);
  }

  if (data.equals("check_levels")) {
    int waterLevel = checkWaterLevel();

    msg = "ack;water;" + String(waterLevel);

    sendMessage(msg);
  }

  if (data.equals("update_oled")) {
    Serial.println("to do");
  }

}
//Datos recibidos por serial, por eso printeo desde Serial.
void sendMessage(String data) {
  Serial.println("Sending data: " + data);
  
  ebyte_msg.msg = data;
  ebyte_msg.count++; 
  
  Transceiver.SendStruct(&ebyte_msg, sizeof(ebyte_msg));
  Serial.println("Sent. Count " + String(ebyte_msg.count));
  delay(1000);
}

int checkWaterLevel() {
  boolean sw1 = digitalRead(LEVEL_SW1);
  boolean sw2 = digitalRead(LEVEL_SW2);

  if (sw1) {
    return 100; // 100%
  } else if (sw2) {
    return 50; // 50%
  } else {
    return 25; // < 25%
  }
}

String rellenarBebedero() {
  int waterLevel = checkWaterLevel();

  if (waterLevel > 25) {
    //accionar relé para activar bomba
    Serial.println("Rellenando bebedero...");

    digitalWrite(PUMP, HIGH);
    delay(REFILL_TIME);
    digitalWrite(PUMP, LOW);

    //delay(500);

    Serial.println("Bebedero lleno");

    return "ok";
  } else {
    return "ko";
  }

}

String rellenarComedero(){

  if (num_refill >= 0 && num_refill <= TOTAL_REFILL){
    
    Serial.println("Rellenando comedero...");
    Serial.println("Contador de rellenado: " + String(num_refill));
    Serial.println("Ciclos restantes de rellenado: " + String((TOTAL_REFILL-num_refill)));
    
    for (int i = 0; i <= NUM_CICLO; i++) {    //ajustar en defines NUM_CICLO si queremos un ciclo de rellenado más largo o más corto
      //Serial.println(i);
      servo1.write(0);
      delay(SPEED_SERVO);
      servo1.write(105);
      delay(SPEED_SERVO);
    }

    // Posicion estatica
    servo1.write(90);
    delay(SPEED_SERVO);   // ajustar en define
    
    num_refill++;
    Serial.println("Ciclo " + String(num_refill) + "/20 de rellenado finalizado.");
    
    return("ok");
  } else{
    return("ko");
  }

}
