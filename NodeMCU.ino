#include <Adafruit_INA219.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// Configura tus datos Wi-Fi
const char* ssid = "SSDI"; //NOMBRE DEL WIFI
const char* password = "CONTRASEÑA"; //CONTRASEÑA DEL WIFI

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
bool luz;
//Para leer el sensor INA219
Adafruit_INA219 ina219;

// Variables donde se almacen los valores de los sensores
float humedad1;
float humedad2;
float humedad3;
float humedad4;
float humedad5;
float humedad6;
float humedad7;
float potenciaINA;
float corrienteINA;
float nivelDeAgua;
float fertilizante;
float temperatura;

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
  }
  
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
    String arduinoDatos = Serial.readStringUntil('\n');
    arduinoDatos.trim();
    enviarDatosServidor(arduinoDatos);
  }
  //Cada 20 minutos envia datos sobre electricidad
  if(currentMillis - electMillis >= electInterval){
    electMillis = currentMillis;
    enviarDatosServidorElectrico();            
  }  
}
void enviarDatosServidorElectrico(){
  
  potenciaINA = ina219.getPower_mW();
  corrienteINA = ina219.getCurrent_mA();
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient,serverUrlSubirDatos);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData =  "humedad1=0&humedad2=0&humedad3=0&humedad4=0&humedad5=0&humedad6=0&humedad7=0&potenciaINA=" + String(potenciaINA) + 
                              "&corrienteINA=" + String(corrienteINA) +  
                              "&nivelDeAgua=0&fertilizante=0&temperatura=0";
    
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {

    } else {

    }
    http.end();
  }

}
void enviarDatosServidor(String datos){
  
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient,serverUrlSubirDatos);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    procesarDatos(datos);

    if(!ina219.begin()){
    //Verificar INA
    }

    potenciaINA = ina219.getPower_mW();
    corrienteINA = ina219.getCurrent_mA();

    String httpRequestData =  "humedad1=" + String(humedad1) + 
                              "&humedad2=" + String(humedad2) + 
                              "&humedad3=" + String(humedad3) +  
                              "&humedad4=" + String(humedad4) + 
                              "&humedad5=" + String(humedad5) + 
                              "&humedad6=" + String(humedad6) +
                              "&humedad7=" + String(humedad7) + 
                              "&potenciaINA=" + String(potenciaINA) + 
                              "&corrienteINA=" + String(corrienteINA) +  
                              "&nivelDeAgua=" + String(nivelDeAgua) + 
                              "&fertilizante=" + String(fertilizante) + 
                              "&temperatura=" + String(temperatura);
    int httpResponseCode = http.POST(httpRequestData);
    
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
    
      String payload = http.getString();
      
      // Parsear el JSON recibido
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);
      
      if (error) {
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
      luz = doc["Luz"].as<bool>();
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
      Serial.print(lecturaForzada);
      Serial.print(",");
      Serial.println(luz);

    } else {
      //Error en la solicitud HTTP
      }

    http.end();  // Finalizar la conexión HTTP
  
  }
}

void procesarDatos(String texto) {
  // Encontrar las comas y extraer los números
  int startIndex = 0;
  int commaIndex = texto.indexOf(',');

  // Almacenar el primer número
  humedad1 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad2 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad3 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad4 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad5 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad6 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  humedad7 = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);
  
  potenciaINA = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  corrienteINA = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  nivelDeAgua = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  fertilizante = texto.substring(startIndex, commaIndex).toFloat();
  startIndex = commaIndex + 1;
  commaIndex = texto.indexOf(',', startIndex);

  // Almacenar el ultimo numero
  temperatura = texto.substring(startIndex).toFloat();
}
