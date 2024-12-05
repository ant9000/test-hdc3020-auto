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
    uint8_t status;
    double temp, hum;
    uint16_t status_reg;

    for (i = 0; i < HDC3020_NUMOF; i++) {
        hdc3020_data[i].connected = 0;
        if (hdc3020_init(&hdc3020[i], &hdc3020_params[i]) == HDC3020_OK) {
            hdc3020_data[i].connected = 1;
            hdc3020_deactivate_alert(&hdc3020[i]);
#if AUTO_MEASURE
            status = hdc3020_set_auto_measurement_mode(&hdc3020[i], HDC3020_MPS_0_5_LPM_0);
            printf("[%d] set auto measure %s\n", i, status == HDC3020_OK ? "ok" : "failed");
#endif
        } else {
            printf("[%d] init failed\n", i);
        }
    }

    while(1) {
        for (i = 0; i < HDC3020_NUMOF; i++) {
            if (hdc3020_data[i].connected) {
#if AUTO_MEASURE
                status = hdc3020_auto_measurement_mode_read(&hdc3020[i], &temp, &hum);
#else
                status = hdc3020_read(&hdc3020[i], &temp, &hum);
#endif
                hdc3020_get_status(&hdc3020[i], &status_reg);

                if (status == HDC3020_OK) {
                    hdc3020_data[i].temperature = temp;
                    hdc3020_data[i].humidity = hum;
                }
                printf(
                    "[%d] status = 0x%04x, temp = %3.2f, hum = %3.2f %s\n",
                    i,
                    status_reg,
                    hdc3020_data[i].temperature,
                    hdc3020_data[i].humidity,
                    status == HDC3020_OK ? "***" : ""
                );

            }
        }
        ztimer_sleep(ZTIMER_MSEC, SLEEP_SECS * 1000);
    }
    return 0;
}
