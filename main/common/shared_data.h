#ifndef SHARED_DATA_H
#define SHARED_DATA_H

typedef struct
{
    float ax;
    float ay;
    float az;
    float pitch;
    float roll;

} imu_data_t;

extern imu_data_t g_imu_data;

#endif // SHARED_DATA_H
