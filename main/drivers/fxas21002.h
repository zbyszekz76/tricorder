#ifndef FXAS21002_H
#define FXAS21002_H

#include "esp_err.h"

esp_err_t fxas21002_init(void);

esp_err_t fxas21002_read_gyro(
    float *gx,
    float *gy,
    float *gz
);

#endif