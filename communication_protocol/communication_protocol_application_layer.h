#ifndef __COMMUNICATION_PROTOCOL_APPLICATION_LAYER__
#define __COMMUNICATION_PROTOCOL_APPLICATION_LAYER__

#include "communication_protocol_driver_layer.h"
#include "communication_protocol_interface_layer.h"
#include <stdint.h>
#include <stdio.h>

extern communication_protocol_handle_t g_communication_protocol_handle;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t communication_protocol_init(void);
uint8_t communication_protocol_cmd_set_val(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);
uint8_t communication_protocol_cmd_set_motor_enable(uint8_t ch_num);
uint8_t communication_protocol_cmd_set_motor_disable(uint8_t ch_num);
uint8_t communication_protocol_cmd_set_motor_reset(uint8_t ch_num);
uint8_t communication_protocol_cmd_set_motor_period(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);


#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATION_PROTOCOL_APPLICATION_LAYER__ */
