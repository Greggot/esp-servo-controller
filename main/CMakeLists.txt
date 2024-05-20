set(BLE_SRC_PATH "../ble_plus_plus/src/Bluetooth/")
set(BLE_INC_PATH "../ble_plus_plus/inc/")
set(NETWORK_INC_PATH "../network/inc/")
set(NETWORK_SRC_PATH "../network/src/")
idf_component_register(SRCS "main.cpp"
                            "${BLE_SRC_PATH}/Characteristic.cpp" 
                            "${BLE_SRC_PATH}/Server.cpp"
                            "${BLE_SRC_PATH}/Service.cpp"
                            "${NETWORK_SRC_PATH}/WiFi/STA.cpp"
                    INCLUDE_DIRS 
                        ${BLE_INC_PATH} 
                        ${NETWORK_INC_PATH})
