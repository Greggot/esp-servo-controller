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

#include <SkyBlue/Device.hpp>
#include <SkyBlue/Collector.hpp>
#include <SkyBlue/InterfaceTraits.hpp>

#include "nvs_flash.h"
#include "esp_log.h"

static const char* TAG = "app";
extern "C" { void app_main(void); }

template<class Address_t>
void output_address(const char* prefix, const Address_t& address) {
    ESP_LOGI(TAG, "%s: %u.%u.%u.%u\n", prefix, address.IP[0], address.IP[1], address.IP[2], address.IP[3]);
}

static SkyBlue::TCPserverDevice device;
void skyblue_init(WiFi::STA&);

void app_main(void)
{
    nvs_flash_init();

    static WiFi::STA sta;
    static TCP::Address tcp_host_address;
    static BLE::Characteristic wifi_characteristic(0x4ED, BLE::permition::Write, BLE::property::Write | BLE::property::Notify);
    static BLE::Service connectivity_service(0xC111E1A, {&wifi_characteristic});
    wifi_characteristic.setWriteCallback([](BLE::Characteristic*, const uint16_t length, const void* value)
    {
        auto* ssid = (char*)value;
        auto* password = (char*)value;        

        int index = 0;
        while(++index < length && *++password != 0);
        ++password;

        ESP_LOGI(TAG, "Connect to wifi with: %s: %s\n", ssid, password);
        memcpy(&tcp_host_address, (uint8_t*)value + (length - sizeof(tcp_host_address)), sizeof(tcp_host_address));
        sta.connect(ssid, password);
    });

    sta.add(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
    [](void*, esp_event_base_t, int32_t, void*){
        if(BLE::Server::Status())
            wifi_characteristic.Notify(int(1));
        else
            sta.TurnOffFullPower();
        BLE::Server::Enable();
    });

    sta.add(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED,
    [](void*, esp_event_base_t, int32_t, void*){
        wifi_characteristic.Notify(int(0));
    });

    sta.add(IP_EVENT, IP_EVENT_STA_GOT_IP, [](void*, esp_event_base_t, int32_t, void* data){        
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
        tcp_host_address = {event->ip_info.ip.addr, 4444};
        output_address("Starting TCP server (4444) on: ", tcp_host_address);

        BLE::Server::Disable();
        WiFi::STA::TurnOnFullPower();

        // Start server, begin work after first connection
        device.connect(tcp_host_address);
        device.listen();
        auto client = device.otherside();
        output_address("Client connected: ", client);
    });
    ESP_LOGI(TAG, "WiFi initialized");

    static BLE::Characteristic omega_rotor_characteristic(0xA0111, BLE::permition::Write, BLE::property::Write);
    static BLE::Characteristic theta_rotor_characteristic(0xA111E, BLE::permition::Write, BLE::property::Write);
    static BLE::Service rotor_service (0xCFFFAE, {&omega_rotor_characteristic, &theta_rotor_characteristic});

    static BLE::Server BLE_Device("BTservoController", {&rotor_service, &connectivity_service});
    BLE::Server::Enable();
    ESP_LOGI(TAG, "BLE initialized");

    skyblue_init(sta);
    ESP_LOGI(TAG, "Application layer TCP protocol initialized");
}

void skyblue_init(WiFi::STA& sta)
{
    static SkyBlue::Module rotormodule;
    rotormodule.setWrite([](const SkyBlue::ID& id, const void* data, size_t){
        struct vertex{
            float x; float y; float z;
        };
        vertex input;
        memcpy(&input, data, sizeof(vertex));
        ESP_LOGI(TAG, "Rotor %u received vertex x:%.2f, y:%.2f, z:%.2f", id.number, 
            input.x, input.y, input.z);
    });


    static SkyBlue::Module wifi_module;
    wifi_module.setWrite([&sta](const SkyBlue::ID&, const void*, size_t){
        ESP_LOGI(TAG, "WiFi disconnection triggered");
        // device.disconnect();
        // sta.disconnect();
        // sta.TurnOffFullPower();
        // BLE::Server::Enable();
        // device.deaf();
        esp_restart();
    });

    device.add({0, SkyBlue::type_t::steady}, &wifi_module);
    device.add({0, SkyBlue::type_t::rotorservo}, &rotormodule);
    device.add({1, SkyBlue::type_t::rotorservo},& rotormodule);
}
