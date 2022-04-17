#ifndef GLOBAL_H
#define GLOBAL_H

//#define CONFIGURE_WIFI true
#define WIFI_SSID "name"
#define WIFI_PASS "pass"

#define BROKER_HOST "192.168.1.33"
#define BROKER_PORT 1883

#define EXECUTION_TIMEOUT 20

struct globalConfig {
    int sensor_baud = 9600;
    int read_delay = 10000;
    char name[16] = "noname";
};

struct globalState {
    int pieces = 0;
    int configured = 0;
};

extern globalState state;
extern globalConfig config;


#endif // GLOBAL_H