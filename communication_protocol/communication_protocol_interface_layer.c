#include "communication_protocol_interface_layer.h"
#include "communication_protocol_driver_layer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include <stdarg.h>
#include <stdbool.h>   
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "boards.h"
#include "app_uart.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define UART_TX_BUF_SIZE 256       /* 串口发送缓存大小(字节数) */
#define UART_RX_BUF_SIZE 256       /* 串口接收缓存大小(字节数) */

/* 串口事件回调函数，该函数中判断事件类型并进行处理 */
static void uart_event_callback_handle(app_uart_evt_t * p_event)
{
    uint8_t dat;
	
	  /* 通讯错误事件 */
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR){
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    /* FIFO错误事件 */
    else if (p_event->evt_type == APP_UART_FIFO_ERROR){
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
	  /* 串口接收事件 */
    else if (p_event->evt_type == APP_UART_DATA_READY){
      //nrf_gpio_pin_toggle(LED_1);	   
      app_uart_get(&dat); /* 从FIFO中读取数据 */
			protocol_data_recv( &g_communication_protocol_handle, &dat, 1);	/* 将接收到的数据推入环形缓冲区中 */
	    printf("%c",dat);  /* 串口输出数据 */
    }
	  /* 串口发送完成事件 */
    else if (p_event->evt_type == APP_UART_TX_EMPTY){
        //nrf_gpio_pin_toggle(LED_2);
    }
}

/* 串口配置 */
static void uart_init(void)
{
	uint32_t err_code;
	
	/* 定义串口通讯参数配置结构体并初始化 */
  const app_uart_comm_params_t comm_params = {
    RX_PIN_NUMBER,	/* 定义uart接收引脚 */
    TX_PIN_NUMBER,	/* 定义uart发送引脚 */
    RTS_PIN_NUMBER, /* 定义uart RTS引脚 */
    CTS_PIN_NUMBER,	/* 定义uart CTS引脚 */
    APP_UART_FLOW_CONTROL_DISABLED, /* 关闭uart硬件流控,RTS和CTS引脚不会被使用 */
    false,	/* 禁止奇偶检验 */
    NRF_UART_BAUDRATE_115200	/* uart波特率设置为115200bps */
  };
  /* 初始化串口，注册串口事件回调函数 */
  APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_event_callback_handle,
                         APP_IRQ_PRIORITY_HIGH,
                         err_code);

  APP_ERROR_CHECK(err_code);
}


/* 底层通信接口初始化 */
uint8_t communication_protocol_hal_init(void)
{
		uart_init(); 	/* 初始化串口 */
	
		//app_uart_put('a');
	
		return 0;
}	

/* 底层通信接口解除初始化 */
uint8_t communication_protocol_hal_deinit(void)
{
		return app_uart_close();	/* 关闭串口 */
}                                     

/* 通过通信接口将数据包发送出去 */
void communication_protocol_send_data_packet(uint8_t *data, uint16_t data_len)
{
	uint8_t *buf = data;
	
	for(uint16_t i = 0; i < data_len; i++){
		app_uart_put(*buf);
		buf++;
	}
}	
	
/* log调试接口 */
void communication_protocol_debug_print(const char *const fmt, ...)
{
	  char str[256];
    //uint16_t len;
    va_list args;
    
    memset((char *)str, 0, sizeof(char) * 256); 
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);
    
    //len = strlen((char *)str);
	
    //(void)uart_write((uint8_t *)str, len);		/* 此处也可以调用串口将DEBUG信息输出到串口 */
		//NRF_LOG_HEXDUMP_INFO(NRF_LOG_PUSH(str), len)
		NRF_LOG_INFO("%s", NRF_LOG_PUSH(str));
}	
