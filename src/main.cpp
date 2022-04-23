#include <Arduino.h>
#include "Ticker.h"
#include "mqtt.h"
#include <PMserial.h>

#define PMS_RX D4
#define PMS_TX D4

SerialPM pms(PMS5003, PMS_RX, PMS_TX);

Ticker wd;
int wd_counter = 0;


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
    pms.read();

    if (pms) {
        state.pieces = pms.pm25;
        return state.pieces;
    } else {
        return -1;
    }
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

    pms.init();

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