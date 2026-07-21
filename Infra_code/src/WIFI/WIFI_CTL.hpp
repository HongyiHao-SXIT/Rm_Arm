#ifndef __WIFI_CTL_HPP_
#define __WIFI_CTL_HPP_

#include <WiFi.h>
#include "../Robot_arm.hpp"
#include "../PC_BLE/PC_BLE_CTL.hpp"

class WIFI_CTL {
public:
    void init(PC_BLE_CTL* pc_ble_ctl);
    void task(LeArm_t* robot, Led_t* led, Buzzer_t* buzzer);

private:
    PC_BLE_CTL* pc_ble_obj = nullptr;
    WiFiServer server{5000};
    WiFiClient client;
    bool started = false;

    void ensure_client_connected();
};

#endif
