#ifndef __COMMUNICATION_PROTOCOL_DRIVER_LAYER__
#define __COMMUNICATION_PROTOCOL_DRIVER_LAYER__

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 数据接收环形队列缓冲区大小 */
#define PROT_FRAME_LEN_RECV  128

/* 校验数据的长度 */
#define PROT_FRAME_LEN_CHECKSUM    1

/* 发送的数据包结构体 */
typedef __packed struct packet_head_s
{
  uint32_t head;    /* 包头 */
  uint8_t  ch;      /* 通道 */
  uint32_t len;     /* 包长度 */
  uint8_t  cmd;     /* 命令 */
//  uint8_t sum;    /* 校验和 */
}packet_head_t;

/* 内部环形队列缓冲区结构 */
typedef struct prot_frame_parser_s			
{
    uint8_t *recv_ptr;  				/* 指向数据接收缓冲区 */
    uint16_t r_oft;							/* 环形缓冲区的读指针处 */
    uint16_t w_oft;							/* 环形缓冲区的写指针处 */
    uint16_t frame_len;					/* 当前解析出的数据包帧长度 */
    uint16_t found_frame_head;	/* 用于标记当前是否找到数据包帧头 */
}prot_frame_parser_t;

/* communication_protocol句柄 */
typedef struct communication_protocol_handle_s
{
		uint8_t inited;  																								 	/* 通信协议是否已经初始化 */
		prot_frame_parser_t *parser;																		  /* 待解析数据的环形队列句柄 */
		uint8_t (*hal_init)(void);                                        /* 底层通信接口初始化 */
		uint8_t (*hal_deinit)(void);                                      /* 底层通信接口解除初始化 */
		void (*send_data_packet)(uint8_t *data, uint16_t data_len);			 	/* 通过通信接口将数据包发送出去 */
		void (*debug_print)(const char *const fmt, ...);                  /* log调试接口 */
		/* 一些指令功能函数 */
		uint8_t (*cmd_set_val)(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);						/* 设置目标值 */
		uint8_t (*cmd_set_motor_enable)(uint8_t ch_num);	  																				/* 启动电机 */
		uint8_t (*cmd_set_motor_disable)(uint8_t ch_num);																						/* 停止电机 */
		uint8_t (*cmd_set_motor_reset)(uint8_t ch_num);	  																					/* 复位电机 */
		uint8_t (*cmd_set_motor_period)(uint8_t ch_num, uint8_t* data_params, uint16_t data_len);	  /* 设置电机频率 */
}communication_protocol_handle_t;

/* 硬件抽象层函数注册宏 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_INIT(HANDLE, STRUCTURE)            memset(HANDLE, 0, sizeof(STRUCTURE))	/* 初始化通信协议句柄 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_INIT(HANDLE, FUC)              (HANDLE)->hal_init = FUC							/* 注册hal_init函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_DEINIT(HANDLE, FUC)            (HANDLE)->hal_deinit = FUC						/* 注册hal_de_init函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_SEND_DATA_PACKET(HANDLE, FUC)      (HANDLE)->send_data_packet = FUC			/* 注册send_data_packet函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_DEBUG_PRINT(HANDLE, FUC)           (HANDLE)->debug_print = FUC						/* 注册debug_print函数 */
/* 指令功能函数注册宏 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_VAL(HANDLE, FUC)           (HANDLE)->cmd_set_val = FUC						/* 注册cmd_set_val函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_ENABLE(HANDLE, FUC)  (HANDLE)->cmd_set_motor_enable = FUC  /* 注册cmd_set_motor_enable函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_DISABLE(HANDLE, FUC) (HANDLE)->cmd_set_motor_disable = FUC /* 注册cmd_set_motor_disable函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_RESET(HANDLE, FUC)   (HANDLE)->cmd_set_motor_reset = FUC		/* 注册cmd_set_motor_reset函数 */
#define DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_PERIOD(HANDLE, FUC)  (HANDLE)->cmd_set_motor_period = FUC	/* 注册cmd_set_motor_period函数 */

#define FRAME_HEADER     0x59485A53    /* 帧头 */

/* 通道宏定义 */
#define CURVES_CH1       0x01
#define CURVES_CH2       0x02
#define CURVES_CH3       0x03
#define CURVES_CH4       0x04
#define CURVES_CH5       0x05

/* 指令(下位机 -> 上位机) */
#define SEND_TARGET_CMD      0x01     /* 发送上位机通道的目标值 */
#define SEND_FACT_CMD        0x02     /* 发送通道实际值 */
#define SEND_START_CMD       0x03     /* 发送启动指令(同步上位机按钮状态) */
#define SEND_STOP_CMD        0x04     /* 发送停止指令(同步上位机按钮状态) */
#define SEND_PERIOD_CMD      0x05     /* 发送周期(同步上位机显示的值) */

/* 指令(上位机 -> 下位机) */
#define SET_TARGET_CMD       0x11     /* 设置目标值 */
#define START_CMD            0x12     /* 启动指令 */
#define STOP_CMD             0x13     /* 停止指令 */
#define RESET_CMD            0x14     /* 复位指令 */
#define SET_PERIOD_CMD       0x15     /* 设置周期 */

/* 空指令 */
#define CMD_NONE             0xFF     /* 空指令 */

/* 索引值宏定义 */
#define HEAD_INDEX_VAL       0x3u     /* 包头索引值(4字节) */
#define CHX_INDEX_VAL        0x4u     /* 通道索引值(1字节) */
#define LEN_INDEX_VAL        0x5u     /* 包长索引值(4字节) */
#define CMD_INDEX_VAL        0x9u     /* 命令索引值(1字节) */

/* 这是一个大小端字节序相互转换的宏 */
#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     /* 交换高低字节 */

#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      /* 合成为一个字 */
																		 
/**
 * @brief   接收数据处理
 * @param   *handle:  通信句柄
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void protocol_data_recv(communication_protocol_handle_t *handle, uint8_t *data, uint16_t data_len);

/**
 * @brief   解除通信协议初始化
 * @param   通信协议句柄
 * @return  解初始化结果.
 */
int32_t protocol_deinit(communication_protocol_handle_t *handle);

/**
 * @brief   初始化接收协议
 * @param   通信协议句柄
 * @return  初始化结果.
 */
int32_t protocol_init(communication_protocol_handle_t *handle);

/**
 * @brief   接收的数据处理
 * @param   *handle 通信句柄
 * @return  -1：没有找到一个正确的命令.
 */
int8_t receiving_process(communication_protocol_handle_t *handle);

/**
  * @brief 设置上位机的值
	* @param *handle:  通信句柄
  * @param cmd：命令
  * @param ch: 曲线通道
  * @param data：参数指针
  * @param num：参数个数
  * @retval 无
  */
void set_computer_value(communication_protocol_handle_t *handle, uint8_t cmd, uint8_t ch, void *data, uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATION_PROTOCOL_DRIVER_LAYER__ */
