#include <LowPower.h>

#include <Servo.h>
#include "LowPower.h"

//defines for servo
Servo servo1;

#define PIN_SERVO 9         //conectado a D9 del arduino nano, que es pin digital y PWM
#define NUM_CICLO 4
#define SPEED_SERVO 1500    //default: 1500. Lower value, it goes faster

#define SLEEP_CYCLES 4      // tiempo = 4S * SLEEP_CYCLES[0:5) = 16 seg

//defines for bebedero

#define RELAY_PIN 7
#define REFILL_TIME 3000    // 5000 ms == 5s para rellenar bebedero (default)

//void comprobarBebedero(){

//}

void rellenarBebedero() {

  Serial.println("Rellenando bebedero...");

  digitalWrite(RELAY_PIN, HIGH);
  delay(REFILL_TIME);
  digitalWrite(RELAY_PIN, LOW);

  delay(500);

  Serial.println("Bebedero lleno");
}

void rellenarComedero() {
  Serial.println("Rellenando comedero...");
  for (int i = 0; i <= NUM_CICLO; i++) {
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

  Serial.begin(115200);

  // setup comedero
  //servo1.attach(PIN_SERVO);

  delay(1000);

  // setup bebedero
  pinMode(RELAY_PIN, OUTPUT);


  Serial.println("Ready...");
  delay(1000);
}

void loop() {

  // tengo q decirle aqui que lo haga solo cuando el arduino este despierto, y se supone q esta despierto el tiempo justo
  // para q se cojan datos de sensores y q se envie por lora, formacion de paquetes
  Serial.println("Go to sleep...");
  delay(200);

  for (int i = 0;  i < SLEEP_CYCLES; i++) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);   //se puede subir a SLEEP_8S
  }
  Serial.println("Awaken");

  //rellenarComedero();

  //comprobarBebedero();
  rellenarBebedero();


  delay(500);

}
