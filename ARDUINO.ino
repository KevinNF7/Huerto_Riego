#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Servo.h>

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
int targetAngle = 32;
int currentAngle = 0;

const int humiditySensorPin1 = A0;
const int humiditySensorPin2 = A1;
const int humiditySensorPin3 = A2;
const int humiditySensorPin4 = A3;
const int humiditySensorPin5 = A4;
const int humiditySensorPin6 = A5;
const int humiditySensorPin7 = A6;
const int lm35SensorPin = A7;


float t_humedadMin = 0;
float t_humedadMax = 0;
String t_horaEncendido = "";
String t_horaApagado = "";
float t_fertilizadoPeriodo = 0;
String t_fertilizadoReinicio = "";
String t_fertilizadoRecarga = "";
String t_lecturaForzada = "";
String t_horaActual = "";

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


Servo myServo;
const int pinServo = 9;


float min_humidity = 716;
float max_humidity = 380;
int fertilizer_days;
unsigned long previousMillis = 0;  // Almacena el tiempo en que se activó la última acción
unsigned long interval = 1800000; // Intervalo de 30 minutos en ms

unsigned long servoMillis = 0;
unsigned long gearMillis = 0;
unsigned long pumpMillis = 0;
unsigned long manualPreviousMillis = 0; // Almacena el tiempo para la medición manual
bool needManualReading = false;

unsigned long releOnMillis = 0;
unsigned long releOffMillis = 0;
bool irrigationActive = false;
unsigned long riegoStartMillis = 0;
const long INAinterval = 900000;
unsigned long previousINA = 0;
bool initial = true;
bool needServo = false;
//long initial_interval = 10000;
bool needFertilizer = false;
int fertilizerPeriod = 100;
int daysCount = 0;
bool fertilizerSetting = false;
const unsigned long fertilizerInterval = 1209600000; // 14 días en milisegundos
unsigned long fertilizerPrevious = 0;  // Tiempo en que se inicia el intervalo
const unsigned long dayDuration = 86400000;
enum State {
  WAITING,
  RELAY_ON,
  RELAY_OFF,
  FERTILIZER_PUMP,
  FERTILIZER_DOSE
};


State currentState = WAITING;


void setup() {
  Serial.begin(115200);
  pinMode(relay1Pin, OUTPUT); // Configura el pin del relé como salida
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
  pinMode(relay5Pin, OUTPUT);
  pinMode(relay6Pin, OUTPUT);
 
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay3Pin, HIGH);
  digitalWrite(relay4Pin, HIGH);
  digitalWrite(relay5Pin, HIGH);
  digitalWrite(relay6Pin, HIGH);
  pinMode(water_triggerPin, OUTPUT);
  pinMode(water_echoPin, INPUT);
  myServo.attach(pinServo);
  myServo.write(0);
  fertilizerPrevious = 0;
}


void loop() {
  unsigned long currentMillis = millis();  // Obtiene el tiempo actual

  if(currentMillis - fertilizerPrevious >= dayDuration || !needFertilizer){
    daysCount++;
    fertilizerPrevious = currentMillis;
    if(daysCount >= fertilizerPeriod){
      needFertilizer = true;
      daysCount = 0;
    }
  }
  
  if(Serial.available()){
    String message = Serial.readStringUntil('\n');
    separar(message);
    Serial.print(message);
    actualizar_datos();
    if(comparar_horas(t_horaEncendido,t_horaActual)){
      digitalWrite(relay6Pin,LOW);
    }
    if(comparar_horas(t_horaApagado, t_horaActual) || comparar_horas(t_horaActual,t_horaEncendido)){
      digitalWrite(relay6Pin,HIGH);
    }
  }
  
  switch (currentState) {
    case WAITING:
      
      if (currentMillis - previousMillis >= interval || needManualReading ) {
        // Ha pasado el intervalo
        previousMillis = currentMillis;
        digitalWrite(relay1Pin, LOW);  // Enciende el relé
        currentState = RELAY_ON;
        releOnMillis = currentMillis;
        
      }
      if(needManualReading){
        currentState = RELAY_ON;
        releOnMillis = currentMillis;
        needManualReading = false;
      }
      break;


    case RELAY_ON:
       //Actualizar el tiempo de intervalo
      if(initial){
        interval = 1800000; //30 minutos en ms
        initial = false;
      }
      if (currentMillis - releOnMillis >= 1000) {
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


        if(!ina219.begin()){
        //Serial.println("No se encontro");
        }
        current_ina = ina219.getCurrent_mA();
        power_ina = ina219.getPower_mW();
        
        temperature = getTemperature();


        String output = String(humidity_sensor1) + "," + String(humidity_sensor2)+ "," + String(humidity_sensor3)+ "," + String(humidity_sensor4) + "," + String(humidity_sensor5) + "," + String(humidity_sensor6) + "," + String(humidity_sensor7) + ","
                  + String(power_ina) + ","+ String(current_ina) + "," +String(current_acs) + "," + String(water_level) + "," + String(fertilizer) + "," + String(temperature);
        Serial.println(output);
       
        fertilizer = 0;
        if(humidity_average >= min_humidity && !irrigationActive){
         // min_humidity = 818;
          //Inicia el Riego
          if(!needFertilizer ){
            digitalWrite(relay2Pin, LOW);
            irrigationActive = true;
            interval = 30000; //30Segundos
          }
          
        }
        if(humidity_average <= max_humidity && irrigationActive){
          //max_humidity = 460;
          //Detener el riego
          digitalWrite(relay2Pin, HIGH);
          irrigationActive = false;
          interval = 1800000; //30 minutos
          
        }
        releOffMillis = currentMillis;
        currentState = RELAY_OFF;       
      }
      break;


    case RELAY_OFF:
      
      if (currentMillis - releOffMillis >= 1000) {
        // Espera 1 segundo después de leer el sensor
        digitalWrite(relay1Pin,HIGH);   // Apaga el relé
        if(!needFertilizer){
          currentState = WAITING;
        }else{
          if(!fertilizerSetting){
            gearMillis = currentMillis;
            servoMillis = currentMillis;
            currentState = FERTILIZER_DOSE;
          }else{
            currentState = WAITING;
          }
        }
      }
      break;
  

    
    case FERTILIZER_DOSE:
      digitalWrite(relay4Pin,LOW);
      //El servo se momera 1 grado cada 1 segundos
      if (currentMillis - servoMillis >= 1000 && currentAngle <= targetAngle && needServo) {
        servoMillis = currentMillis;
        currentAngle++;  // Incrementar el ángulo en 1 grado
        myServo.write(currentAngle);
        if (currentAngle >= targetAngle) {
          if(currentAngle >= 128){
            currentAngle = 0;
            
            myServo.write(currentAngle);
            needServo = false;
          }else{
            targetAngle += 32;
            needServo = false;
          }
        }
      }
      if(currentMillis - gearMillis >= 90000){
        
        pumpMillis = currentMillis;
        digitalWrite(relay4Pin, HIGH);
        digitalWrite(relay3Pin, LOW);
        digitalWrite(relay5Pin, LOW);
        currentState = FERTILIZER_PUMP;
      }
      
      
      break;

    case FERTILIZER_PUMP:
      
      if(currentMillis - pumpMillis >= 120000){
        digitalWrite(relay5Pin, HIGH);
        digitalWrite(relay3Pin, HIGH);
        currentState = WAITING;
        fertilizer = 180;
      }

      break;
    
  }

}  

float getTemperature(){
  int value = analogRead(lm35SensorPin);
  float volts = (value*5) / 1024.0;
  float celsius = (volts * 100) - 32;
  return celsius;
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
void separar(String texto){
  // Encontrar las comas y extraer los números
  int startIndex = 0;
  int commaIndex = texto.indexOf(',');

  // Almacenar el primer número
  t_humedadMin = texto.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humedadMax = texto.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_horaEncendido = texto.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_horaApagado = texto.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_horaActual = texto.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_fertilizadoPeriodo = texto.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_fertilizadoReinicio = (texto.substring(startIndex, commaIndex));
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);
  
  t_fertilizadoRecarga = (texto.substring(startIndex, commaIndex));
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  // Almacenar el ultimo numero
  t_lecturaForzada = (texto.substring(startIndex));
}

boolean comparar_horas(String recibida, String actual){

  //Formato HH:MM
  int actualHora = actual.substring(0, 2).toInt();
  int actualMinutos = actual.substring(3, 5).toInt();
  
  //FORMATO HH:MM:SS
  int recibidaHora = recibida.substring(0, 2).toInt();
  int recibidaMinutos = recibida.substring(3, 5).toInt();

  // Compara horas y minutos
  if (actualHora > recibidaHora) {
    return true;
  } else if (actualHora == recibidaHora && actualMinutos >= recibidaMinutos) {
    return true;
  }
  return false;
}
  
void actualizar_datos(){
  min_humidity = t_humedadMin;
  max_humidity = t_humedadMax;
  //Serial.print(comparar_horas(t_horaEncendido,t_horaActual));
  //Serial.print(comparar_horas(t_horaApagado,t_horaActual));
  fertilizer_days = t_fertilizadoPeriodo;
  
  Serial.println("Fertilizado reinicio= " + t_fertilizadoReinicio + " FertilizadoRecarga = " + t_fertilizadoRecarga + " LecturaForzada = " + t_lecturaForzada);
  if(t_lecturaForzada == "1"){
    needManualReading = true;
  }
  if(t_fertilizadoReinicio == "1" || t_fertilizadoRecarga == "1"){
    fertilizerSetting = true;
    if(t_fertilizadoRecarga == "1"){
      currentAngle = 128;
      myServo.write(currentAngle);  
    }
    if(t_fertilizadoReinicio== "1"){
      currentAngle = 0;
      myServo.write(currentAngle);
      fertilizerSetting = false;
    }
  }
}
