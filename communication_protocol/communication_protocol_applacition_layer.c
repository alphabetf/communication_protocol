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

communication_protocol_handle_t g_communication_protocol_handle;        /* 全局通信句柄 */
APP_TIMER_DEF(communication_protocol_timer_id);							 						/* 通信协议定时器句柄 */	

/* PWM驱动程序实例ID,0:PWM0 1:PWM1 2:PWM2 */
#define PWM_INSTANCE  0 
/* 步进电机脉冲输出引脚 */
#define MOTOR_PULSE_PIN	17		
/* 电机方向控制引脚 */
#define MOTOR_DIR_PIN	18		
/* 电机使能控制引脚 */
#define MOTOR_EN_PIN	19	
/* 外部中断触发引脚 */
#define EXTERN_INT_PIN 	8
/* 转盘每转一圈所需的电机圈数 */
#define GEAR_RATIO		18
/* 电机每转一圈所需的脉冲数 */
#define NUM_PULSE_PER_REVOLUTION	2000
/* 总格子的数量 */
#define NUM_OF_GRIDS	20

static nrfx_pwm_t motor_pwm0 = NRFX_PWM_INSTANCE(PWM_INSTANCE); /* PWM0 */
static nrf_pwm_values_common_t motor_pwm_seq_values[1] = {400}; 	/* 定义PWM序列(通用装载模式),该序列必须位于RAM中,因此要定义为static类型的 */
static bool motor_has_been_reset = false;											  /* 电机是否已经复位 */
static uint8_t current_grids_pos = 0;														/* 用于标志转盘当前位置 */

//GPIOTE事件处理函回调函数，事件回调函数里面可以获取pin编号和引脚动作
void external_interrupt_in_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	  if(pin == EXTERN_INT_PIN){ /* 确认中断触发的引脚 */
			if(motor_has_been_reset == false){			/* 电机尚未复位 */
					nrfx_pwm_stop(&motor_pwm0, false);	/* 停止PWM脉冲输出 */
					current_grids_pos = 0;							/* 当前格点复位为0 */
					motor_has_been_reset = true;				/* 复位完成 */
			}
			NRF_LOG_INFO("external_interrupt_in_handler tar!!!"); 
    }
//判断引脚动作
//    if(action == NRF_GPIOTE_POLARITY_HITOLO)//nrf_gpio_pin_toggle(LED_2);
//    else if(action == NRF_GPIOTE_POLARITY_LOTOHI)nrf_gpio_pin_toggle(LED_3);
//    else if(action == NRF_GPIOTE_POLARITY_TOGGLE)nrf_gpio_pin_toggle(LED_4);  
}

/* 外部中断输入初始化 */
static void external_interrupt_in_init(void)
{
	ret_code_t err_code;	
	
	/* 初始化GPIOTE模块 */
	err_code = nrf_drv_gpiote_init();
  APP_ERROR_CHECK(err_code);
	
	/* 配置下降沿产生中断事件 */
	nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	/* 开启引脚上拉电阻 */
	in_config.pull = NRF_GPIO_PIN_PULLUP;
  /* 配置该引脚为GPIOTE输入 */
  err_code = nrf_drv_gpiote_in_init(EXTERN_INT_PIN, &in_config, external_interrupt_in_handler);
  APP_ERROR_CHECK(err_code);
  /* 使能该引脚所在GPIOTE通道的事件模式 */
  nrf_drv_gpiote_in_event_enable(EXTERN_INT_PIN, true);
}

/* 控制PWM输出指定数量的脉冲 */
static void motor_pulse_num(uint32_t pulse_num)
{ 
		if(pulse_num == 0){
			return;
		}
		
    /* 定义PWM播放序列，播放序列包含了PWM序列的起始地址、大小和序列播放控制描述 */
	  nrf_pwm_sequence_t const seq0 = {
        .values.p_common = motor_pwm_seq_values,							  					/* 指向PWM序列 */
        .length          = NRF_PWM_VALUES_LENGTH(motor_pwm_seq_values),   /* PWM序列中包含的周期个数 */
        .repeats         = 0, 																	          /* 序列中周期重复次数为0 */
        .end_delay       = 0  																						/* 序列后不插入延时 */
    };
    /* 启动PWM序列播放，flags设置为NRFX_PWM_FLAG_LOOP：序列播放完成后，自动触发任务重新播放,如改为NRFX_PWM_FLAG_STOP，则播放结束后，PWM停止 */
    (void)nrfx_pwm_simple_playback(&motor_pwm0, &seq0, pulse_num, NRFX_PWM_FLAG_STOP);
}

/* 电机脉冲PWM初始化 */
static void motor_pwm_init(uint16_t pwm_top_value)
{
		/* 定义PWM初始化配置结构体并初始化参数 */
    nrfx_pwm_config_t const config0 = {
        .output_pins = {
            MOTOR_PULSE_PIN | NRFX_PWM_PIN_INVERTED, /* 通道0映射到MOTOR_PULSE_PIN引脚,空闲状态输出高电平 */
            NRFX_PWM_PIN_NOT_USED, /* 通道1不使用 */
            NRFX_PWM_PIN_NOT_USED, /* 通道2不使用 */
            NRFX_PWM_PIN_NOT_USED  /* 通道3不使用 */
        },
        .irq_priority = APP_IRQ_PRIORITY_HIGH,	/* 中断优先级 */
        .base_clock   = NRF_PWM_CLK_16MHz,     /* PWM时钟频率设置为125K */
        .count_mode   = NRF_PWM_MODE_UP,        /* 向上计数模式 */
        .top_value    = pwm_top_value,          /* 最大计数值 */
        .load_mode    = NRF_PWM_LOAD_COMMON,    /* 通用装载模式 */
        .step_mode    = NRF_PWM_STEP_AUTO       /* 序列中的周期自动推进 */
    };
		/* 初始化PWM */
    APP_ERROR_CHECK(nrfx_pwm_init(&motor_pwm0, &config0, NULL));
}

/* 定时处理缓冲队列中的指令 */
static void communication_protocol_timer_handler(void * p_context) 
{
		receiving_process(&g_communication_protocol_handle);
		//NRF_LOG_INFO("communication_protocol_timer_handler.\n");
}

/* 通信协议初始化 */
uint8_t communication_protocol_init(void)
{
		uint8_t res;
	
		/* 注册通信协议接口 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_INIT(&g_communication_protocol_handle, communication_protocol_handle_t);			     						/* 初始化通信协议句柄 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_INIT(&g_communication_protocol_handle, communication_protocol_hal_init);  	   						/* 注册hal_init函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_HAL_DEINIT(&g_communication_protocol_handle, communication_protocol_hal_deinit); 						/* 注册hal_de_init函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_SEND_DATA_PACKET(&g_communication_protocol_handle, communication_protocol_send_data_packet); /* 注册send_data_packet函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_DEBUG_PRINT(&g_communication_protocol_handle, communication_protocol_debug_print);          	/* 注册debug_print函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_VAL(&g_communication_protocol_handle, communication_protocol_cmd_set_val);          					  /* 注册cmd_set_val函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_ENABLE(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_enable);  	/* 注册cmd_set_motor_enable函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_DISABLE(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_disable);	/* 注册cmd_set_motor_disable函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_RESET(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_reset);  		/* 注册cmd_set_motor_reset函数 */
		DRIVER_COMMUNICATION_PROTOCOL_LINK_CMD_SET_MOTOR_PERIOD(&g_communication_protocol_handle, communication_protocol_cmd_set_motor_period);  	/* 注册cmd_set_motor_period函数 */

	  res = protocol_init(&g_communication_protocol_handle);		/* 初始化通信协议 */
    if (res != 0){
        NRF_LOG_INFO("communication_protocol_init: init failed.\n");
        return 1;
    }
		app_timer_create(&communication_protocol_timer_id, APP_TIMER_MODE_REPEATED, communication_protocol_timer_handler);
		app_timer_start(communication_protocol_timer_id, APP_TIMER_TICKS(100), NULL);
		NRF_LOG_INFO("communication_protocol init finsh"); 
		
		motor_pwm_init(800);									 	/* 初始化PWM,默认频率20Khz */
		nrf_gpio_cfg_output(MOTOR_DIR_PIN);			/* 电机方向控制引脚 */
		nrf_gpio_cfg_output(MOTOR_EN_PIN);			/* 电机使能控制引脚 */
		nrf_gpio_pin_set(MOTOR_DIR_PIN);				/* 默认设置电机驱动方向 */
		
		external_interrupt_in_init();
		
		return 0;
}

/* 设置目标值 */
uint8_t communication_protocol_cmd_set_val(uint8_t ch_num, uint8_t* data_params, uint16_t data_len)
{
	if(motor_has_been_reset == false) return 1;				/* 电机没有复位不允许转动 */
	
	uint32_t target_pos = (uint32_t)COMPOUND_32BIT(((data_params+3)));	/* 取得目标位置数据 */
	uint32_t pluse_num = 0;						/* 实际电机脉冲数量 */
	
	if(current_grids_pos < target_pos){
		nrf_gpio_pin_set(MOTOR_DIR_PIN);						/* 电机方向设置为前进 */
		pluse_num = (target_pos-current_grids_pos)*((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)/NUM_OF_GRIDS);	/* 每一格的电机脉冲 */
	}else{
		nrf_gpio_pin_clear(MOTOR_DIR_PIN);					/* 控制电机旋转方向相反 */
		pluse_num = (current_grids_pos-target_pos)*((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)/NUM_OF_GRIDS);	/* 每一格的电机脉冲 */
	}
	current_grids_pos = target_pos;							/* 更新当前位置 */
	motor_pulse_num(pluse_num);						/* 控制电机输出脉冲 */
	NRF_LOG_INFO("call communication_protocol_cmd_set_val.ch:%d,len:%d,data:%d",ch_num,data_len,target_pos);
	set_computer_value(&g_communication_protocol_handle, SEND_FACT_CMD, CURVES_CH1, &pluse_num, 1);		/* 返回实际脉冲数 */
	
	return 0;
}

/* 启动电机 */
uint8_t communication_protocol_cmd_set_motor_enable(uint8_t ch_num)
{
	nrf_gpio_pin_clear(MOTOR_EN_PIN); /* 使能电机 */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_enable.ch:%d",ch_num);
	
	return 0;
}

/* 停止电机 */
uint8_t communication_protocol_cmd_set_motor_disable(uint8_t ch_num)
{
	nrf_gpio_pin_set(MOTOR_EN_PIN);	/* 禁用电机 */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_disable.ch:%d",ch_num);
	
	return 0;
}

/* 复位电机 */
uint8_t communication_protocol_cmd_set_motor_reset(uint8_t ch_num)
{
	motor_has_been_reset = false;		/* 电机还未复位 */
	motor_pulse_num((GEAR_RATIO*NUM_PULSE_PER_REVOLUTION)*20);	/* 转两圈寻找定位点 */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_reset.ch:%d",ch_num);
	
	return 0;
}

/* 设置电机频率,输入的参数是电机频率 */
uint8_t communication_protocol_cmd_set_motor_period(uint8_t ch_num, uint8_t* data_params, uint16_t data_len)
{
	uint32_t target_period = (uint32_t)COMPOUND_32BIT(((data_params+3)));	/* 取得目标位置数据 */
	
	if(target_period > 16000000) return 1;	/* 输入频率不应该大于16Mhz */
		
	target_period = 16000000/target_period; /* 根据所需要频率计算top_value的值 */
	motor_pwm_seq_values[0] = (target_period/2);	/* 占空比50% */
	motor_has_been_reset = false;		/* 取消电机复位 */
	nrfx_pwm_uninit(&motor_pwm0);		/* 取消PWM使能 */
	motor_pwm_init(target_period);	/* 重写初始化PWM */
	set_computer_value(&g_communication_protocol_handle, SEND_FACT_CMD, CURVES_CH1, &target_period, 1);	/* 返回电机当前频率 */
	NRF_LOG_INFO("call communication_protocol_cmd_set_motor_period.ch:%d,len:%d,data:%d",ch_num,data_len,target_period);
	
	return 0;
}

