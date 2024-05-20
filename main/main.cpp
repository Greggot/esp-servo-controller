/**
 * @file
 * @brief  Входная точка приложения
 * @author https://github.com/Greggot/
 */

#include <freertos/FreeRTOS.h>
#include <Bluetooth/Characteristic.hpp>
#include <Bluetooth/Service.hpp>
#include <Bluetooth/Server.hpp>
#include <WiFi/STA.hpp>
#include <TCP/server.hpp>

#include "nvs_flash.h"

extern "C" { void app_main(void); }

void wifi_connection_callback(BLE::Characteristic*, const uint16_t length, const void* value);
void wifi_init();

static WiFi::STA sta;
static TCP::Address tcp_host_address;
static BLE::Characteristic wifi_characteristic(0x4ED, BLE::permition::Write, BLE::property::Write | BLE::property::Notify);

void app_main(void)
{
    nvs_flash_init();

    wifi_init();
    wifi_characteristic.setWriteCallback(wifi_connection_callback);
    auto* connectivity_service = new BLE::Service(0xC111E1A, {&wifi_characteristic});

    static BLE::Characteristic omega_rotor_characteristic(0xA0111, BLE::permition::Write, BLE::property::Write);
    static BLE::Characteristic theta_rotor_characteristic(0xA111E, BLE::permition::Write, BLE::property::Write);
    auto* rotor_service = new BLE::Service(0xCFFFAE, {&omega_rotor_characteristic, &theta_rotor_characteristic});

    static BLE::Server BLE_Device("BTservoController", {connectivity_service, rotor_service});
    BLE::Server::Enable();

    while(true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

template<class Address_t>
void output_address(const char* prefix, const Address_t& address) {
    printf("%s: %u.%u.%u.%u\n", prefix, address.IP[0], address.IP[1], address.IP[2], address.IP[3]);
}

/// @brief Add connector charactertistic notifications,
/// add TCP server start on IP gain.
void wifi_init()
{
    sta.add(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
    [](void*, esp_event_base_t, int32_t, void*){
        // BLE wasn't disabled - it means unsuccessfull IP_EVENT_STA_GOT_IP finish
        // Otherwise, turn off full wifi power to be able to turn on BLE again
        if(BLE::Server::Status())
            wifi_characteristic.Notify(int(1));
        else
            sta.TurnOffFullPower();
        BLE::Server::Enable();
    });

    // Tell BLE application that wifi connection established
    sta.add(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED,
    [](void*, esp_event_base_t, int32_t, void*){
        wifi_characteristic.Notify(int(0));
    });

    sta.add(IP_EVENT, IP_EVENT_STA_GOT_IP, [](void*, esp_event_base_t, int32_t, void* data){        
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
        tcp_host_address = {event->ip_info.ip.addr, 4444};
        output_address("Starting TCP server (4444) on: ", tcp_host_address);

        // Gotta turn off BLE before enabling full power
        BLE::Server::Disable();
        WiFi::STA::TurnOnFullPower();

        // Start server, begin work after first connection
        // device.connect(tcp_host_address);
        // device.listen();
        // auto client = device.otherside();
        // output_address("Client connected: ", client);
    });
}

/// @brief Read [ssid][0][password][0][IP][Port]
void wifi_connection_callback(BLE::Characteristic*, const uint16_t length, const void* value)
{
    auto* ssid = (char*)value;
    auto* password = (char*)value;        

    int index = 0;
    while(++index < length && *++password != 0);
    ++password;

    printf("Connect to wifi with: %s \ %s\n", ssid, password);
    memcpy(&tcp_host_address, (uint8_t*)value + (length - sizeof(tcp_host_address)), sizeof(tcp_host_address));
    sta.connect(ssid, password);
}
