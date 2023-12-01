#ifndef __COMMUNICATION_PROTOCOL_DRIVER_LAYER__
#define __COMMUNICATION_PROTOCOL_DRIVER_LAYER__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ���ݽ��ջ��ζ��л�������С */
#define PROT_FRAME_LEN_RECV  128

/* У�����ݵĳ��� */
#define PROT_FRAME_LEN_CHECKSUM    1

/* ���͵����ݰ��ṹ�� */
typedef __packed struct packet_head_s
{
  uint32_t head;    /* ��ͷ */
  uint8_t  ch;      /* ͨ�� */
  uint32_t len;     /* ������ */
  uint8_t  cmd;     /* ���� */
//  uint8_t sum;    /* У��� */
}packet_head_t;

/* �ڲ����ζ��л������ṹ */
typedef struct prot_frame_parser_s			
{
    uint8_t *recv_ptr;  				/* ָ�����ݽ��ջ����� */
    uint16_t r_oft;							/* ���λ������Ķ�ָ�봦 */
    uint16_t w_oft;							/* ���λ�������дָ�봦 */
    uint16_t frame_len;					/* ��ǰ�����������ݰ�֡���� */
    uint16_t found_frame_head;	/* ���ڱ�ǵ�ǰ�Ƿ��ҵ����ݰ�֡ͷ */
}prot_frame_parser_t;

/* communication_protocol��� */
typedef struct communication_protocol_handle_s
{
		uint8_t inited;  																								 	/* ͨ��Э���Ƿ��Ѿ���ʼ�� */
		prot_frame_parser_t *parser;																		  /* ���������ݵĻ��ζ��о�� */
		uint8_t (*hal_init)(void);                                        /* �ײ�ͨ�Žӿڳ�ʼ�� */
		uint8_t (*hal_deinit)(void);                                      /* �ײ�ͨ�Žӿڽ����ʼ�� */
		void (*send_data_packet)(uint8_t *data, uint16_t data_len);			 	/* ͨ��ͨ�Žӿڽ����ݰ����ͳ�ȥ */
		void (*debug_print)(const char *const fmt, ...);                  /* log���Խӿ� */
		/* һЩָ��ܺ��� */
		uint8_t (*cmd_set_val)(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);						/* ����Ŀ��ֵ */
		uint8_t (*cmd_set_motor_enable)(uint8_t ch_num);	  																				/* ������� */
		uint8_t (*cmd_set_motor_disable)(uint8_t ch_num);																						/* ֹͣ��� */
		uint8_t (*cmd_set_motor_reset)(uint8_t ch_num);	  																					/* ��λ��� */
		uint8_t (*cmd_set_motor_period)(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);	  /* ���õ��Ƶ�� */
}communication_protocol_handle_t;

/* Ӳ������㺯��ע��� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_INIT(HANDLE, STRUCTURE)            memset(HANDLE, 0, sizeof(STRUCTURE))	/* ��ʼ��ͨ��Э���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_INIT(HANDLE, FUC)              (HANDLE)->hal_init = FUC							/* ע��hal_init���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_DEINIT(HANDLE, FUC)            (HANDLE)->hal_deinit = FUC						/* ע��hal_de_init���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_SEND_DATA_PACKET(HANDLE, FUC)      (HANDLE)->send_data_packet = FUC			/* ע��send_data_packet���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_DEBUG_PRINT(HANDLE, FUC)           (HANDLE)->debug_print = FUC						/* ע��debug_print���� */
/* ָ��ܺ���ע��� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_VAL(HANDLE, FUC)           (HANDLE)->cmd_set_val = FUC						/* ע��cmd_set_val���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_ENABLE(HANDLE, FUC)  (HANDLE)->cmd_set_motor_enable = FUC  /* ע��cmd_set_motor_enable���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_DISABLE(HANDLE, FUC) (HANDLE)->cmd_set_motor_disable = FUC /* ע��cmd_set_motor_disable���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_RESET(HANDLE, FUC)   (HANDLE)->cmd_set_motor_reset = FUC		/* ע��cmd_set_motor_reset���� */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_PERIOD(HANDLE, FUC)  (HANDLE)->cmd_set_motor_period = FUC	/* ע��cmd_set_motor_period���� */

#define FRAME_HEADER     0x59485A53    /* ֡ͷ */

/* ͨ���궨�� */
#define CURVES_CH1       0x01
#define CURVES_CH2       0x02
#define CURVES_CH3       0x03
#define CURVES_CH4       0x04
#define CURVES_CH5       0x05

/* ָ��(��λ�� -> ��λ��) */
#define SEND_TARGET_CMD      0x01     /* ������λ��ͨ����Ŀ��ֵ */
#define SEND_FACT_CMD        0x02     /* ����ͨ��ʵ��ֵ */
#define SEND_START_CMD       0x03     /* ��������ָ��(ͬ����λ����ť״̬) */
#define SEND_STOP_CMD        0x04     /* ����ָֹͣ��(ͬ����λ����ť״̬) */
#define SEND_PERIOD_CMD      0x05     /* ��������(ͬ����λ����ʾ��ֵ) */

/* ָ��(��λ�� -> ��λ��) */
#define SET_TARGET_CMD       0x11     /* ����Ŀ��ֵ */
#define START_CMD            0x12     /* ����ָ�� */
#define STOP_CMD             0x13     /* ָֹͣ�� */
#define RESET_CMD            0x14     /* ��λָ�� */
#define SET_PERIOD_CMD       0x15     /* �������� */

/* ��ָ�� */
#define CMD_NONE             0xFF     /* ��ָ�� */

/* ����ֵ�궨�� */
#define HEAD_INDEX_VAL       0x3u     /* ��ͷ����ֵ(4�ֽ�) */
#define CHX_INDEX_VAL        0x4u     /* ͨ������ֵ(1�ֽ�) */
#define LEN_INDEX_VAL        0x5u     /* ��������ֵ(4�ֽ�) */
#define CMD_INDEX_VAL        0x9u     /* ��������ֵ(1�ֽ�) */

/* ����һ����С���ֽ����໥ת���ĺ� */
#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     /* �����ߵ��ֽ� */

#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      /* �ϳ�Ϊһ���� */
																		 
/**
 * @brief   �������ݴ���
 * @param   *handle:  ͨ�ž��
 * @param   *data:  Ҫ��������ݵ�����.
 * @param   data_len: ���ݵĴ�С
 * @return  void.
 */
void protocol_data_recv(communication_protocol_handle_t *handle, uint8_t *data, uint16_t data_len);

/**
 * @brief   ���ͨ��Э���ʼ��
 * @param   ͨ��Э����
 * @return  ���ʼ�����.
 */
int32_t protocol_deinit(communication_protocol_handle_t *handle);

/**
 * @brief   ��ʼ������Э��
 * @param   ͨ��Э����
 * @return  ��ʼ�����.
 */
int32_t protocol_init(communication_protocol_handle_t *handle);

/**
 * @brief   ���յ����ݴ���
 * @param   *handle ͨ�ž��
 * @return  -1��û���ҵ�һ����ȷ������.
 */
int8_t receiving_process(communication_protocol_handle_t *handle);

/**
  * @brief ������λ����ֵ
	* @param *handle:  ͨ�ž��
  * @param cmd������
  * @param ch: ����ͨ��
  * @param data������ָ��
  * @param num����������
  * @retval ��
  */
void set_computer_value(communication_protocol_handle_t *handle, uint8_t cmd, uint8_t ch, void *data, uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATION_PROTOCOL_DRIVER_LAYER__ */
