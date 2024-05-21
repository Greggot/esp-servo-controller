set(BLE_SRC_PATH "../ble_plus_plus/src/Bluetooth/")
set(BLE_INC_PATH "../ble_plus_plus/inc/")
set(NETWORK_INC_PATH "../network/inc/")
set(NETWORK_SRC_PATH "../network/src/")
set(APPLICATION_INC_PATH "./inc/")

file(GLOB BLE_SRCS "${BLE_SRC_PATH}*.cpp")
file(GLOB SERVO_SRCS "./src/servo/*.cpp")

idf_component_register(SRCS "main.cpp"
                            ${BLE_SRCS}
                            "${NETWORK_SRC_PATH}/WiFi/STA.cpp"
                            ${SERVO_SRCS}
                    INCLUDE_DIRS 
                        ${BLE_INC_PATH} 
                        ${NETWORK_INC_PATH}
                        ${APPLICATION_INC_PATH})
