#ifndef SHARED_DATA_H
#define SHARED_DATA_H

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

} imu_data_t;

// =================================================
// ===================== GLOBAL ====================
// =================================================

extern imu_data_t g_imu_data;

#endif