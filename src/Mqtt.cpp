#include "Mqtt.h"
#include <iostream>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>


void Mqtt::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void Mqtt::reconnect() { // Función para conectarse a MQTT
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection... ");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
            Serial.println("connected.");


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

void Mqtt::Mqtt_publish(float t){
    char msg[MSG_BUFFER_SIZE];
    snprintf (msg, MSG_BUFFER_SIZE, "{\"dispositivo\":\"Refrigerador1\",\"tipo\":\"Temperatura\",\"dato\":%.2f}", t*1.0);
    client.publish(topico_entrada, msg);
    delay(10000); 
}

void Mqtt::Mqtt_setup(){
    client.setServer(mqtt_server, 1883);
    //client.setCallback(callback(topic,payload,length));
}