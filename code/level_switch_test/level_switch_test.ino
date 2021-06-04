
//Sensor de nivel tipo boya
//Salida a relé, para bomba de agua

#define nivel 9 //mido el nivel de agua a la DI9
#define 
#define RELAY_PIN 7

void setup()
{
  Serial.begin(9600);
  pinMode(nivel, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop()
{
   int SensorNivel = digitalRead(nivel); //leemos lo que marca el nivel
  
   Serial.println(SensorNivel); //

   if (SensorNivel == 1){
    digitalWrite(RELAY_PIN, LOW);
   } else {
    digitalWrite(RELAY_PIN, HIGH);
   }

   delay(1000);
}
  
