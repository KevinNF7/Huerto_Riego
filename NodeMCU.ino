#include <Adafruit_INA219.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// Configura tus datos Wi-Fi
const char* ssid = "PESCADITO"; //NOMBRE DEL WIFI
const char* password = "Pelusa20"; //CONTRASEÑA DEL WIFI

// URL de la API en la base de datos
String serverUrlControl = "http://historia-del-tiempo.com/tablacontrol.php?random=" + String(random(1000));  // Para evitar almacenamiento en cache
String serverUrlSubirDatos = "http://historia-del-tiempo.com/upload.php";

//Para obtener la hora
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5 * 3600, 60000);  // Ajusta a la zona horaria

// Variables donde se almacenan los datos de control
int humedadMin;
int humedadMax;
String horaEncendido;
String horaApagado;
int fertilizadoPeriodo;
bool fertilizadoReinicio;
bool fertilizadoRecarga;
bool lecturaForzada;
String horaActual;
//Para leer el sensor INA219
Adafruit_INA219 ina219;

// Variables donde se almacen los valores de los sensores
float humidity1;
float humidity2;
float humidity3;
float humidity4;
float humidity5;
float humidity6;
float humidity7;
float power_ina;
float current_ina;
float current_acs;
float water_level;
float fertilizer;
float temperature;

// Variables para controlar el tiempo
unsigned long tableMillis = 0;  
const long tableInterval = 20000; //20 Segundos

unsigned long electMillis = 0;
const long electInterval = 1200000; // 20 minutos
void setup() {
  Serial.begin(115200);
  delay(10);

  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi conectado");
  timeClient.begin();

  //obtenerDatosTablaControl();
}

void loop() {
  
  unsigned long currentMillis = millis();
  // Comprueba si ha pasado el intervalo para la actualizacion de parametros
  if (currentMillis - tableMillis >= tableInterval) {
    tableMillis = currentMillis;  // Actualiza el tiempo de referencia
    timeClient.update();  // Actualizar la hora
    horaActual = timeClient.getFormattedTime().substring(0, 5);
    obtenerDatosTablaControl(); //Revisa la tabla de control
  }

  //Comprueba si hubo comunicacion serial para actualizar datos  
  if(Serial.available()){
    String data_arduino = Serial.readStringUntil('\n');
    data_arduino.trim();
    sendDataToServer(data_arduino);
  }
  //Cada 20 minutos envia datos sobre electricidad
  if(currentMillis - electMillis >= electInterval){
    electMillis = currentMillis;
    sendDataToServerElect();            
  }  
}
void sendDataToServerElect(){
  
  power_ina = ina219.getPower_mW();
  current_ina = ina219.getCurrent_mA();
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient,serverUrlSubirDatos);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "humidity1=0&humidity2=0&humidity3=0&humidity4=0&humidity5=0&humidity6=0&humidity7=0&power_ina=" + String(power_ina) + "&current_ina=" + String(current_ina) + "&current_acs=0&water_level=0&fertilizer=0&temperature=0";
  
  int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
    
    } else {}
    http.end();
  }

}
void sendDataToServer(String data){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient,serverUrlSubirDatos);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    separar(data);

    if(!ina219.begin()){
    //Serial.println("No se encontro");
    }

    power_ina = ina219.getPower_mW();
    current_ina = ina219.getCurrent_mA();
    current_acs = 0;

    String httpRequestData = "humidity1=" + String(humidity1) + "&humidity2=" + String(humidity2) + "&humidity3=" + String(humidity3) + "&humidity4=" + String(humidity4) + "&humidity5=" + String(humidity5) + "&humidity6=" + String(humidity6) +
                             "&humidity7=" + String(humidity7) + 
                             "&power_ina=" + String(power_ina) + "&current_ina=" + String(current_ina) + "&current_acs=" + String(current_acs) + "&water_level=" + String(water_level) + "&fertilizer=" + String(fertilizer) + 
                             "&temperature=" + String(temperature);
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
     
      //String response = http.getString();
      //Serial.println(response);
    } else {
      //Serial.print("Error en la conexión: ");
      //Serial.println(httpResponseCode);
    }
    http.end();
  }
}
void obtenerDatosTablaControl() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient wifiClient;
    // Iniciar la conexión HTTP
    http.begin(wifiClient,serverUrlControl);
    int httpCode = http.GET();

    if (httpCode > 0) {  // Si la respuesta es positiva
      //Serial.println("Conexión HTTP exitosa. Código HTTP: " + String(httpCode));
      String payload = http.getString();
      
      // Parsear el JSON recibido
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
        //Serial.print("Error al deserializar JSON: ");
        //Serial.println(error.c_str());
        return;
      }
      // Asignar valores a las variables
      humedadMin = doc["Humedad_min"].as<int>();
      humedadMax = doc["Humedad_max"].as<int>();
      horaEncendido = doc["Horario_enc"].as<String>();
      horaApagado = doc["Horario_apg"].as<String>();
      fertilizadoPeriodo = doc["Fertilizado_periodo"].as<int>();
      fertilizadoReinicio = doc["Fertilizado_reinicio"].as<bool>();
      fertilizadoRecarga = doc["Fertilizado_recarga"].as<bool>();
      lecturaForzada = doc["Lectura_forzada"].as<bool>();

      // Enviar datos al Arduino mediante comunicacion serial
        Serial.print(humedadMin);
        Serial.print(",");
        Serial.print(humedadMax);
        Serial.print(",");
        Serial.print(horaEncendido);
        Serial.print(",");
        Serial.print(horaApagado);
        Serial.print(",");
        Serial.print(horaActual);
        Serial.print(",");
        Serial.print(fertilizadoPeriodo);
        Serial.print(",");
        Serial.print(fertilizadoReinicio);
        Serial.print(",");
        Serial.print(fertilizadoRecarga);
        Serial.print(",");
        Serial.println(lecturaForzada);

    } else {
      //Serial.println("Error en la solicitud HTTP");
    }

    http.end();  // Finalizar la conexión HTTP
  } else {
    //Serial.println("WiFi no conectado");
  }
}

void separar(String texto) {
  // Encontrar las comas y extraer los números
  int startIndex = 0;
  int commaIndex = texto.indexOf(',');

  // Almacenar el primer número
  humidity1 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity2 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity3 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity4 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity5 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity6 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humidity7 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);
  
  power_ina = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  current_ina = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  current_acs = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  water_level = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  fertilizer = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  // Almacenar el ultimo numero
  temperature = texto.substring(startIndex).toFloat();
}
