#ifndef FXOS8700_H
#define FXOS8700_H

#include <esp_err.h>

esp_err_t fxos8700_init(void);

esp_err_t fxos8700_read_accel(
    float *ax,
    float *ay,
    float *az
);

#endif