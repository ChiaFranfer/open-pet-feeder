#include <Servo.h>
#include "LowPower.h"

Servo servo1;

#define PIN_SERVO 9         //conectado a D9 del arduino nano, que es pin digital y PWM
#define NUM_CICLO 4
#define SPEED_SERVO 1500    //default: 1500. Lower value, it goes faster

#define SLEEP_CYCLES 4      // tiempo = 4S * SLEEP_CYCLES[0:5) = 16 seg

void rellenarComedero(){
  Serial.println("Rellenando comedero...");
  for(int i=0; i<=NUM_CICLO; i++){
    //Serial.println(i);
    servo1.write(0);
    delay(SPEED_SERVO);
    servo1.write(105);
    delay(SPEED_SERVO);
  }

  // Posicion estatica
  servo1.write(90);
  delay(SPEED_SERVO);
  Serial.println("OK");
}

void setup() {

  Serial.begin(9600);
  servo1.attach(PIN_SERVO);

  delay(1000);
  //rellenarComedero();

  Serial.println("Ready...");
  delay(1000);
}

void loop() {

  // tengo q decirle aqui que lo haga solo cuando el arduino este despierto, y se supone q esta despierto el tiempo justo
  // para q se cojan datos de sensores y q se envie por lora, formacion de paquetes
  Serial.println("Go to sleep...");
  delay(200);
  
  for (int i = 0;  i < SLEEP_CYCLES; i++){
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);   //se puede subir a SLEEP_8S
  }
  Serial.println("Awaken");
  
  rellenarComedero();

  
  delay(500);

}
