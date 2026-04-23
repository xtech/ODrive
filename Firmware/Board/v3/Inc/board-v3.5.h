#ifndef __BOARD_CONFIG_V3_5_H
#define __BOARD_CONFIG_V3_5_H


// Pin Defines
// GPIOs
// GPIO1 == MISO_EXT
#define GPIO_1_Pin GPIO_PIN_6
#define GPIO_1_GPIO_Port GPIOA
// GPIO2 == SCK_EXT
#define GPIO_2_Pin GPIO_PIN_5
#define GPIO_2_GPIO_Port GPIOA

// GPIO3 == RED LED
#define GPIO_3_Pin GPIO_PIN_1
#define GPIO_3_GPIO_Port GPIOB

// PC3
#define VBUS_S_Pin GPIO_PIN_3

// PC4
#define M0_TEMP_Pin GPIO_PIN_4

// M0 Gate Driver Chip Select
#define M0_nCS_Pin GPIO_PIN_9
#define M0_nCS_GPIO_Port GPIOC

// M0 Gate Driver nSLEEP
#define M0_nSLEEP_Pin GPIO_PIN_15
#define M0_nSLEEP_GPIO_Port GPIOA

// M0 Gate Driver ILIM
#define M0_ILIM_Pin GPIO_PIN_4
#define M0_ILIM_GPIO_Port GPIOA

// Define MISO (PB3) MOSI (PB4) and SCK (PC10)
#define M0_DRV_MISO_Pin GPIO_PIN_3
#define M0_DRV_MISO_GPIO_Port GPIOB
#define M0_DRV_MOSI_Pin GPIO_PIN_4
#define M0_DRV_MOSI_GPIO_Port GPIOB
#define M0_DRV_SCK_Pin GPIO_PIN_10
#define M0_DRV_SCK_GPIO_Port GPIOC


// Phases High and Low to Gate Driver
#define M0_AH_Pin GPIO_PIN_8
#define M0_AH_GPIO_Port GPIOA
#define M0_BH_Pin GPIO_PIN_9
#define M0_BH_GPIO_Port GPIOA
#define M0_CH_Pin GPIO_PIN_10
#define M0_CH_GPIO_Port GPIOA
#define M0_AL_Pin GPIO_PIN_13
#define M0_AL_GPIO_Port GPIOB
#define M0_BL_Pin GPIO_PIN_14
#define M0_BL_GPIO_Port GPIOB
#define M0_CL_Pin GPIO_PIN_15
#define M0_CL_GPIO_Port GPIOB

// Current sense phase B and C
#define M0_IB_Pin GPIO_PIN_0
#define M0_IB_GPIO_Port GPIOC
#define M0_IC_Pin GPIO_PIN_1
#define M0_IC_GPIO_Port GPIOC

// Fault Pin
#define nFAULT_Pin GPIO_PIN_7
#define nFAULT_GPIO_Port GPIOB



// Encoder
#define M0_ENC_Z_Pin GPIO_PIN_6
#define M0_ENC_Z_GPIO_Port GPIOC
#define M0_ENC_A_Pin GPIO_PIN_7
#define M0_ENC_A_GPIO_Port GPIOC
#define M0_ENC_B_Pin GPIO_PIN_8
#define M0_ENC_B_GPIO_Port GPIOC


#define DEFAULT_MIN_DC_VOLTAGE 8.0f

// Dummy shunt value because DRV already gives us Amps * gain
#define SHUNT_RESISTANCE (1.0f)

#define M0_THERMISTOR_ADC_CHANNEL 15
#define M0_STEP_GPIO_PIN 1
#define M0_DIR_GPIO_PIN 2
#define PWM0_CHANNELS {1, 2, 3, 4}

// the first one is dummy in the FW for some reason, lets keep it like that
#define GPIO_COUNT 17
#define DEFAULT_GPIO_MODES \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL, \
    ODriveIntf::GPIO_MODE_DIGITAL

#define VBUS_S_DIVIDER_RATIO 15.666666724f
#define HW_VERSION_VOLTAGE (32)

// GPIO for Error output
#define DEFAULT_ERROR_PIN 3


#define BOARD_GPIOS { \
    {}, \
    {GPIO_1_GPIO_Port, GPIO_1_Pin},\
    {GPIO_2_GPIO_Port, GPIO_2_Pin},\
    {GPIO_3_GPIO_Port, GPIO_3_Pin},\
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {}, \
    {} \
}

#define BOARD_ALTERANATE_FUNCTIONS { \
    /* GPIO0 (inexistent): */ {{}}, \
    /* GPIO1: */ {{}}, \
    /* GPIO1: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}}, \
    /* GPIO2: */ {{}} \
};

// v3.5 and 3.6 use the default main.h (no extra include needed)

#endif
