#include "communication_protocol_application_layer.h"
#include "nrf_log.h"
#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "string.h"
#include "app_timer.h"
#include "nrf_drv_pwm.h"
#include "nrf_drv_gpiote.h"

communication_protocol_handle_t g_communication_protocol_handle;        /* ȫ��ͨ�ž�� */
APP_TIMER_DEF(communication_protocol_timer_id);							 						/* ͨ��Э�鶨ʱ����� */	

/* PWM��������ʵ��ID,0:PWM0 1:PWM1 2:PWM2 */
#define PWM_INSTANCE  0 
/* ������������������ */
#define MOTOR_PULSE_PIN	17		
/* �������������� */
#define MOTOR_DIR_PIN	18		
/* ���ʹ�ܿ������� */
#define MOTOR_EN_PIN	19	
/* �ⲿ�жϴ������� */
#define EXTERN_INT_PIN 	8
/* ת��ÿתһȦ����ĵ��Ȧ�� */
#define GEAR_RATIO		18
/* ���ÿתһȦ����������� */
#define NUM_PULSE_PER_REVOLUTION	2000
/* �ܸ��ӵ����� */
#define NUM_OF_GRIDS	20

static nrfx_pwm_t motor_pwm0 = NRFX_PWM_INSTANCE(PWM_INSTANCE); /* PWM0 */
static nrf_pwm_values_common_t motor_pwm_seq_values[1] = {400}; 	/* ����PWM����(ͨ��װ��ģʽ),�����б���λ��RAM��,���Ҫ����Ϊstatic���͵� */
static bool motor_has_been_reset = false;											  /* ����Ƿ��Ѿ���λ */
static uint8_t current_grids_pos = 0;														/* ���ڱ�־ת�̵�ǰλ�� */

//GPIOTE�¼������ص��������¼��ص�����������Ի�ȡpin��ź����Ŷ���
void external_interrupt_in_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	  if(pin == EXTERN_INT_PIN){ /* ȷ���жϴ��������� */
			if(motor_has_been_reset == false){			/* �����δ��λ */
					nrfx_pwm_stop(&motor_pwm0, false);	/* ֹͣPWM������� */
					current_grids_pos = 0;							/* ��ǰ��㸴λΪ0 */
					motor_has_been_reset = true;				/* ��λ��� */
			}
			NRF_LOG_INFO("external_interrupt_in_handler tar!!!"); 
    }
//�ж����Ŷ���
//    if(action == NRF_GPIOTE_POLARITY_HITOLO)//nrf_gpio_pin_toggle(LED_2);
//    else if(action == NRF_GPIOTE_POLARITY_LOTOHI)nrf_gpio_pin_toggle(LED_3);
//    else if(action == NRF_GPIOTE_POLARITY_TOGGLE)nrf_gpio_pin_toggle(LED_4);  
}

/* �ⲿ�ж������ʼ�� */
static void external_interrupt_in_init(void)
{
	ret_code_t err_code;	
	
	/* ��ʼ��GPIOTEģ�� */
	err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);
	
	/* �����½��ز����ж��¼� */
	nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	/* ���������������� */
	in_config.pull = NRF_GPIO_PIN_PULLUP;
  /* ���ø�����ΪGPIOTE���� */
  err_code = nrf_drv_gpiote_in_init(EXTERN_INT_PIN, &in_config, external_interrupt_in_handler);
  APP_ERROR_CHECK(err_code);
  /* ʹ�ܸ���������GPIOTEͨ�����¼�ģʽ */
  nrf_drv_gpiote_in_event_enable(EXTERN_INT_PIN, true);
}

/* ����PWM���ָ������������ */
static void motor_pulse_num(uint32_t pulse_num)
{ 
		if(pulse_num == 0){
			return;
		}
		
    /* ����PWM�������У��������а�����PWM���е���ʼ��ַ����С�����в��ſ������� */
	  nrf_pwm_sequence_t const seq0 = {
        .values.p_common = motor_pwm_seq_values,							  					/* ָ��PWM���� */
        .length          = NRF_PWM_VALUES_LENGTH(motor_pwm_seq_values),   /* PWM�����а��������ڸ��� */
        .repeats         = 0, 																	          /* �����������ظ�����Ϊ0 */
        .end_delay       = 0  																						/* ���к󲻲�����ʱ */
    };
    /* ����PWM���в��ţ�flags����ΪNRFX_PWM_FLAG_LOOP�����в�����ɺ��Զ������������²���,���ΪNRFX_PWM_FLAG_STOP���򲥷Ž�����PWMֹͣ */
    (void)nrfx_pwm_simple_playback(&motor_pwm0, &seq0, pulse_num, NRFX_PWM_FLAG_STOP);
}

/* �������PWM��ʼ�� */
static void motor_pwm_init(uint16_t pwm_top_value)
{
		/* ����PWM��ʼ�����ýṹ�岢��ʼ������ */
    nrfx_pwm_config_t const config0 = {
        .output_pins = {
            MOTOR_PULSE_PIN | NRFX_PWM_PIN_INVERTED, /* ͨ��0ӳ�䵽MOTOR_PULSE_PIN����,����״̬����ߵ�ƽ */
            NRFX_PWM_PIN_NOT_USED, /* ͨ��1��ʹ�� */
            NRFX_PWM_PIN_NOT_USED, /* ͨ��2��ʹ�� */
            NRFX_PWM_PIN_NOT_USED  /* ͨ��3��ʹ�� */
        },
        .irq_priority = APP_IRQ_PRIORITY_HIGH,	/* �ж����ȼ� */
        .base_clock   = NRF_PWM_CLK_16MHz,     /* PWMʱ��Ƶ������Ϊ125K */
        .count_mode   = NRF_PWM_MODE_UP,        /* ���ϼ���ģʽ */
        .top_value    = pwm_top_value,          /* ������ֵ */
        .load_mode    = NRF_PWM_LOAD_COMMON,    /* ͨ��װ��ģʽ */
        .step_mode    = NRF_PWM_STEP_AUTO       /* �����е������Զ��ƽ� */
    };
		/* ��ʼ��PWM */
    APP_ERROR_CHECK(nrfx_pwm_init(&motor_pwm0, &config0, NULL));
}

/* ��ʱ����������е�ָ�� */
static void communication_protocol_timer_handler(void * p_context) 
{
		receiving_process(&g_communication_protocol_handle);
		//NRF_LOG_INFO("communication_protocol_timer_handler.\n");
}

/* ͨ��Э���ʼ�� */
uint8_t communication_protocol_init(void)
{
		uint8_t res;
	
		/* ע��ͨ��Э��ӿ� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_INIT(&g_communication_protocol_handle, communication_protocol_handle_t);			     						/* ��ʼ��ͨ��Э���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_INIT(&g_communication_protocol_handle, communication_protocol_hal_init);  	   						/* ע��hal_init���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_DEINIT(&g_communication_protocol_handle, communication_protocol_hal_deinit); 						/* ע��hal_de_init���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_SEND_DATA_PACKET(&g_communication_protocol_handle, communication_protocol_send_data_packet); /* ע��send_data_packet���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_DEBUG_PRINT(&g_communication_protocol_handle, communication_protocol_debug_print);          	/* ע��debug_print���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_VAL(&g_communication_protocol_handle, communication_protocol_cmd_set_val);          					  /* ע��cmd_set_val���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_ENABLE(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_enable);  	/* ע��cmd_set_motor_enable���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_DISABLE(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_disable);	/* ע��cmd_set_motor_disable���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_RESET(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_reset);  		/* ע��cmd_set_motor_reset���� */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_PERIOD(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_period);  	/* ע��cmd_set_motor_period���� */

	  res = protocol_init(&g_communication_protocol_handle);		/* ��ʼ��ͨ��Э�� */
    if (res != 0){
        NRF_LOG_INFO("communication_protocol_init: init failed.\n");
        return 1;
    }
		app_timer_create(&communication_protocol_timer_id, APP_TIMER_MODE_REPEATED, communication_protocol_timer_handler);
		app_timer_start(communication_protocol_timer_id, APP_TIMER_TICKS(100), NULL);
		NRF_LOG_INFO("communication_protocol init finsh"); 
		
		motor_pwm_init(800);									 	/* ��ʼ��PWM,Ĭ��Ƶ��20Khz */
		nrf_gpio_cfg_output(MOTOR_DIR_PIN);			/* �������������� */
		nrf_gpio_cfg_output(MOTOR_EN_PIN);			/* ���ʹ�ܿ������� */
		nrf_gpio_pin_set(MOTOR_DIR_PIN);				/* Ĭ�����õ���������� */
		
		external_interrupt_in_init();
		
		return 0;
}

/* ����Ŀ��ֵ */
uint8_t communication_protocol_cmd_set_val(uint8_t ch_num, uint8_t* data_params, uint16_t data_len)
{
	if(motor_has_been_reset == false) return 1;				/* ���û�и�λ������ת�� */
	
	uint32_t target_pos = (uint32_t)COMPOUND_32BIT(((data_params+3)));	/* ȡ��Ŀ��λ������ */
	uint32_t pluse_num = 0;						/* ʵ�ʵ���������� */
	
	if(current_grids_pos < target_pos){
		nrf_gpio_pin_set(MOTOR_DIR_PIN);						/* �����������Ϊǰ�� */
		pluse_num = (target_pos-current_grids_pos)*((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)/NUM_OF_GRIDS);	/* ÿһ��ĵ������ */
	}else{
		nrf_gpio_pin_clear(MOTOR_DIR_PIN);					/* ���Ƶ����ת�����෴ */
		pluse_num = (current_grids_pos-target_pos)*((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)/NUM_OF_GRIDS);	/* ÿһ��ĵ������ */
	}
	current_grids_pos = target_pos;							/* ���µ�ǰλ�� */
	motor_pulse_num(pluse_num);						/* ���Ƶ��������� */
	NRF_LOG_INFO("call communication_protocol_cmd_set_val.ch:%d,len:%d,data:%d",ch_num,data_len,target_pos);
	set_computer_value(&g_communication_protocol_handle, SEND_FACT_CMD, CURVES_CH1, &pluse_num, 1);		/* ����ʵ�������� */
	
	return 0;
}

/* ������� */
uint8_t communication_protocol_cmd_set_motor_enable(uint8_t ch_num)
{
	nrf_gpio_pin_clear(MOTOR_EN_PIN); /* ʹ�ܵ�� */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_enable.ch:%d",ch_num);
	
	return 0;
}

/* ֹͣ��� */
uint8_t communication_protocol_cmd_set_motor_disable(uint8_t ch_num)
{
	nrf_gpio_pin_set(MOTOR_EN_PIN);	/* ���õ�� */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_disable.ch:%d",ch_num);
	
	return 0;
}

/* ��λ��� */
uint8_t communication_protocol_cmd_set_motor_reset(uint8_t ch_num)
{
	motor_has_been_reset = false;		/* �����δ��λ */
	motor_pulse_num((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)*20);	/* ת��ȦѰ�Ҷ�λ�� */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_reset.ch:%d",ch_num);
	
	return 0;
}

/* ���õ��Ƶ��,����Ĳ����ǵ��Ƶ�� */
uint8_t communication_protocol_cmd_set_motor_period(uint8_t ch_num, uint8_t* data_params, uint16_t data_len)
{
	uint32_t target_period = (uint32_t)COMPOUND_32BIT(((data_params+3)));	/* ȡ��Ŀ��λ������ */
	
	if(target_period > 16000000) return 1;	/* ����Ƶ�ʲ�Ӧ�ô���16Mhz */
		
	target_period = 16000000/target_period; /* ��������ҪƵ�ʼ���top_value��ֵ */
	motor_pwm_seq_values[0] = (target_period/2);	/* ռ�ձ�50% */
	motor_has_been_reset = false;		/* ȡ�������λ */
	nrfx_pwm_uninit(&motor_pwm0);		/* ȡ��PWMʹ�� */
	motor_pwm_init(target_period);	/* ��д��ʼ��PWM */
	set_computer_value(&g_communication_protocol_handle, SEND_FACT_CMD, CURVES_CH1, &target_period, 1);	/* ���ص����ǰƵ�� */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_period.ch:%d,len:%d,data:%d",ch_num,data_len,target_period);
	
	return 0;
}

