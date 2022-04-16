#include <Arduino.h>
#include "mqtt.h"
#include <SoftwareSerial.h>

SoftwareSerial ss(D4,D4);

byte incomingByte = 0;

void initWifi() {
    //Serial.setDebugOutput(true);
    //system_phy_set_powerup_option(3);
    #ifdef CONFIGURE_WIFI
        WiFi.persistent(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    #endif
}

void initSensor() {
    //Serial.printf("Setting up sensor serial with %d baud\n", config.sensor_baud);
    ss.begin(config.sensor_baud);
}

int readSensor() {
    ss.flush();
    delay(200);

    int i = 0;
    char data[128];
    while (ss.available()) {
        data[i] = ss.read();
        //Serial.printf("byte[%2d]: %X\n", i, data[i]);
        i++;
        if (i>65)
            return -1;
    }
    if (data[0] != 0x42 || data[1] != 0x4D)
        return -2;

    if (data[6] > 20)
        return -3;

    state.pieces = data[6];
    state.pieces = state.pieces << 8;
    state.pieces += data[7];
    return state.pieces;
}

// ===============================================

void setup() {
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    initWifi();

    Serial.print("Wifi connecting: ");
    while ( WiFi.status() != WL_CONNECTED ) {
        delay(10);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("Connecting to MQTT broker");
    setupMqtt();

    Serial.print("Waiting for configuration: ");
    while ( state.configured == 0 ) {
        delay(10);
        loopMqtt();
        Serial.print(".");
    }
    Serial.println();
 }

void loop() {

    loopMqtt();
    delay(100);
    initSensor();
    if (readSensor() > 0) {
        sendMessage();
        delay(config.read_delay);
    }

}