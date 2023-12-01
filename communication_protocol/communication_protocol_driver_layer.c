#include "communication_protocol_driver_layer.h"
#include <string.h>
#include <stdlib.h>

/**
  * @brief ����У���
  * @param ptr����Ҫ���������
  * @param len����Ҫ����ĳ���
  * @retval У���
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
 * @brief   �õ�֡���ͣ�֡���
 * @param   *frame:  ����֡
 * @param   head_oft: ֡ͷ��ƫ��λ��
 * @return  ֡����.
 */
static uint8_t get_frame_type(uint8_t *frame, uint16_t head_oft)
{
    return (frame[(head_oft + CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

/**
 * @brief   �õ�֡����
 * @param   *buf:  ���ݻ�����.
 * @param   head_oft: ֡ͷ��ƫ��λ��
 * @return  ֡����.
 */
static uint16_t get_frame_len(uint8_t *frame, uint16_t head_oft)
{
    return ((frame[(head_oft + LEN_INDEX_VAL + 0) % PROT_FRAME_LEN_RECV] <<  0) |
            (frame[(head_oft + LEN_INDEX_VAL + 1) % PROT_FRAME_LEN_RECV] <<  8) |
            (frame[(head_oft + LEN_INDEX_VAL + 2) % PROT_FRAME_LEN_RECV] << 16) |
            (frame[(head_oft + LEN_INDEX_VAL + 3) % PROT_FRAME_LEN_RECV] << 24));    /* �ϳ�֡���� */
}

/**
 * @brief   ��ȡ crc-16 У��ֵ
 * @param   *frame:  ���ݻ�����.
 * @param   head_oft: ֡ͷ��ƫ��λ��
 * @param   head_oft: ֡��
 * @return  ֡����.
 */
static uint8_t get_frame_checksum(uint8_t *frame, uint16_t head_oft, uint16_t frame_len)
{
    return (frame[(head_oft + frame_len - 1) % PROT_FRAME_LEN_RECV]);
}

/**
 * @brief   ����֡ͷ
 * @param   *buf:  ���ݻ�����.
 * @param   ring_buf_len: ��������С
 * @param   start: ��ʼλ��
 * @param   len: ��Ҫ���ҵĳ���
 * @return  -1��û���ҵ�֡ͷ������ֵ��֡ͷ��λ��.
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
 * @brief   ����δ���������ݳ���
 * @param   *buf:  ���ݻ�����.
 * @param   ring_buf_len: ��������С
 * @param   start: ��ʼλ��
 * @param   end: ����λ��
 * @return  Ϊ���������ݳ���
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
 * @brief   ��������д�뻺����
 * @param   *buf:  ���ݻ�����.
 * @param   ring_buf_len: ��������С
 * @param   w_oft: дƫ��
 * @param   *data: ��Ҫд�������
 * @param   *data_len: ��Ҫд�����ݵĳ���
 * @return  void.
 */
static void recvbuf_put_data(uint8_t *buf, uint16_t ring_buf_len, uint16_t w_oft,
        uint8_t *data, uint16_t data_len)
{
    if ((w_oft + data_len) > ring_buf_len)               /* ����������β */
    {
        uint16_t data_len_part = ring_buf_len - w_oft;     /* ������ʣ�೤�� */

        /* ���ݷ�����д�뻺����*/
        memcpy(buf + w_oft, data, data_len_part);                         /* д�뻺����β */
        memcpy(buf, data + data_len_part, data_len - data_len_part);      /* д�뻺����ͷ */
    }
    else
        memcpy(buf + w_oft, data, data_len);    /* ����д�뻺���� */
}

/**
 * @brief   ��ѯ֡���ͣ����
 * @param   *data:  ֡����
 * @param   data_len: ֡���ݵĴ�С
 * @return  ֡���ͣ����.
 */
static uint8_t protocol_frame_parse(communication_protocol_handle_t *handle, uint8_t *data, uint16_t *data_len)
{
    uint8_t frame_type = CMD_NONE;
    uint16_t need_to_parse_len = 0;
    int16_t header_oft = -1;
    uint8_t checksum = 0;
    
    need_to_parse_len = recvbuf_get_len_to_parse(handle->parser->frame_len, PROT_FRAME_LEN_RECV, handle->parser->r_oft, handle->parser->w_oft);    /* �õ�δ���������ݳ��� */
    if (need_to_parse_len < 9){     /* �϶�������ͬʱ�ҵ�֡ͷ��֡���� */
        return frame_type;
		}
    /* ��δ�ҵ�֡ͷ����Ҫ���в��� */
    if (0 == handle->parser->found_frame_head){
        /* ͬ��ͷΪ���ֽڣ����ܴ���δ���������������һ���ֽڸպ�Ϊͬ��ͷ��һ���ֽڵ������
           ��˲���ͬ��ͷʱ�����һ���ֽڽ���������Ҳ���ᱻ���� */
        header_oft = recvbuf_find_header(handle->parser->recv_ptr, PROT_FRAME_LEN_RECV, handle->parser->r_oft, need_to_parse_len);
        if (0 <= header_oft){
            /* ���ҵ�֡ͷ */
            handle->parser->found_frame_head = 1;
            handle->parser->r_oft = header_oft;
          
            /* ȷ���Ƿ���Լ���֡�� */
            if (recvbuf_get_len_to_parse(handle->parser->frame_len, PROT_FRAME_LEN_RECV,
                    handle->parser->r_oft, handle->parser->w_oft) < 9)
                return frame_type;
        }else{
            /* δ��������������Ȼδ�ҵ�֡ͷ�������˴ν��������������� */
            handle->parser->r_oft = ((handle->parser->r_oft + need_to_parse_len - 3) % PROT_FRAME_LEN_RECV);
            return frame_type;
        }
    }
    
    /* ����֡������ȷ���Ƿ���Խ������ݽ��� */
    if (0 == handle->parser->frame_len){
        handle->parser->frame_len = get_frame_len(handle->parser->recv_ptr, handle->parser->r_oft);
        if(need_to_parse_len < handle->parser->frame_len)
            return frame_type;
    }

    /* ֡ͷλ��ȷ�ϣ���δ���������ݳ���֡�������Լ���У��� */
    if ((handle->parser->frame_len + handle->parser->r_oft - PROT_FRAME_LEN_CHECKSUM) > PROT_FRAME_LEN_RECV){
        /* ����֡����Ϊ�����֣�һ�����ڻ�����β��һ�����ڻ�����ͷ */
        checksum = check_sum(checksum, handle->parser->recv_ptr + handle->parser->r_oft, 
                PROT_FRAME_LEN_RECV - handle->parser->r_oft);
        checksum = check_sum(checksum, handle->parser->recv_ptr, handle->parser->frame_len -
                PROT_FRAME_LEN_CHECKSUM + handle->parser->r_oft - PROT_FRAME_LEN_RECV);
    }else{
        /* ����֡����һ����ȡ�� */
        checksum = check_sum(checksum, handle->parser->recv_ptr + handle->parser->r_oft, handle->parser->frame_len - PROT_FRAME_LEN_CHECKSUM);
    }

    if (checksum == get_frame_checksum(handle->parser->recv_ptr, handle->parser->r_oft, handle->parser->frame_len)){
        /* У��ɹ���������֡���� */
        if ((handle->parser->r_oft + handle->parser->frame_len) > PROT_FRAME_LEN_RECV){
            /* ����֡����Ϊ�����֣�һ�����ڻ�����β��һ�����ڻ�����ͷ */
            uint16_t data_len_part = PROT_FRAME_LEN_RECV - handle->parser->r_oft;
            memcpy(data, handle->parser->recv_ptr + handle->parser->r_oft, data_len_part);
            memcpy(data + data_len_part, handle->parser->recv_ptr, handle->parser->frame_len - data_len_part);
        }else{
            /* ����֡����һ����ȡ�� */
            memcpy(data, handle->parser->recv_ptr + handle->parser->r_oft, handle->parser->frame_len);
        }
        *data_len = handle->parser->frame_len;
        frame_type = get_frame_type(handle->parser->recv_ptr, handle->parser->r_oft);

        /* �����������е�����֡ */
        handle->parser->r_oft = (handle->parser->r_oft + handle->parser->frame_len) % PROT_FRAME_LEN_RECV;
    }else{
        /* У�����˵��֮ǰ�ҵ���֡ͷֻ��żȻ���ֵķ����� */
        handle->parser->r_oft = (handle->parser->r_oft + 1) % PROT_FRAME_LEN_RECV;
    }
    handle->parser->frame_len = 0;
    handle->parser->found_frame_head = 0;

    return frame_type;
}

/**
 * @brief   �������ݴ���
 * @param   *handle:  ͨ�ž��
 * @param   *data:  Ҫ��������ݵ�����.
 * @param   data_len: ���ݵĴ�С
 * @return  void.
 */
void protocol_data_recv(communication_protocol_handle_t *handle, uint8_t *data, uint16_t data_len)
{
    recvbuf_put_data(handle->parser->recv_ptr, PROT_FRAME_LEN_RECV, handle->parser->w_oft, data, data_len);    /* �������� */
    handle->parser->w_oft = (handle->parser->w_oft + data_len) % PROT_FRAME_LEN_RECV;                          /* ����дƫ�� */
}

/**
 * @brief ��ʼ��ͨ��Э��
 * @param *handle  ͨ��Э����
 * @return ��ʼ�����.
 */
int32_t protocol_init(communication_protocol_handle_t *handle)
{
		if(handle->inited == 1){	/* ͨ��Э���Ѿ���ʼ���� */
				return -1;
		}
	
		if (handle == NULL){  /* ��������� */
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
		
		handle->parser = (prot_frame_parser_t*)malloc(sizeof(prot_frame_parser_t));	/* �������ζ��� */
		if(handle->parser == NULL){	/* �ڴ����ʧ�� */
			 handle->debug_print("protocol_init: create ring queue fail.\n"); 	
			return -1;
		}
    memset(handle->parser, 0, sizeof(prot_frame_parser_t));
	
		handle->parser->recv_ptr = (uint8_t*)malloc(PROT_FRAME_LEN_RECV); /* �����ζ��л����������ڴ� */
		if(handle->parser->recv_ptr == NULL){	/* �ڴ����ʧ�� */
			 handle->debug_print("protocol_init: ring queue memory allocation failed.\n"); 	
			return -1;
		}
    memset(handle->parser->recv_ptr, 0, PROT_FRAME_LEN_RECV);
		
		if (handle->hal_init() != 0){	/* ��ʼ���ײ�ӿ� */ 
        handle->debug_print("protocol_init: hal_init failed.\n");    /* �ײ�ͨ�Žӿڳ�ʼ��ʧ�� */           
        return 1;                                                     
    }
		
		handle->inited = 1;				/* ��־��Э���Ѿ���ɳ�ʼ������ */
		
    return 0;
}

/**
 * @brief   ���ͨ��Э���ʼ��
 * @param *handle ͨ��Э����
 * @return  ���ʼ�����.
 */
int32_t protocol_deinit(communication_protocol_handle_t *handle)
{
		if (handle == NULL){	/* ����������� */
        return 2;                                                   
    }
	
		if(handle->inited == 0){	/* ͨ��Э�黹û����ʼ����������deinit */
				return 3;
		}
		/* ���ͷ�buf�ڴ滺���� */
		if(handle->parser->recv_ptr != NULL){		
			free(handle->parser->recv_ptr);
			handle->parser->recv_ptr = NULL;	/* ����ָ�� */
			handle->debug_print("protocol_deinit: ring queue buf memory has been release.\n"); 	
			return 0;
		}
		/* �ͷŻ��ζ��� */
		if(handle->parser != NULL){	
				free(handle->parser);
				handle->parser = NULL;	/* ����ָ�� */
				handle->debug_print("protocol_deinit: ring queue has been release.\n"); 	
				return 0;
		}
		
		if (handle->hal_deinit() != 0){	/* ͨ�Žӿڽ��ʼ�� */
        handle->debug_print("protocol_deinit: hal_deinit failed.\n");        
        return 1;                                                    
    }

		handle->inited = 0;				/* �ѽ����ʼ�� */
		
    return 0;
}

/**
 * @brief   ���յ����ݴ���
 * @param   *handle ͨ�ž��
 * @return  -1��û���ҵ�һ����ȷ������.
 */
int8_t receiving_process(communication_protocol_handle_t *handle)
{
  uint8_t 	frame_data[128];         /* Ҫ�ܷ������֡ */
  uint16_t 	frame_len = 0;           /* ֡���� */
  uint8_t 	cmd_type = CMD_NONE;     /* �������� */
  
  while(1)
  {
			cmd_type = protocol_frame_parse(handle, frame_data, &frame_len);
			switch (cmd_type){
					case CMD_NONE:{				/* δ����������ָ�� */
						return -1;
					}

					case SET_TARGET_CMD:{ /* ����Ŀ��ֵ */
						handle->debug_print("receiving_process: SET_TARGET_CMD.\n");  
						handle->cmd_set_val(frame_data[4],&frame_data[10],(frame_len-11)); /* ����Ŀ��ֵ */					
					}
					break;
     
					case START_CMD:{			/* ����ָ�� */
						handle->debug_print("receiving_process: START_CMD.\n");  
						handle->cmd_set_motor_enable(frame_data[4]); /* ������� */ 
					}
					break;
      
					case STOP_CMD:{				/* ָֹͣ�� */
						handle->debug_print("receiving_process: STOP_CMD.\n");  
						handle->cmd_set_motor_disable(frame_data[4]); /* ֹͣ��� */ 
					}
					break;
      
					case RESET_CMD:{			/* ��λָ�� */
						handle->debug_print("receiving_process: RESET_CMD.\n");  
						handle->cmd_set_motor_reset(frame_data[4]); 	/* ��λ��� */ 
					}
					break;
      
					case SET_PERIOD_CMD:{	/* �������� */
						handle->debug_print("receiving_process: SET_PERIOD_CMD.\n");  
						handle->cmd_set_motor_period(frame_data[4],&frame_data[10],(frame_len-11));		/* ���õ��Ƶ�� */
					}
					break;

					default: 
						return -1;
    }
  }
}

/**
  * @brief ������λ����ֵ
	* @param *handle:  ͨ�ž��
  * @param cmd������
  * @param ch: ����ͨ��
  * @param data������ָ��
  * @param num����������
  * @retval ��
  */
void set_computer_value(communication_protocol_handle_t *handle, uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    /* У��� */
  num *= 4;           /* һ������ 4 ���ֽ� */
  
  static packet_head_t set_packet;
  
  set_packet.head = FRAME_HEADER;    /* ��ͷ 0x59485A53 */
  set_packet.len  = 0x0B + num;      /* ���� */
  set_packet.ch   = ch;              /* ����ͨ�� */
  set_packet.cmd  = cmd;             /* �������� */
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       /* �����ͷУ��� */
  sum = check_sum(sum, (uint8_t *)data, num);                           /* �������У��� */
  
	handle->send_data_packet((uint8_t *)&set_packet, sizeof(set_packet)); /* ��������ͷ */
	handle->send_data_packet((uint8_t *)data, num);												/* ���Ͳ��� */
	handle->send_data_packet((uint8_t *)&sum, sizeof(sum)); 							/* ����У��� */
}
