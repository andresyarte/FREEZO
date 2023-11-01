#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "Classes/TemperatureSensor.h"
#include "Mqtt.h"


void setup_wifi(); // Función para conectarse a Wi-Fi
void callback(char*, byte*, unsigned int); // Función para procesar mensajes recibidos por MQTT
void reconnect(); // Función para conectarse a MQTT
float readTemp(); // Función para leer la temperatura

UBYTE *BlackImage;  // Declarar BlackImage como una variable global

const int ThermistorPin = 35;         // Pin de datos del termistor (pines 2, 4 no funcionan por uso de Wi-Fi)
const int SeriesResistor = 10000;     // Resistencia en serie con el termistor [ohms]
const int ThermistorNominal = 10000;  // Resistencia del termistor a la temperatura nominal [ohms]
const int NominalTemp = 25;           // Temperatura para resistencia nominal del termistor [ºC]
const int BCoefficient = 3950;        // Coeficiente beta del termistor [ºC]
const int NumSamples = 20;            // Número de muestras para sacar un promedio de lecturas
int samples[NumSamples];              // Arreglo con las muestras

const char* ssid = "Totalplay-4EA5";
const char* password =  "4EA57B68uz5tViRf";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* topico_salida = "equipoPATO";
const char* topico_entrada = "equipoPATO";

const int MSG_BUFFER_SIZE = 80;       // Tamaño de buffer del mensaje
char msg[MSG_BUFFER_SIZE];            // Generar un arreglo para el mensaje

//WiFiClient espClient;
//PubSubClient client(espClient);
TemperatureSensor sensor(20.0,30.0);

void setup() {
  Serial.begin(9600);
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  //if (!client.connected()) {
  //reconnect();
  //}
  
    printf("EPD_2IN7_test Demo\r\n");
    DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN7_V2_Init();
    EPD_2IN7_V2_Clear();
    DEV_Delay_ms(500);

    // Create a new image cache
    UWORD Imagesize = ((EPD_2IN7_V2_WIDTH % 8 == 0) ? (EPD_2IN7_V2_WIDTH / 8) : (EPD_2IN7_V2_WIDTH / 8 + 1)) * EPD_2IN7_V2_HEIGHT;

    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while (1);
    }
    
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 270, WHITE);

    // Show image for array
    printf("show image for array\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    EPD_2IN7_V2_Display(BlackImage);
    DEV_Delay_ms(500);

    // Drawing on the image
    Paint_NewImage(BlackImage, EPD_2IN7_V2_WIDTH, EPD_2IN7_V2_HEIGHT, 90, WHITE);
    printf("Drawing\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // Agregar el mensaje "Bienvenido a FREEZO" en toda la pantalla
    Paint_DrawString_EN(10, 60, "Bienvenido a     FREEZO", &Font24, WHITE, BLACK);
    DEV_Delay_ms(5000);

}

//  Monitor CLASE
float monitor(float steinhart) { // Función para leer la temperatura del termistor
  
  
  // Actualizar la pantalla con el texto de la temperatura
  EPD_2IN7_V2_Display_Base(BlackImage);
  Paint_Clear(WHITE);
  Paint_DrawString_EN(10, 0, "La temperatura actual es de:", &Font24, WHITE, BLACK);
  char tempStr[10];
  dtostrf(steinhart, 4, 2, tempStr);
  Paint_DrawString_EN(85, 85, tempStr, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(170, 85, " C", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(0, 150, "30 de Octubre del 2023", &Font12, WHITE, BLACK);
  return steinhart;
}

void loop() {
  float t;
  t = sensor.readTemp(); 
  monitor(t); // Leer la temperatura del sensor
  //if (!client.connected()) {
  //  reconnect();
  //}
  //client.loop();
  snprintf (msg, MSG_BUFFER_SIZE, "{\"dispositivo\":\"Refrigerador 1\",\"tipo\":\"Temperatura\",\"dato\":%.2f}", t*1.0);
  client.publish(topico_entrada, msg);
  delay(10000);  // Esperar 5 segundos antes de leer la temperatura nuevamente
}

// WIFI & DATOS CLASE
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.print("\nConnected to the WiFi network ");
    Serial.println(ssid);
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

// MQTT CLASE
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// MQTT CLASE
void reconnect() { // Función para conectarse a MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected.");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
      client.subscribe(topico_entrada);
      client.subscribe(topico_salida);   
    } else {
      Serial.print("Failed, rc = ");
      Serial.print(client.state());
      Serial.println(". Try again in 5 seconds.");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}