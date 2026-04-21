#ifndef __BOARD_CONFIG_V3_5_H
#define __BOARD_CONFIG_V3_5_H


// Pin Defines
#define M0_nCS_Pin GPIO_PIN_13
#define M0_nCS_GPIO_Port GPIOC
#define M1_nCS_Pin GPIO_PIN_14
#define M1_nCS_GPIO_Port GPIOC
#define M1_ENC_Z_Pin GPIO_PIN_15
#define M1_ENC_Z_GPIO_Port GPIOC
#define M0_IB_Pin GPIO_PIN_0
#define M0_IB_GPIO_Port GPIOC
#define M0_IC_Pin GPIO_PIN_1
#define M0_IC_GPIO_Port GPIOC
#define M1_IC_Pin GPIO_PIN_2
#define M1_IC_GPIO_Port GPIOC
#define M1_IB_Pin GPIO_PIN_3
#define M1_IB_GPIO_Port GPIOC
#define GPIO_1_Pin GPIO_PIN_0
#define GPIO_1_GPIO_Port GPIOA
#define GPIO_2_Pin GPIO_PIN_1
#define GPIO_2_GPIO_Port GPIOA
#define GPIO_3_Pin GPIO_PIN_2
#define GPIO_3_GPIO_Port GPIOA
#define GPIO_4_Pin GPIO_PIN_3
#define GPIO_4_GPIO_Port GPIOA
#define M1_TEMP_Pin GPIO_PIN_4
#define M1_TEMP_GPIO_Port GPIOA
#define AUX_TEMP_Pin GPIO_PIN_5
#define AUX_TEMP_GPIO_Port GPIOA
#define VBUS_S_Pin GPIO_PIN_6
#define VBUS_S_GPIO_Port GPIOA
#define M1_AL_Pin GPIO_PIN_7
#define M1_AL_GPIO_Port GPIOA
#define GPIO_5_Pin GPIO_PIN_4
#define GPIO_5_GPIO_Port GPIOC
#define M0_TEMP_Pin GPIO_PIN_5
#define M0_TEMP_GPIO_Port GPIOC
#define M1_BL_Pin GPIO_PIN_0
#define M1_BL_GPIO_Port GPIOB
#define M1_CL_Pin GPIO_PIN_1
#define M1_CL_GPIO_Port GPIOB
#define GPIO_6_Pin GPIO_PIN_2
#define GPIO_6_GPIO_Port GPIOB
#define AUX_L_Pin GPIO_PIN_10
#define AUX_L_GPIO_Port GPIOB
#define AUX_H_Pin GPIO_PIN_11
#define AUX_H_GPIO_Port GPIOB
#define EN_GATE_Pin GPIO_PIN_12
#define EN_GATE_GPIO_Port GPIOB
#define M0_AL_Pin GPIO_PIN_13
#define M0_AL_GPIO_Port GPIOB
#define M0_BL_Pin GPIO_PIN_14
#define M0_BL_GPIO_Port GPIOB
#define M0_CL_Pin GPIO_PIN_15
#define M0_CL_GPIO_Port GPIOB
#define M1_AH_Pin GPIO_PIN_6
#define M1_AH_GPIO_Port GPIOC
#define M1_BH_Pin GPIO_PIN_7
#define M1_BH_GPIO_Port GPIOC
#define M1_CH_Pin GPIO_PIN_8
#define M1_CH_GPIO_Port GPIOC
#define M0_ENC_Z_Pin GPIO_PIN_9
#define M0_ENC_Z_GPIO_Port GPIOC
#define M0_AH_Pin GPIO_PIN_8
#define M0_AH_GPIO_Port GPIOA
#define M0_BH_Pin GPIO_PIN_9
#define M0_BH_GPIO_Port GPIOA
#define M0_CH_Pin GPIO_PIN_10
#define M0_CH_GPIO_Port GPIOA
#define GPIO_7_Pin GPIO_PIN_15
#define GPIO_7_GPIO_Port GPIOA
#define nFAULT_Pin GPIO_PIN_2
#define nFAULT_GPIO_Port GPIOD
#define GPIO_8_Pin GPIO_PIN_3
#define GPIO_8_GPIO_Port GPIOB
#define M0_ENC_A_Pin GPIO_PIN_4
#define M0_ENC_A_GPIO_Port GPIOB
#define M0_ENC_B_Pin GPIO_PIN_5
#define M0_ENC_B_GPIO_Port GPIOB
#define M1_ENC_A_Pin GPIO_PIN_6
#define M1_ENC_A_GPIO_Port GPIOB
#define M1_ENC_B_Pin GPIO_PIN_7
#define M1_ENC_B_GPIO_Port GPIOB


#define AXIS_COUNT (2)
#define DEFAULT_ERROR_PIN 0
#define DEFAULT_MIN_DC_VOLTAGE 8.0f

#define SHUNT_RESISTANCE (500e-6f)
#if HW_VERSION_VOLTAGE >= 48
#define DEFAULT_BRAKE_RESISTANCE (2.0f)
#else
#define DEFAULT_BRAKE_RESISTANCE (0.47f)
#endif
#define M1_THERMISTOR_ADC_CHANNEL 4
#define M1_STEP_GPIO_PIN 7
#define M1_DIR_GPIO_PIN 8

#define M0_THERMISTOR_ADC_CHANNEL 15
#define M0_STEP_GPIO_PIN 1
#define M0_DIR_GPIO_PIN 2
#define PWM0_CHANNELS {1, 2, 3, 4}

#define GPIO_COUNT 17
#define DEFAULT_GPIO_MODES \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_UART_A, \
    ODriveIntf::GPIO_MODE_UART_A, \
    ODriveIntf::GPIO_MODE_ANALOG_IN, \
    ODriveIntf::GPIO_MODE_ANALOG_IN, \
    ODriveIntf::GPIO_MODE_ANALOG_IN, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_ENC0, \
    ODriveIntf::GPIO_MODE_ENC0, \
    ODriveIntf::GPIO_MODE_DIGITAL_PULL_DOWN, \
    ODriveIntf::GPIO_MODE_ENC1, \
    ODriveIntf::GPIO_MODE_ENC1, \
    ODriveIntf::GPIO_MODE_DIGITAL_PULL_DOWN, \
    ODriveIntf::GPIO_MODE_CAN_A, \
    ODriveIntf::GPIO_MODE_CAN_A,

#if HW_VERSION_VOLTAGE >= 48
#define VBUS_S_DIVIDER_RATIO 19.0f
#elif HW_VERSION_VOLTAGE == 24
#define VBUS_S_DIVIDER_RATIO 11.0f
#else
#error "unknown board voltage"
#endif

#define BOARD_GPIOS { \
    {nullptr, 0}, \
    {GPIOA, GPIO_PIN_0}, {GPIOA, GPIO_PIN_1}, {GPIOA, GPIO_PIN_2}, {GPIOA, GPIO_PIN_3}, \
    {GPIOC, GPIO_PIN_4}, {GPIOB, GPIO_PIN_2}, {GPIOA, GPIO_PIN_15}, {GPIOB, GPIO_PIN_3}, \
    {GPIOB, GPIO_PIN_4}, {GPIOB, GPIO_PIN_5}, {GPIOC, GPIO_PIN_9}, {GPIOB, GPIO_PIN_6}, \
    {GPIOB, GPIO_PIN_7}, {GPIOC, GPIO_PIN_15}, {GPIOB, GPIO_PIN_8}, {GPIOB, GPIO_PIN_9} \
}

#define BOARD_ALTERANATE_FUNCTIONS { \
    /* GPIO0 (inexistent): */ {{}}, \
    /* GPIO1: */ {{{ODrive::GPIO_MODE_UART_A, GPIO_AF8_UART4}, {ODrive::GPIO_MODE_PWM, GPIO_AF2_TIM5}}}, \
    /* GPIO2: */ {{{ODrive::GPIO_MODE_UART_A, GPIO_AF8_UART4}, {ODrive::GPIO_MODE_PWM, GPIO_AF2_TIM5}}}, \
    /* GPIO3: */ {{{ODrive::GPIO_MODE_UART_B, GPIO_AF7_USART2}, {ODrive::GPIO_MODE_PWM, GPIO_AF2_TIM5}}}, \
    /* GPIO4: */ {{{ODrive::GPIO_MODE_UART_B, GPIO_AF7_USART2}, {ODrive::GPIO_MODE_PWM, GPIO_AF2_TIM5}}}, \
    /* GPIO5: */ {{}}, \
    /* GPIO6: */ {{}}, \
    /* GPIO7: */ {{}}, \
    /* GPIO8: */ {{}}, \
    /* ENC0_A: */ {{{ODrive::GPIO_MODE_ENC0, GPIO_AF2_TIM3}}}, \
    /* ENC0_B: */ {{{ODrive::GPIO_MODE_ENC0, GPIO_AF2_TIM3}}}, \
    /* ENC0_Z: */ {{}}, \
    /* ENC1_A: */ {{{ODrive::GPIO_MODE_I2C_A, GPIO_AF4_I2C1}, {ODrive::GPIO_MODE_ENC1, GPIO_AF2_TIM4}}}, \
    /* ENC1_B: */ {{{ODrive::GPIO_MODE_I2C_A, GPIO_AF4_I2C1}, {ODrive::GPIO_MODE_ENC1, GPIO_AF2_TIM4}}}, \
    /* ENC1_Z: */ {{}}, \
    /* CAN_R: */ {{{ODrive::GPIO_MODE_CAN_A, GPIO_AF9_CAN1}, {ODrive::GPIO_MODE_I2C_A, GPIO_AF4_I2C1}}}, \
    /* CAN_D: */ {{{ODrive::GPIO_MODE_CAN_A, GPIO_AF9_CAN1}, {ODrive::GPIO_MODE_I2C_A, GPIO_AF4_I2C1}}}, \
};

// v3.5 and 3.6 use the default main.h (no extra include needed)

#endif
