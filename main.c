#include <stdio.h>
#include <string.h>
#include "ztimer.h"

#include "hdc3020.h"
#include "hdc3020_params.h"

static hdc3020_t hdc3020[HDC3020_NUMOF];
typedef struct {
    uint8_t connected;
    double temperature;
    double humidity;
} hdc3020_data_t;
static hdc3020_data_t hdc3020_data[HDC3020_NUMOF];

int main(void)
{
    size_t i;
    double temp, hum;

    for (i = 0; i < HDC3020_NUMOF; i++) {
        hdc3020_data[i].connected = 0;
        if (hdc3020_init(&hdc3020[i], &hdc3020_params[i]) == HDC3020_OK) {
            if (hdc3020_set_auto_measurement_mode(&hdc3020[i], HDC3020_MPS_0_5_LPM_0) == HDC3020_OK) {
                hdc3020_data[i].connected = 1;
            } else {
                printf("[%d] set auto measure failed\n", i);
            }
        } else {
            printf("[%d] init failed\n", i);
        }
    }

    while(1) {
        for (i = 0; i < HDC3020_NUMOF; i++) {
            if (hdc3020_data[i].connected) {
                uint8_t valid = 0;
                if (hdc3020_auto_measurement_mode_read(&hdc3020[i], &temp, &hum) == HDC3020_OK) {
                    hdc3020_data[i].temperature = temp;
                    hdc3020_data[i].humidity = hum;
                    valid = 1;
                }
                printf(
                    "[%d] temp = %3.2f, hum = %3.2f %s\n", i,
                    hdc3020_data[i].temperature,
                    hdc3020_data[i].humidity,
                    valid ? "***" : ""
                );
            }
        }
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }
    return 0;
}
