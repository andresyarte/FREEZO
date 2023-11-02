#pragma once

#ifndef Mqtt_h
#define Mqtt_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <vector>
#include <WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient);

// const int MSG_BUFFER_SIZE = 80; 
// char msg[MSG_BUFFER_SIZE];

class Mqtt {
public:
    void callback(char* topic, byte* payload, unsigned int length);
    void reconnect();
    void MqttSetup();
    void MqttPublish(float t);


private:
    const char* ssid = "Totalplay-4EA5";
    const char* password =  "4EA57B68uz5tViRf";
    const char* mqtt_server = "broker.mqtt-dashboard.com";
    const char* topico_salida = "equipoPATO";
    const char* topico_entrada = "equipoPATO";

    const int MSG_BUFFER_SIZE = 80;   // Tamaño de buffer del mensaje
  // char msg[MSG_BUFFER_SIZE];            // Generar un arreglo para el mensaje

};
#endif