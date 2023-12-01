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

#define UART_TX_BUF_SIZE 256       /* ���ڷ��ͻ����С(�ֽ���) */
#define UART_RX_BUF_SIZE 256       /* ���ڽ��ջ����С(�ֽ���) */

/* �����¼��ص��������ú������ж��¼����Ͳ����д��� */
static void uart_event_callback_handle(app_uart_evt_t * p_event)
{
    uint8_t dat;
	
	  /* ͨѶ�����¼� */
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR){
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    /* FIFO�����¼� */
    else if (p_event->evt_type == APP_UART_FIFO_ERROR){
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
	  /* ���ڽ����¼� */
    else if (p_event->evt_type == APP_UART_DATA_READY){
      //nrf_gpio_pin_toggle(LED_1);	   
      app_uart_get(&dat); /* ��FIFO�ж�ȡ���� */
			protocol_data_recv( &g_communication_protocol_handle, &dat, 1);	/* �����յ����������뻷�λ������� */
	    printf("%c",dat);  /* ����������� */
    }
	  /* ���ڷ�������¼� */
    else if (p_event->evt_type == APP_UART_TX_EMPTY){
        //nrf_gpio_pin_toggle(LED_2);
    }
}

/* �������� */
static void uart_init(void)
{
	uint32_t err_code;
	
	/* ���崮��ͨѶ�������ýṹ�岢��ʼ�� */
  const app_uart_comm_params_t comm_params = {
    RX_PIN_NUMBER,	/* ����uart�������� */
    TX_PIN_NUMBER,	/* ����uart�������� */
    RTS_PIN_NUMBER, /* ����uart RTS���� */
    CTS_PIN_NUMBER,	/* ����uart CTS���� */
    APP_UART_FLOW_CONTROL_DISABLED, /* �ر�uartӲ������,RTS��CTS���Ų��ᱻʹ�� */
    false,	/* ��ֹ��ż���� */
    NRF_UART_BAUDRATE_115200	/* uart����������Ϊ115200bps */
  };
  /* ��ʼ�����ڣ�ע�ᴮ���¼��ص����� */
  APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_event_callback_handle,
                         APP_IRQ_PRIORITY_HIGH,
                         err_code);

  APP_ERROR_CHECK(err_code);
}


/* �ײ�ͨ�Žӿڳ�ʼ�� */
uint8_t communication_protocol_hal_init(void)
{
		uart_init(); 	/* ��ʼ������ */
	
		//app_uart_put('a');
	
		return 0;
}	

/* �ײ�ͨ�Žӿڽ����ʼ�� */
uint8_t communication_protocol_hal_deinit(void)
{
		return app_uart_close();	/* �رմ��� */
}                                     

/* ͨ��ͨ�Žӿڽ����ݰ����ͳ�ȥ */
void communication_protocol_send_data_packet(uint8_t *data, uint16_t data_len)
{
	uint8_t *buf = data;
	
	for(uint16_t i = 0; i < data_len; i++){
		app_uart_put(*buf);
		buf++;
	}
}	
	
/* log���Խӿ� */
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
	
    //(void)uart_write((uint8_t *)str, len);		/* �˴�Ҳ���Ե��ô��ڽ�DEBUG��Ϣ��������� */
		//NRF_LOG_HEXDUMP_INFO(NRF_LOG_PUSH(str), len)
		NRF_LOG_INFO("%s", NRF_LOG_PUSH(str));
}	
