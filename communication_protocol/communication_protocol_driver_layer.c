#include "communication_protocol_driver_layer.h"
#include <string.h>
#include <stdlib.h>

/**
  * @brief 计算校验和
  * @param ptr：需要计算的数据
  * @param len：需要计算的长度
  * @retval 校验和
  */
static uint8_t check_sum(uint8_t init, uint8_t *ptr, uint8_t len )
{
  uint8_t sum = init;	
  
  while(len--)
  {
    sum += *ptr;
    ptr++;
  }
  
  return sum;
}

/**
 * @brief   得到帧类型（帧命令）
 * @param   *frame:  数据帧
 * @param   head_oft: 帧头的偏移位置
 * @return  帧长度.
 */
static uint8_t get_frame_type(uint8_t *frame, uint16_t head_oft)
{
    return (frame[(head_oft + CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

/**
 * @brief   得到帧长度
 * @param   *buf:  数据缓冲区.
 * @param   head_oft: 帧头的偏移位置
 * @return  帧长度.
 */
static uint16_t get_frame_len(uint8_t *frame, uint16_t head_oft)
{
    return ((frame[(head_oft + LEN_INDEX_VAL + 0) % PROT_FRAME_LEN_RECV] <<  0) |
            (frame[(head_oft + LEN_INDEX_VAL + 1) % PROT_FRAME_LEN_RECV] <<  8) |
            (frame[(head_oft + LEN_INDEX_VAL + 2) % PROT_FRAME_LEN_RECV] << 16) |
            (frame[(head_oft + LEN_INDEX_VAL + 3) % PROT_FRAME_LEN_RECV] << 24));    /* 合成帧长度 */
}

/**
 * @brief   获取 crc-16 校验值
 * @param   *frame:  数据缓冲区.
 * @param   head_oft: 帧头的偏移位置
 * @param   head_oft: 帧长
 * @return  帧长度.
 */
static uint8_t get_frame_checksum(uint8_t *frame, uint16_t head_oft, uint16_t frame_len)
{
    return (frame[(head_oft + frame_len - 1) % PROT_FRAME_LEN_RECV]);
}

/**
 * @brief   查找帧头
 * @param   *buf:  数据缓冲区.
 * @param   ring_buf_len: 缓冲区大小
 * @param   start: 起始位置
 * @param   len: 需要查找的长度
 * @return  -1：没有找到帧头，其他值：帧头的位置.
 */
static int32_t recvbuf_find_header(uint8_t *buf, uint16_t ring_buf_len, uint16_t start, uint16_t len)
{
    uint16_t i = 0;

    for (i = 0; i < (len - 3); i++)
    {
        if (((buf[(start + i + 0) % ring_buf_len] <<  0) |
             (buf[(start + i + 1) % ring_buf_len] <<  8) |
             (buf[(start + i + 2) % ring_buf_len] << 16) |
             (buf[(start + i + 3) % ring_buf_len] << 24)) == FRAME_HEADER)
        {
            return ((start + i) % ring_buf_len);
        }
    }
    return -1;
}

/**
 * @brief   计算未解析的数据长度
 * @param   *buf:  数据缓冲区.
 * @param   ring_buf_len: 缓冲区大小
 * @param   start: 起始位置
 * @param   end: 结束位置
 * @return  为解析的数据长度
 */
static int32_t recvbuf_get_len_to_parse(uint16_t frame_len, uint16_t ring_buf_len,uint16_t start, uint16_t end)
{
    uint16_t unparsed_data_len = 0;

    if (start <= end)
        unparsed_data_len = end - start;
    else
        unparsed_data_len = ring_buf_len - start + end;

    if (frame_len > unparsed_data_len)
        return 0;
    else
        return unparsed_data_len;
}

/**
 * @brief   接收数据写入缓冲区
 * @param   *buf:  数据缓冲区.
 * @param   ring_buf_len: 缓冲区大小
 * @param   w_oft: 写偏移
 * @param   *data: 需要写入的数据
 * @param   *data_len: 需要写入数据的长度
 * @return  void.
 */
static void recvbuf_put_data(uint8_t *buf, uint16_t ring_buf_len, uint16_t w_oft,
        uint8_t *data, uint16_t data_len)
{
    if ((w_oft + data_len) > ring_buf_len)               /* 超过缓冲区尾 */
    {
        uint16_t data_len_part = ring_buf_len - w_oft;     /* 缓冲区剩余长度 */

        /* 数据分两段写入缓冲区*/
        memcpy(buf + w_oft, data, data_len_part);                         /* 写入缓冲区尾 */
        memcpy(buf, data + data_len_part, data_len - data_len_part);      /* 写入缓冲区头 */
    }
    else
        memcpy(buf + w_oft, data, data_len);    /* 数据写入缓冲区 */
}

/**
 * @brief   查询帧类型（命令）
 * @param   *data:  帧数据
 * @param   data_len: 帧数据的大小
 * @return  帧类型（命令）.
 */
static uint8_t protocol_frame_parse(communication_protocol_handle_t *handle, uint8_t *data, uint16_t *data_len)
{
    uint8_t frame_type = CMD_NONE;
    uint16_t need_to_parse_len = 0;
    int16_t header_oft = -1;
    uint8_t checksum = 0;
    
    need_to_parse_len = recvbuf_get_len_to_parse(handle->parser->frame_len, PROT_FRAME_LEN_RECV, handle->parser->r_oft, handle->parser->w_oft);    /* 得到未解析的数据长度 */
    if (need_to_parse_len < 9){     /* 肯定还不能同时找到帧头和帧长度 */
        return frame_type;
		}
    /* 还未找到帧头，需要进行查找 */
    if (0 == handle->parser->found_frame_head){
        /* 同步头为四字节，可能存在未解析的数据中最后一个字节刚好为同步头第一个字节的情况，
           因此查找同步头时，最后一个字节将不解析，也不会被丢弃 */
        header_oft = recvbuf_find_header(handle->parser->recv_ptr, PROT_FRAME_LEN_RECV, handle->parser->r_oft, need_to_parse_len);
        if (0 <= header_oft){
            /* 已找到帧头 */
            handle->parser->found_frame_head = 1;
            handle->parser->r_oft = header_oft;
          
            /* 确认是否可以计算帧长 */
            if (recvbuf_get_len_to_parse(handle->parser->frame_len, PROT_FRAME_LEN_RECV,
                    handle->parser->r_oft, handle->parser->w_oft) < 9)
                return frame_type;
        }else{
            /* 未解析的数据中依然未找到帧头，丢掉此次解析过的所有数据 */
            handle->parser->r_oft = ((handle->parser->r_oft + need_to_parse_len - 3) % PROT_FRAME_LEN_RECV);
            return frame_type;
        }
    }
    
    /* 计算帧长，并确定是否可以进行数据解析 */
    if (0 == handle->parser->frame_len){
        handle->parser->frame_len = get_frame_len(handle->parser->recv_ptr, handle->parser->r_oft);
        if(need_to_parse_len < handle->parser->frame_len)
            return frame_type;
    }

    /* 帧头位置确认，且未解析的数据超过帧长，可以计算校验和 */
    if ((handle->parser->frame_len + handle->parser->r_oft - PROT_FRAME_LEN_CHECKSUM) > PROT_FRAME_LEN_RECV){
        /* 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头 */
        checksum = check_sum(checksum, handle->parser->recv_ptr + handle->parser->r_oft, 
                PROT_FRAME_LEN_RECV - handle->parser->r_oft);
        checksum = check_sum(checksum, handle->parser->recv_ptr, handle->parser->frame_len -
                PROT_FRAME_LEN_CHECKSUM + handle->parser->r_oft - PROT_FRAME_LEN_RECV);
    }else{
        /* 数据帧可以一次性取完 */
        checksum = check_sum(checksum, handle->parser->recv_ptr + handle->parser->r_oft, handle->parser->frame_len - PROT_FRAME_LEN_CHECKSUM);
    }

    if (checksum == get_frame_checksum(handle->parser->recv_ptr, handle->parser->r_oft, handle->parser->frame_len)){
        /* 校验成功，拷贝整帧数据 */
        if ((handle->parser->r_oft + handle->parser->frame_len) > PROT_FRAME_LEN_RECV){
            /* 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头 */
            uint16_t data_len_part = PROT_FRAME_LEN_RECV - handle->parser->r_oft;
            memcpy(data, handle->parser->recv_ptr + handle->parser->r_oft, data_len_part);
            memcpy(data + data_len_part, handle->parser->recv_ptr, handle->parser->frame_len - data_len_part);
        }else{
            /* 数据帧可以一次性取完 */
            memcpy(data, handle->parser->recv_ptr + handle->parser->r_oft, handle->parser->frame_len);
        }
        *data_len = handle->parser->frame_len;
        frame_type = get_frame_type(handle->parser->recv_ptr, handle->parser->r_oft);

        /* 丢弃缓冲区中的命令帧 */
        handle->parser->r_oft = (handle->parser->r_oft + handle->parser->frame_len) % PROT_FRAME_LEN_RECV;
    }else{
        /* 校验错误，说明之前找到的帧头只是偶然出现的废数据 */
        handle->parser->r_oft = (handle->parser->r_oft + 1) % PROT_FRAME_LEN_RECV;
    }
    handle->parser->frame_len = 0;
    handle->parser->found_frame_head = 0;

    return frame_type;
}

/**
 * @brief   接收数据处理
 * @param   *handle:  通信句柄
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void protocol_data_recv(communication_protocol_handle_t *handle, uint8_t *data, uint16_t data_len)
{
    recvbuf_put_data(handle->parser->recv_ptr, PROT_FRAME_LEN_RECV, handle->parser->w_oft, data, data_len);    /* 接收数据 */
    handle->parser->w_oft = (handle->parser->w_oft + data_len) % PROT_FRAME_LEN_RECV;                          /* 计算写偏移 */
}

/**
 * @brief 初始化通信协议
 * @param *handle  通信协议句柄
 * @return 初始化结果.
 */
int32_t protocol_init(communication_protocol_handle_t *handle)
{
		if(handle->inited == 1){	/* 通信协议已经初始化了 */
				return -1;
		}
	
		if (handle == NULL){  /* 句柄不存在 */
        return 2;       
    }
		
		if (handle->debug_print == NULL){  
				return 3;               
    }
		
		if (handle->hal_init == NULL){
        handle->debug_print("protocol_init: hal_init is null.\n");                    
        return 3;                                                            
    }
		
		if (handle->hal_deinit == NULL){
        handle->debug_print("protocol_init: hal_deinit is null.\n");               
        return 3;                                                          
    }
		
		if (handle->send_data_packet == NULL){
        handle->debug_print("protocol_init: send_data_packet is null.\n");           
        return 3;                                                        
    }
		
		handle->parser = (prot_frame_parser_t*)malloc(sizeof(prot_frame_parser_t));	/* 创建环形队列 */
		if(handle->parser == NULL){	/* 内存分配失败 */
			 handle->debug_print("protocol_init: create ring queue fail.\n"); 	
			return -1;
		}
    memset(handle->parser, 0, sizeof(prot_frame_parser_t));
	
		handle->parser->recv_ptr = (uint8_t*)malloc(PROT_FRAME_LEN_RECV); /* 给环形队列缓冲区分配内存 */
		if(handle->parser->recv_ptr == NULL){	/* 内存分配失败 */
			 handle->debug_print("protocol_init: ring queue memory allocation failed.\n"); 	
			return -1;
		}
    memset(handle->parser->recv_ptr, 0, PROT_FRAME_LEN_RECV);
		
		if (handle->hal_init() != 0){	/* 初始化底层接口 */ 
        handle->debug_print("protocol_init: hal_init failed.\n");    /* 底层通信接口初始化失败 */           
        return 1;                                                     
    }
		
		handle->inited = 1;				/* 标志该协议已经完成初始化工作 */
		
    return 0;
}

/**
 * @brief   解除通信协议初始化
 * @param *handle 通信协议句柄
 * @return  解初始化结果.
 */
int32_t protocol_deinit(communication_protocol_handle_t *handle)
{
		if (handle == NULL){	/* 传入参数错误 */
        return 2;                                                   
    }
	
		if(handle->inited == 0){	/* 通信协议还没被初始化过，无需deinit */
				return 3;
		}
		/* 先释放buf内存缓冲区 */
		if(handle->parser->recv_ptr != NULL){		
			free(handle->parser->recv_ptr);
			handle->parser->recv_ptr = NULL;	/* 重置指向 */
			handle->debug_print("protocol_deinit: ring queue buf memory has been release.\n"); 	
			return 0;
		}
		/* 释放环形队列 */
		if(handle->parser != NULL){	
				free(handle->parser);
				handle->parser = NULL;	/* 重置指向 */
				handle->debug_print("protocol_deinit: ring queue has been release.\n"); 	
				return 0;
		}
		
		if (handle->hal_deinit() != 0){	/* 通信接口解初始化 */
        handle->debug_print("protocol_deinit: hal_deinit failed.\n");        
        return 1;                                                    
    }

		handle->inited = 0;				/* 已解除初始化 */
		
    return 0;
}

/**
 * @brief   接收的数据处理
 * @param   *handle 通信句柄
 * @return  -1：没有找到一个正确的命令.
 */
int8_t receiving_process(communication_protocol_handle_t *handle)
{
  uint8_t 	frame_data[128];         /* 要能放下最长的帧 */
  uint16_t 	frame_len = 0;           /* 帧长度 */
  uint8_t 	cmd_type = CMD_NONE;     /* 命令类型 */
  
  while(1)
  {
			cmd_type = protocol_frame_parse(handle, frame_data, &frame_len);
			switch (cmd_type){
					case CMD_NONE:{				/* 未解析到完整指令 */
						return -1;
					}

					case SET_TARGET_CMD:{ /* 设置目标值 */
						handle->debug_print("receiving_process: SET_TARGET_CMD.\n");  
						handle->cmd_set_val(frame_data[4],&frame_data[10],(frame_len-11)); /* 设置目标值 */					
					}
					break;
     
					case START_CMD:{			/* 启动指令 */
						handle->debug_print("receiving_process: START_CMD.\n");  
						handle->cmd_set_motor_enable(frame_data[4]); /* 启动电机 */ 
					}
					break;
      
					case STOP_CMD:{				/* 停止指令 */
						handle->debug_print("receiving_process: STOP_CMD.\n");  
						handle->cmd_set_motor_disable(frame_data[4]); /* 停止电机 */ 
					}
					break;
      
					case RESET_CMD:{			/* 复位指令 */
						handle->debug_print("receiving_process: RESET_CMD.\n");  
						handle->cmd_set_motor_reset(frame_data[4]); 	/* 复位电机 */ 
					}
					break;
      
					case SET_PERIOD_CMD:{	/* 设置周期 */
						handle->debug_print("receiving_process: SET_PERIOD_CMD.\n");  
						handle->cmd_set_motor_period(frame_data[4],&frame_data[10],(frame_len-11));		/* 设置电机频率 */
					}
					break;

					default: 
						return -1;
    }
  }
}

/**
  * @brief 设置上位机的值
	* @param *handle:  通信句柄
  * @param cmd：命令
  * @param ch: 曲线通道
  * @param data：参数指针
  * @param num：参数个数
  * @retval 无
  */
void set_computer_value(communication_protocol_handle_t *handle, uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    /* 校验和 */
  num *= 4;           /* 一个参数 4 个字节 */
  
  static packet_head_t set_packet;
  
  set_packet.head = FRAME_HEADER;    /* 包头 0x59485A53 */
  set_packet.len  = 0x0B + num;      /* 包长 */
  set_packet.ch   = ch;              /* 设置通道 */
  set_packet.cmd  = cmd;             /* 设置命令 */
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       /* 计算包头校验和 */
  sum = check_sum(sum, (uint8_t *)data, num);                           /* 计算参数校验和 */
  
	handle->send_data_packet((uint8_t *)&set_packet, sizeof(set_packet)); /* 发送数据头 */
	handle->send_data_packet((uint8_t *)data, num);												/* 发送参数 */
	handle->send_data_packet((uint8_t *)&sum, sizeof(sum)); 							/* 发送校验和 */
}
