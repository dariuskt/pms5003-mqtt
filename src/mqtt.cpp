#include "mqtt.h"
#include "ArduinoJson.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);


void setupMqtt()
{
    String clientId = WiFi.hostname();

    mqtt.setServer(BROKER_HOST, BROKER_PORT);
    mqtt.setCallback(configCallback);
    mqtt.connect(clientId.c_str());
    mqtt.subscribe((char*)("config/"+clientId).c_str(), 1);
}

void loopMqtt() {
    mqtt.loop();
}

void destroyMqtt() {
    mqtt.disconnect();
}

void sendMessage()
{
    if (mqtt.connected()) {
        char msg[256];
        char topic[64];
        sprintf(msg
            , "{\"host\":\"%s\",\"ugm3_pm_01_0\":%d,\"ugm3_pm_02_5\":%d,\"ugm3_pm_10_0\":%d,\"count_pm_00_3\":%d,\"count_pm_00_5\":%d,\"count_pm_01_0\":%d,\"count_pm_02_5\":%d,\"count_pm_05_0\":%d,\"count_pm_10_0\":%d,\"configured\":%d}"
            , config.name
            , state.ugm3_pm_01_0
            , state.ugm3_pm_02_5
            , state.ugm3_pm_10_0
            , state.count_pm_00_3
            , state.count_pm_00_5
            , state.count_pm_01_0
            , state.count_pm_02_5
            , state.count_pm_05_0
            , state.count_pm_10_0
            , state.configured
        );

        sprintf(topic, "data/%s/json", config.name);
        mqtt.publish(topic, (char*)msg, true);
        Serial.printf("publishing: %s:%s\n",topic, msg);
    } else {
        Serial.println("MQTT disconnected. restarting ...");
        ESP.restart();
    }
}

void configCallback(char* topic, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    strlcpy(config.name, doc["name"] | "unknown", sizeof(config.name));
    config.read_delay = doc["read_delay"];
    state.configured = 1;

    Serial.print("\n\nConfiguration aquired:\n");
    Serial.printf("config.name='%s'\n", config.name);
    Serial.printf("config.read_delay='%d'\n", config.read_delay);
}

