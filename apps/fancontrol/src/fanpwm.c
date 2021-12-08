
#include "system.h"
#include "fanpwm.h"

TRACE_TAG(fanpwm);

#define CFG_FANPWM_25KHZ_PERIOD        2880     // 25Khz period
#define CFG_FANPWM_MAX_PULSE           3000     // Max pulse length
#define CFG_FANWPM_INIT_PULSE          20       // Init pulse length in pct


static void timer_set_pwm(TIM_HandleTypeDef *timer, uint32_t channel, uint16_t period, uint16_t pulse);
static TIM_HandleTypeDef htim1;


int fanpwm_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Peripheral clock enable
    __HAL_RCC_TIM3_CLK_ENABLE();

    // TIM1 GPIO Configuration  PA6     ------> TIM3_CH1
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize timer
    TIM_MasterConfigTypeDef sMasterConfig; 
    TIM_OC_InitTypeDef sConfigOC; 
 
    // Initialize timer
    htim1.Instance = TIM3; 
    htim1.Init.Prescaler = 0; 
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP; 
    htim1.Init.Period = 0; 
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
    HAL_TIM_PWM_Init(&htim1);     
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; 
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; 
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig); 
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1; 
    sConfigOC.Pulse = 0; 
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; 
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; 
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1); 
    
    // Set Default
    fanpwm_set(CFG_FANWPM_INIT_PULSE);

    TRACE("Init");

    return 0;
}

int fanpwm_set(int pwm_pct)
{
    int pwm;
    
    if (pwm_pct < 0 || pwm_pct > 100) {
        TRACE_ERROR("PWM value must be 0-100%%");
        return -1;
    }

    pwm = (CFG_FANPWM_MAX_PULSE * pwm_pct) / 100;

    timer_set_pwm(&htim1, TIM_CHANNEL_1, CFG_FANPWM_25KHZ_PERIOD, pwm);

    TRACE("Set PWM  %d %% -> %d", pwm_pct, pwm);

    return 0;
}

static void timer_set_pwm(TIM_HandleTypeDef *timer, uint32_t channel,  uint16_t period, uint16_t pulse)
{
    HAL_TIM_PWM_Stop(timer, channel);       // stop generation of pwm
    timer->Init.Period = period;            // set the period duration
    HAL_TIM_PWM_Init(timer);                // reinititialise with new period value

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;                // set the pulse duration
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, channel);

    HAL_TIM_PWM_Start(timer, channel);       // start pwm generation

    //TRACE("Set pwm period: %d  pulse: %d", period, pulse);
}
