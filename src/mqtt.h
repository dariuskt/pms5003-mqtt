#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "global.h"


void setupMqtt();
void loopMqtt();
void destroyMqtt();
void sendMessage();
void configCallback(char* topic, byte* payload, unsigned int length);

#endif // MQTT_H