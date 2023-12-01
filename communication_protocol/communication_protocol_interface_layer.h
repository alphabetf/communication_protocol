#ifndef __COMMUNICATION_PROTOCOL_INTERFACE_LAYER__
#define __COMMUNICATION_PROTOCOL_INTERFACE_LAYER__

#include "communication_protocol_application_layer.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t communication_protocol_hal_init(void);
uint8_t communication_protocol_hal_deinit(void);
void communication_protocol_send_data_packet(uint8_t *data, uint16_t data_len);
void communication_protocol_debug_print(const char *const fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATION_PROTOCOL_INTERFACE_LAYER__ */
