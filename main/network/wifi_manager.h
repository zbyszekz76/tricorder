#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
// =================================================
// ===================== API =======================
// =================================================

void wifi_manager_init(void);

bool wifi_is_connected(void);

void wifi_update_time(void);

const char* wifi_get_ip(void);

int wifi_get_rssi(void);

#ifdef __cplusplus
}
#endif