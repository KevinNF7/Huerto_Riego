#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Servo.h>

Adafruit_INA219 ina219;
// Pines del relay y sensores
const int relayPin1 = 2; //Sensores
const int relayPin2 = 3; //Solenoide Riego
const int relayPin3 = 4; //Solenoide Fertilizado
const int relayPin4 = 5; //Fertilizado Mezcla
const int relayPin5 = 6; //Fertilizado Bomba
const int relayPin6 = 7; //Luz

const int aguaTriggerPin = 8; //Nivel de agua
const int aguaEchoPin = 9; //Nivel de agua


//Variables para la dosificacion
Servo myServo;
const int servoPin = 9;
 
int anguloObjetivo = 32;
int anguloActual = 0;

//PINS de los sensores
const int humedadSensorPin1 = A0;
const int humedadSensorPin2 = A1;
const int humedadSensorPin3 = A2;
const int humedadSensorPin4 = A3;
const int humedadSensorPin5 = A4;
const int humedadSensorPin6 = A5;
const int humedadSensorPin7 = A6;
const int lm35SensorPin = A7;

//Variables de control
int controlHumedadMin = 0;
int controlHumedadMax = 0;
String controlHoraEncendido = "";
String controlHoraApagado = "";
int controlFertilizadoPeriodo = 0;
int controlFertilizadoReinicio = 0;
int controlFertilizadoRecarga = 0;
int controlLecturaForzada = 0;
String controlHoraActual = "";
int controlLuz = 0;

//Variables de lectura de los sensores
float humedadSensor1 = 0;
float humedadSensor2 = 0;
float humedadSensor3 = 0;
float humedadSensor4 = 0;
float humedadSensor5 = 0;
float humedadSensor6 = 0;
float humedadSensor7 = 0;
float humedadPromedio = 0;
float corrienteINA = 0;
float potenciaINA = 0;
float current_acs = 0;
float nivelDeAgua = 0;
float fertilizante = 0;
float temperatura = 0;


float humedadMinima = 716;
float humedadMaxima = 380;
float fertilizantePeriodo = 100;

//Variables para controlar el tiempo
unsigned long previousMillis = 0;  // Almacena el tiempo en que se activó la última acción
unsigned long releOnMillis = 0;
unsigned long releOffMillis = 0;


bool pruebaDeEncendido = true;



unsigned long intervaloDeMedicion = 15000; // Intervalo de 30 minutos en ms

unsigned long servoMillis = 0;
unsigned long gearMillis = 0;
unsigned long pumpMillis = 0;
unsigned long manualPreviousMillis = 0; // Almacena el tiempo para la medición manual

bool isLecturaForzada = false;
bool isRiegoActivo = false;
bool isNecesarioServo = false;
bool isNecesitaFertilizante = false;
bool isFertilizadoEnConfiguracion = false;


int contadorDias = 0;

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

  pinMode(relayPin1, OUTPUT); 
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);
  pinMode(relayPin6, OUTPUT);
 
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);
  digitalWrite(relayPin5, HIGH);
  digitalWrite(relayPin6, HIGH);
  
  pinMode(aguaTriggerPin, OUTPUT);
  pinMode(aguaEchoPin, INPUT);
  myServo.attach(servoPin);
  myServo.write(0);
  fertilizerPrevious = 0;
}


void loop() {
  
  unsigned long currentMillis = millis();  // Obtiene el tiempo actual

  //Fertilizado
  if(currentMillis - fertilizerPrevious >= dayDuration || !isNecesitaFertilizante){
    contadorDias++;
    fertilizerPrevious = currentMillis;
    if(contadorDias >= fertilizantePeriodo){
      isNecesitaFertilizante = true;
      contadorDias = 0;
    }
  }
  
  //Recibir datos de control del NodeMCU
  if(Serial.available()){
    
    String datosControl = Serial.readStringUntil('\n');
    actualizarDatos(datosControl); //Actualizar
    
    
  }
  
  switch (currentState) {
    case WAITING:
      
      if (currentMillis - previousMillis >= intervaloDeMedicion) {
        // Ha pasado el intervalo
        digitalWrite(relayPin1, LOW);  // Enciende el relé
        previousMillis = currentMillis;
        currentState = RELAY_ON;
        releOnMillis = currentMillis;
        
      }
      if(isLecturaForzada){
        //Lectura forzada
        digitalWrite(relayPin1, LOW); // Enciende el relé
        currentState = RELAY_ON;
        releOnMillis = currentMillis;
        isLecturaForzada = false;
      }
      break;


    case RELAY_ON:
       //Actualizar el tiempo de intervalo
      if(pruebaDeEncendido){
        intervaloDeMedicion = 1800000; //30 minutos en ms
        pruebaDeEncendido = false;
      }
      if (currentMillis - releOnMillis >= 1000) {
        // Espera 1 segundo después de encender el relé
        humedadSensor1 = analogRead(humedadSensorPin1);
        humedadSensor2 = analogRead(humedadSensorPin2);
        humedadSensor3 = analogRead(humedadSensorPin3);
        humedadSensor4 = analogRead(humedadSensorPin4);
        humedadSensor5 = analogRead(humedadSensorPin5);
        humedadSensor6 = analogRead(humedadSensorPin6);
        humedadSensor7 = analogRead(humedadSensorPin7);
        humedadPromedio = (humedadSensor1 + humedadSensor2 + humedadSensor3 +
                            humedadSensor4 + humedadSensor5 + humedadSensor6 + humedadSensor7)/7;
        
        
        nivelDeAgua = obtenerDatosAgua();

        if(!ina219.begin()){
        //Verifica si esta activo
        }
        corrienteINA = ina219.getCurrent_mA();
        potenciaINA = ina219.getPower_mW();
        temperatura = obtenerTemperatura();


        String output = String(humedadSensor1) + "," + 
                        String(humedadSensor2) + "," + 
                        String(humedadSensor3) + "," + 
                        String(humedadSensor4) + "," + 
                        String(humedadSensor5) + "," + 
                        String(humedadSensor6) + "," + 
                        String(humedadSensor7) + "," + 
                        String(potenciaINA) + ","+ 
                        String(corrienteINA) + "," +
                        String(nivelDeAgua) + "," + 
                        String(fertilizante) + "," + 
                        String(temperatura);
                        
        Serial.println(output);
        fertilizante = 0;

        if(humedadPromedio >= humedadMinima && !isRiegoActivo){
          
          if(!isNecesitaFertilizante){
            digitalWrite(relayPin2, LOW); //Inicia el riego
            intervaloDeMedicion = 30000; // 30 Segundos
            isRiegoActivo = true;
          
          }
        }
        
        if(humedadPromedio <= humedadMaxima && isRiegoActivo){
          //Detener el riego
          digitalWrite(relayPin2, HIGH);
          isRiegoActivo = false;
          intervaloDeMedicion = 1800000; //30 minutos
          
        }
        releOffMillis = currentMillis;
        currentState = RELAY_OFF;       
      }
      break;


    case RELAY_OFF:
      
      if (currentMillis - releOffMillis >= 1000) {
        // Espera 1 segundo después de leer el sensor
        digitalWrite(relayPin1, HIGH);   // Apaga el relé
        
        //Si no necesita fertilizado
        if(!isNecesitaFertilizante){
          currentState = WAITING;
        }else{
          //Si necesita fertilizante
          if(!isFertilizadoEnConfiguracion){
            gearMillis = currentMillis;
            servoMillis = currentMillis;
            currentState = FERTILIZER_DOSE;
            isNecesarioServo = true;
          }else{
            currentState = WAITING;
            intervaloDeMedicion = 300000;
          }
        }
      }
      break;
  

    
    case FERTILIZER_DOSE:

      digitalWrite(relayPin4,LOW);//Energiza el motorreductor
      //El servo se movera 1 grado cada 1 segundos
      if (currentMillis - servoMillis >= 1000 && anguloActual <= anguloObjetivo && isNecesarioServo) {
        servoMillis = currentMillis;
        anguloActual++;  // Incrementar el ángulo en 1 grado
        myServo.write(anguloActual);
        if (anguloActual >= anguloObjetivo) {
          //Si ha llegado al maximoo
          if(anguloActual >= 128){
            
            anguloActual = 0;
            myServo.write(anguloActual);
            isNecesarioServo = false;
          
          }else{
          
            anguloObjetivo += 32;
            isNecesarioServo = false;
          
          }
        }
      }
      //Motorreducto ha estado encendido por 90 segundos
      if(currentMillis - gearMillis >= 90000){
        
        pumpMillis = currentMillis;
        digitalWrite(relayPin4, HIGH); //Apaga el motorreductor
        digitalWrite(relayPin3, LOW); //Abre la solenoide
        digitalWrite(relayPin5, LOW); //Enciende las bombas
        currentState = FERTILIZER_PUMP;
      }
            
      break;

    case FERTILIZER_PUMP:
      //Espera 120 segundos
      if(currentMillis - pumpMillis >= 120000){
        digitalWrite(relayPin5, HIGH); //Apaga las bombas
        digitalWrite(relayPin3, HIGH); //Cierra la solenoide
        currentState = WAITING;
        fertilizante = 180;
      }

      break;
    
  }

}  

float obtenerTemperatura(){
  int valor = analogRead(lm35SensorPin);
  float voltios = (valor*5) / 1024.0;
  float celsius = (voltios * 100) - 32;
  return celsius;
}


float obtenerDatosAgua(){
  
  float duration;
  digitalWrite(aguaTriggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(aguaTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(aguaTriggerPin, LOW);
  duration = pulseIn(aguaEchoPin,HIGH);

  return duration;
}


bool isRangoDeEncendido(String encendido, String apagado, String actual){
  //Formato de la hora HH:MM
  int encendidoHora = encendido.substring(0,2).toInt();
  int encendidoMinutos = encendido.substring(3,5).toInt();
  
  int apagadoHora = apagado.substring(0,2).toInt();
  int apagadoMinutos = apagado.substring(3,5).toInt();
  
  int actualHora = actual.substring(0,2).toInt();
  int actualMinutos = actual.substring(3,5).toInt();

  int encendidoMinutosTotales = encendidoHora * 60 + encendidoMinutos;
  int apagadoMinutosTotales = apagadoHora * 60 + apagadoMinutos;
  int actualMinutosTotales = actualHora * 60 + actualMinutos;

  if(actualMinutosTotales >= encendidoMinutosTotales && actualMinutosTotales <= apagadoMinutosTotales){
    return true;
  }else { return false;}

}

void actualizarDatos(String datosControl){

  int startIndex = 0;
  int commaIndex = datosControl.indexOf(',');

  // Almacenar el primer número
  controlHumedadMin = datosControl.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlHumedadMax = datosControl.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlHoraEncendido = datosControl.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlHoraApagado = datosControl.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlHoraActual = datosControl.substring(startIndex, commaIndex);
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlFertilizadoPeriodo = datosControl.substring(startIndex, commaIndex).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlFertilizadoReinicio = (datosControl.substring(startIndex, commaIndex)).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);
  
  controlFertilizadoRecarga = (datosControl.substring(startIndex, commaIndex)).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',', startIndex);

  controlLecturaForzada = (datosControl.substring(startIndex, commaIndex)).toInt();
  startIndex = commaIndex + 1;
  commaIndex = datosControl.indexOf(',',startIndex);
  // Almacenar el ultimo numero
  controlLuz = (datosControl.substring(startIndex)).toInt();  
  

  humedadMinima = controlHumedadMin;
  humedadMaxima = controlHumedadMax;

  fertilizantePeriodo = controlFertilizadoPeriodo;
  //Serial.println("Fertilizado reinicio= " + t_fertilizadoReinicio + " FertilizadoRecarga = " + t_fertilizadoRecarga + " LecturaForzada = " + t_lecturaForzada);
  
  if(isRangoDeEncendido(controlHoraEncendido, controlHoraApagado, controlHoraActual) || controlLuz == 1){
    digitalWrite(relayPin6, LOW);
  }else{
    digitalWrite(relayPin6, HIGH);
  }
  if(controlLuz == 0){
    digitalWrite(relayPin6, HIGH);
  }

  if(controlLecturaForzada == 1){
    isLecturaForzada = true;
  }

  if(controlFertilizadoReinicio == 1 || controlFertilizadoRecarga == 1){
    isFertilizadoEnConfiguracion = true;

    if(controlFertilizadoRecarga == 1){
      anguloActual = 128;
      myServo.write(anguloActual);  
    }
    if(controlFertilizadoReinicio== 1){
      anguloActual = 0;
      myServo.write(anguloActual);
      isFertilizadoEnConfiguracion = false;
    }
  }
  
}
}
