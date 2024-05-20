/**
 * @file
 * @brief  Входная точка приложения
 * @author https://github.com/Greggot/
 */

#include <freertos/FreeRTOS.h>

void app_main(void)
{
    while(true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
