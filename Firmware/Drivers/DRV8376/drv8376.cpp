
#include "board.h"
#include "cmsis_os.h"
#include "drv8376.hpp"
#include "utils.hpp"

// Any Fault
#define DRV8376_DEVICE_FAULT_FAULT (1 << 0)
// Overtemperature Fault
#define DRV8376_DEVICE_FAULT_OTF (1 << 1)
// Supply Undervoltage
#define DRV8376_DEVICE_FAULT_UVP (1 << 2)
// Supply Overvoltage
#define DRV8376_DEVICE_FAULT_OVP (1 << 3)
// Overcurrent
#define DRV8376_DEVICE_FAULT_OCP (1 << 5)
// SPI Fault
#define DRV8376_DEVICE_FAULT_SPIFLT (1 << 6)
// Reset Status
#define DRV8376_DEVICE_FAULT_RESET (1 << 7)
// System Fault
#define DRV8376_DEVICE_FAULT_SYSFLT (1 << 8)
// Device not Ready
#define DRV8376_DEVICE_FAULT_DNRDY_STS (1 << 9)

#define DRV8376_DEVICE_FAULT_MASK (DRV8376_DEVICE_FAULT_FAULT | \
                                  DRV8376_DEVICE_FAULT_OTF | \
                                  DRV8376_DEVICE_FAULT_UVP | \
                                  DRV8376_DEVICE_FAULT_OVP | \
                                  DRV8376_DEVICE_FAULT_OCP | \
                                  DRV8376_DEVICE_FAULT_SPIFLT | \
                                  DRV8376_DEVICE_FAULT_RESET | \
                                  DRV8376_DEVICE_FAULT_SYSFLT | \
                                  DRV8376_DEVICE_FAULT_DNRDY_STS)

// Overtemperature Shutdown
#define DRV8376_TEMPERATURE_OTSD (1<<0)
// Overtemperature Warning
#define DRV8376_TEMPERATURE_OTW (1<<1)

#define DRV8376_TEMPERATURE_MASK (DRV8376_TEMPERATURE_OTSD | DRV8376_TEMPERATURE_OTW)

// Motor Over Voltage
#define DRV8376_SUPPLY_STAT_VM_OV (1<<6)
// Charge Pump Undervoltage
#define DRV8376_SUPPLY_STAT_CP_UV (1<<4)

#define DRV8376_SUPPLY_STAT_MASK (DRV8376_SUPPLY_STAT_VM_OV | DRV8376_SUPPLY_STAT_CP_UV)

// Overcurrent Protection High Side FET
#define DRV8376_DRIVER_STAT_OCPC_HS (1<<6)
#define DRV8376_DRIVER_STAT_OCPB_HS (1<<5)
#define DRV8376_DRIVER_STAT_OCPA_HS (1<<4)

// Overcurrent Protection Low Side FET
#define DRV8376_DRIVER_STAT_OCPC_LS (1<<2)
#define DRV8376_DRIVER_STAT_OCPB_LS (1<<1)
#define DRV8376_DRIVER_STAT_OCPA_LS (1<<0)

#define DRV8376_DRIVER_STAT_MASK (DRV8376_DRIVER_STAT_OCPC_HS | DRV8376_DRIVER_STAT_OCPB_HS | DRV8376_DRIVER_STAT_OCPA_HS | \
                                   DRV8376_DRIVER_STAT_OCPC_LS | DRV8376_DRIVER_STAT_OCPB_LS | DRV8376_DRIVER_STAT_OCPA_LS)



bool Drv8376::config(float requested_gain, float* actual_gain) {
    *actual_gain = 0.4f;
    return true;
}

bool Drv8376::init() {
    uint16_t val;

    if (state_ == kStateReady) {
        return true;
    }

    nfault_gpio_.config(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    nsleep_gpio_.config(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    nsleep_gpio_.write(false);
    for (int i = 0; i < 10; i++) {
        delay_us(10000);
    }
    nsleep_gpio_.write(true);

    ncs_gpio_.config(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    miso_gpio_.config(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    mosi_gpio_.config(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    sclk_gpio_.config(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    ilim_gpio_.config(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH);
    ilim_gpio_.write(true);


    ncs_gpio_.write(true);
    mosi_gpio_.write(true);

    state_ = kStateReady;
    return is_ready();
}

void Drv8376::do_checks() {
     if (state_ != kStateUninitialized && !nfault_gpio_.read()) {
         state_ = kStateUninitialized;
    }
}

bool Drv8376::is_ready() {
    return state_ == kStateReady;
}

Drv8376::FaultType_e Drv8376::get_error() {
    if (state_ == kStateReady) {
        return FaultType_NoFault;
    }
    return FaultType_FAULT;
}
