#include <LowPower.h>
#include "LowPower.h"


#define SLEEP_CYCLES 4      // tiempo = 4S * SLEEP_CYCLES[0:5) = 16 seg

//defines for bebedero

#define RELAY_PIN 7
#define REFILL_TIME 5000    // 5000 ms == 5s para rellenar bebedero (default)

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

void setup() {

  Serial.begin(115200);

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

  rellenarBebedero();


  delay(500);

}
