#include "WIFI_CTL.hpp"
#include "../Config.h"

void WIFI_CTL::init(PC_BLE_CTL* pc_ble_ctl)
{
    pc_ble_obj = pc_ble_ctl;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
    server = WiFiServer(WIFI_TCP_PORT);
    server.begin();
    started = true;
}

void WIFI_CTL::ensure_client_connected()
{
    if (!started) {
        return;
    }

    if (client && client.connected()) {
        return;
    }

    client = server.available();
    if (client) {
        client.setNoDelay(true);
    }
}

void WIFI_CTL::task(LeArm_t* robot, Led_t* led, Buzzer_t* buzzer)
{
    if (!started || pc_ble_obj == nullptr) {
        return;
    }

    ensure_client_connected();
    if (client && client.connected()) {
        pc_ble_obj->set_stream(&client);
        pc_ble_obj->PC_BLE_Task(robot, led, buzzer);
    }
}
