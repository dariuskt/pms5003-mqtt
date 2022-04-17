#include <Arduino.h>
#include "Ticker.h"
#include "mqtt.h"
#include <SoftwareSerial.h>

Ticker wd;
int wd_counter = 0;

SoftwareSerial ss(D4,D4);

void initWifi() {
    //Serial.setDebugOutput(true);
    //system_phy_set_powerup_option(3);
    #ifdef CONFIGURE_WIFI
        WiFi.persistent(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
    #endif
}

int readSensor() {
    ss.begin(config.sensor_baud);
    delay(990);

    int i = 0;
    int crc = 0;
    char data[128];
    while (ss.available()) {
        data[i] = ss.read();
        //Serial.printf("byte[%2d]: %X\n", i, data[i]);

        if (i<30)
            crc += data[i];

        if (i>31)
            continue;

        i++;
    }

    if (i != 32)
        return -20;

    if (data[0] != 0x42 || data[1] != 0x4D)
        return -30;

    if (data[30] != (crc>>8) || data[31] != (crc&0xFF))
        return -40;

    if (data[6] > 40)
        return -50;

    state.pieces = data[6];
    state.pieces = state.pieces << 8;
    state.pieces += data[7];
    return state.pieces;
}

void wd_tick() {
    wd_counter++;
    if (wd_counter > EXECUTION_TIMEOUT) {
        Serial.println("Rebooting...");
        ESP.restart();
    }
}
// ===============================================

void setup() {
    Serial.begin(76800);
    Serial.println("\nBooting... ");

    wd.attach(1, wd_tick);

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
    int i = readSensor();
    if (i > 0) {
        sendMessage();
        wd_counter = 0;
        delay(config.read_delay);
    } else {
        Serial.printf("ReadError %d (timeout:%d)\n"
        , i, wd_counter);
    }

}