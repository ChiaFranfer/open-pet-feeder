
//Sensor de nivel tipo boya
//Salida a relé, para bomba de agua

#define nivel 9 //mido el nivel de agua en el pin D9
#define RELAY_PIN 7 //conecto arduino con rele a traves de pin D7

void setup()
{
  Serial.begin(9600);
  pinMode(nivel, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
   int SensorNivel = digitalRead(nivel); //leemos lo que marca el nivel
  
   Serial.println(SensorNivel); //comprobamos la lectura

   if (SensorNivel == 1){
    digitalWrite(RELAY_PIN, LOW); //si el agua supera el nivel, cortamos el paso con rele a 0
   } else {
    digitalWrite(RELAY_PIN, HIGH);
   }

   delay(1000);
}
  
