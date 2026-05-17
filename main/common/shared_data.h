#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdbool.h>
// =================================================
// ===================== IMU DATA ==================
// =================================================

typedef struct
{
    // ================= ACCEL =================

    float ax;
    float ay;
    float az;

    // ================= ANGLES =================

    float pitch;
    float roll;

    // ================= GYRO ==================

    float gx;
    float gy;
    float gz;

    // ================= INTEGRATED GYRO =======

    float gyro_roll;
    float gyro_pitch;

    char current_time[16];

    char current_date[16];

    char ip_address[16];

    int wifi_rssi;

    bool wifi_connected;

} imu_data_t;

// =================================================
// ===================== GLOBAL ====================
// =================================================

extern imu_data_t g_imu_data;

// extern bool wifi_connected;

// extern char ip_address[16];

// extern char current_time[16];

// extern char current_date[16];

// extern int wifi_rssi;

#endif