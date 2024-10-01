#include <Adafruit_INA219.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// Configura tus datos Wi-Fi
const char* ssid = "PESCADITO"; //NOMBRE DEL WIFI
const char* password = "Pelusa120"; //CONTRASEÑA DEL WIFI

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

//Para leer el sensor INA219
Adafruit_INA219 ina219;

// Variables donde se almacen los valores de los sensores
float t_humidity1;
float t_humidity2;
float t_humidity3;
float t_humidity4;
float t_humidity5;
float t_humidity6;
float t_humidity7;
float t_power_ina;
float t_current_ina;
float t_current_acs;
float t_water_level;
float t_fertilizer;
float t_temperature;

// Variables para controlar el tiempo
unsigned long previousMillis = 0;  
const long interval = 20000; //20 Segundos

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
  // Comprueba si ha pasado el intervalo
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Actualiza el tiempo de referencia
    obtenerDatosTablaControl();
    timeClient.update();  // Actualizar la hora
    String horaActual = timeClient.getFormattedTime().substring(0, 5);
    
    //Serial.print(horaActual);
  }
  //Comprueba si hubo comunicacion serial  
  if(Serial.available()){
    String data_arduino = Serial.readStringUntil('\n');
    //Serial.println(data_arduino);
    data_arduino.trim();
    sendDataToServer(data_arduino);
  }  
}

void sendDataToServer(String data){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient,serverUrlSubirDatos);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    separar(data);

    float humidity1 = t_humidity1;
    float humidity2 = t_humidity2;
    float humidity3 = t_humidity3;
    float humidity4 = t_humidity4;
    float humidity5 = t_humidity5;
    float humidity6 = t_humidity6;
    float humidity7 = t_humidity7;
    if(!ina219.begin()){
    //Serial.println("No se encontro");
    }
    float power_ina = ina219.getPower_mW();
    float current_ina = ina219.getCurrent_mA();
    float current_acs = 0;

    float water_level = t_water_level;
    float fertilizer = t_fertilizer;
    float temperature = t_temperature;

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
  t_humidity1 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity2 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity3 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity4 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity5 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity6 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_humidity7 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);
  
  t_power_ina = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_current_ina = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_current_acs = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_water_level = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  t_fertilizer = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);


  // Almacenar el ultimo numero
  t_temperature = texto.substring(startIndex).toFloat();
}
