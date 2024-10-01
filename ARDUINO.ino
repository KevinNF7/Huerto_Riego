#include <Wire.h>
#include <Adafruit_INA219.h>


Adafruit_INA219 ina219;
// Pines del relay y sensores
const int relay1Pin = 2; //Sensores
const int relay2Pin = 3; //Solenoide Riego
const int relay3Pin = 4; //Solenoide Fertilizado
const int relay4Pin = 5; //Fertilizado Mezcla
const int relay5Pin = 6; //Fertilizado Bomba
const int relay6Pin = 7; //Luz
const int water_triggerPin = 8; //Nivel de agua
const int water_echoPin = 9; //Nivel de agua

const int servoPin = 11; //Servomotor fertilizado

const int humiditySensorPin1 = A0;
const int humiditySensorPin2 = A1;
const int humiditySensorPin3 = A2;
const int humiditySensorPin4 = A3;
const int humiditySensorPin5 = A4;
const int humiditySensorPin6 = A5;
const int humiditySensorPin7 = A6;
const int lm35SensorPin = A7;



float humidity_sensor1 = 0;
float humidity_sensor2 = 0;
float humidity_sensor3 = 0;
float humidity_sensor4 = 0;
float humidity_sensor5 = 0;
float humidity_sensor6 = 0;
float humidity_sensor7 = 0;
float humidity_average = 0;
float current_ina = 0;
float power_ina = 0;
float current_acs = 0;
float water_level = 0;
float fertilizer = 0;
float temperature = 0;


float humidity_temp = 0;

float min_humidity = 716;
float max_humidity = 380;

unsigned long previousMillis = 0;  // Almacena el tiempo en que se activó la última acción
unsigned long interval = 1800000; // Intervalo de 30 minutos en ms

unsigned long servoMillis = 0;
unsigned long gearMillis = 0;
unsigned long pumpMillis = 0;
unsigned long manualPreviousMillis = 0; // Almacena el tiempo para la medición manual
bool manualMeasurementActive = false;

bool irrigationActive = false;
unsigned long riegoStartMillis = 0;
const long INAinterval = 900000;
unsigned long previousINA = 0;
bool initial = true;
bool needServo = false;
//long initial_interval = 10000;
bool needFertilizer = false;
unsigned long purgeMillis;

enum State {
  WAITING,
  RELAY_ON,
  READ_SENSOR,
  RELAY_OFF,
  MANUAL_RELAY_ON,
  MANUAL_READ_SENSOR,
  MANUAL_RELAY_OFF,
  IRRIGATION_ON,
  IRRIGATION_READ_SENSOR,
  FERTILIZER,
  FERTILIZER_PUMP
};


State currentState = WAITING;


void setup() {
  Serial.begin(115200);
  pinMode(relay1Pin, OUTPUT); // Configura el pin del relé como salida
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
 
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay3Pin, HIGH);
  digitalWrite(relay4Pin, HIGH);
 
  pinMode(water_triggerPin, OUTPUT);
  pinMode(water_echoPin, INPUT);
 
}


void loop() {
  unsigned long currentMillis = millis();  // Obtiene el tiempo actual

  
  if(Serial.available()){
    String message = Serial.readStringUntil('\n');
    Separar(message);
   // if(message.equals("rele_one_off")){digitalWrite(relay1Pin, HIGH);}
   // if(message.equals("rele_two_on")){digitalWrite(relay1Pin, LOW);}
   // if(message.equals("rele_two_off")){digitalWrite(relay1Pin, HIGH);}
  }
  if(currentMillis - previousINA >= INAinterval){
    if(!ina219.begin()){
    //Serial.println("No se encontro");
    }

    humidity_sensor1 = 0;
    humidity_sensor2 = 0;
    humidity_sensor3 = 0;
    humidity_sensor4 = 0;
    humidity_sensor5 = 0;
    humidity_sensor6 = 0;
    humidity_sensor7 = 0;
    water_level = 0;
    fertilizer = 0;
    temperature = 0;
    current_ina = 0;
    power_ina = 0;
    current_acs = 0;
    String current_output = String(humidity_sensor1) + "," + String(humidity_sensor2)+ "," + String(humidity_sensor3)+ "," + String(humidity_sensor4) + "," + String(humidity_sensor5) + "," + String(humidity_sensor6) + "," + String(humidity_sensor7) + ","
                  + String(power_ina) + ","+ String(current_ina) + "," +String(current_acs) + "," + String(water_level) + "," + String(fertilizer) + "," + String(temperature);
    Serial.println(current_output);
    previousINA = currentMillis;
  }
  switch (currentState) {
    case WAITING:
      if()
      if (currentMillis - previousMillis >= interval) {
        // Ha pasado el intervalo
        previousMillis = currentMillis;
        digitalWrite(relay1Pin, LOW);  // Enciende el relé
        currentState = RELAY_ON;
      }
      break;


    case RELAY_ON:
       if(initial){
          interval = 1800000;
          initial = false;
        }
      if (currentMillis - previousMillis >= 1000) {
        // Espera 1 segundo después de encender el relé
        humidity_sensor1 = analogRead(humiditySensorPin1);
        humidity_sensor2 = analogRead(humiditySensorPin2);
        humidity_sensor3 = analogRead(humiditySensorPin3);
        humidity_sensor4 = analogRead(humiditySensorPin4);
        humidity_sensor5 = analogRead(humiditySensorPin5);
        humidity_sensor6 = analogRead(humiditySensorPin6);
        humidity_sensor7 = analogRead(humiditySensorPin7);
        humidity_average = (humidity_sensor1 + humidity_sensor2 +
                            humidity_sensor4 + humidity_sensor5 + humidity_sensor6 + humidity_sensor7)/6;
        water_level = getWaterLevel();
        current_acs = 0;


        if(!ina219.begin()){
        //Serial.println("No se encontro");
         }
        current_ina = ina219.getCurrent_mA();
        power_ina = ina219.getPower_mW();
        fertilizer = 0;
        temperature = getTemperature();


        String output = String(humidity_sensor1) + "," + String(humidity_sensor2)+ "," + String(humidity_sensor3)+ "," + String(humidity_sensor4) + "," + String(humidity_sensor5) + "," + String(humidity_sensor6) + "," + String(humidity_sensor7) + ","
                  + String(power_ina) + ","+ String(current_ina) + "," +String(current_acs) + "," + String(water_level) + "," + String(fertilizer) + "," + String(temperature);
        Serial.println(output);
       
       
        previousMillis = currentMillis;
       if(humidity_average >= min_humidity && !irrigationActive){
         // min_humidity = 818;
          StartIrrigation();
        }
       if(humidity_average <= max_humidity && irrigationActive){
          //max_humidity = 460;
          StopIrrigation();
        }
        currentState = READ_SENSOR;
        
        
      }
      break;


    case READ_SENSOR:
      if (currentMillis - previousMillis >= 1000) {
        // Espera 1 segundo después de leer el sensor
        digitalWrite(relay1Pin,HIGH);   // Apaga el relé
        previousMillis = currentMillis;
        if(needPurge){
          purgeMillis = currentMillis;
          currentState = PURGE;
        }else{
          currentState = WAITING;
        }
      }
      break;
  

    case MANUAL_RELAY_ON:
      if (currentMillis - manualPreviousMillis >= 1000) {
        // Espera 1 segundo después de encender el relé
        humidity_sensor1 = analogRead(humiditySensorPin1);
        humidity_sensor2 = analogRead(humiditySensorPin2);
        humidity_sensor3 = analogRead(humiditySensorPin3);
        humidity_sensor4 = analogRead(humiditySensorPin4);
        humidity_sensor5 = analogRead(humiditySensorPin5);
        humidity_sensor6 = analogRead(humiditySensorPin6);
        humidity_sensor7 = analogRead(humiditySensorPin7);
        humidity_average = (humidity_sensor1 + humidity_sensor2 + humidity_sensor3 +
                            humidity_sensor4 + humidity_sensor5 + humidity_sensor6 + humidity_sensor7)/7;
        water_level = getWaterLevel();
        current_acs = 0;


        fertilizer = 0;
        temperature = getTemperature();


        String output = String(humidity_sensor1) + "," + String(humidity_sensor2)+ "," + String(humidity_sensor3)+ "," + String(humidity_sensor4) + "," + String(humidity_sensor5) + "," + String(humidity_sensor6) + "," + String(humidity_sensor7) + ","
                  + String(power_ina) + ","+ String(current_ina) + "," +String(current_acs) + "," + String(water_level) + "," + String(fertilizer) + "," + String(temperature);
        Serial.println(output);
        manualPreviousMillis = currentMillis;
        currentState = MANUAL_READ_SENSOR;
      }
      break;


    case MANUAL_READ_SENSOR:
      if (currentMillis - manualPreviousMillis >= 1000) {
        // Espera 1 segundo después de leer el sensor
        digitalWrite(relay1Pin, HIGH);   // Apaga el relé
        manualPreviousMillis = currentMillis;
        currentState = MANUAL_RELAY_OFF;
      }
      break;


    case MANUAL_RELAY_OFF:
      manualMeasurementActive = false;
      currentState = WAITING;  // Vuelve al estado de espera
      break;

    case FERTILIZER:
      digitalWrite(relay4Pin,HIGH);
      servoMillis = currentMillis;
      gearMillis = currentMillis;
            //El servo se momera 1 grado cada 0.5 segundos
      if (currentMillis - servoMillis >= 500 && currentAngle < targetAngle && needServo) {
        currentAngle++;  // Incrementar el ángulo en 1 grado
        myServo.write(currentAngle);
        if (currentAngle >= targetAngle) {
          if(currentAngle >= 128){
            currentAngle = 0;  // Reiniciar el ángulo para el siguiente ciclo de 14 días  
            needServo = false;
          }else{
            currentAngle += 32;
            needServo = false;
            
          }
        }
      }
      if(currentMillis - gearMillis >= 90000){
        currentState = FERTILIZER_PUMP;
      }
      
    currentState = FERTILIZER_PUMP;
    case FERTILIZER_PUMP:
      digitalWrite(relay4Pin, LOW);
      digitalWrite(relay5Pin, HIGH);
      
    // Si el servo ha alcanzado el ángulo objetivo, reiniciar el conteo para los 14 días
      
    }
  }


   
}


// Función para iniciar la medición manual
void ManualReading() {
 
  digitalWrite(relay1Pin, LOW);  // Enciende el relé
  manualPreviousMillis = millis(); // Inicia el temporizador para la medición manual
  currentState = MANUAL_RELAY_ON;
}


void StartIrrigation(){
  digitalWrite(relay2Pin, LOW);
  irrigationActive = true;
  interval = 30000;
 
}
void StopIrrigation(){
  digitalWrite(relay2Pin, HIGH);
  interval = 1800000;
  needPurge = true;
  irrigationActive = false;
  //digitalWrite(relay3Pin, LOW);
}
float getTemperature(){
  int value = analogRead(lm35SensorPin);
  float volts = (value*5) / 1024.0;
  float celsius = (volts * 100) - 32;
  return value;
}





float getWaterLevel(){
  float duration;
  digitalWrite(water_triggerPin, LOW);
  delayMicroseconds(2);


  digitalWrite(water_triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(water_triggerPin, LOW);


  duration = pulseIn(water_echoPin,HIGH);
  return duration;
}
void Separar(String m){
  
}
