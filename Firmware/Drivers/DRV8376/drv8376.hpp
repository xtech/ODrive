#ifndef __DRV8376_HPP
#define __DRV8376_HPP

#include "stdbool.h"
#include "stdint.h"

#include <Drivers/gate_driver.hpp>
#include <Drivers/STM32/stm32_spi_arbiter.hpp>
#include <Drivers/STM32/stm32_gpio.hpp>



class Drv8376 : public GateDriverBase, public OpAmpBase {
public:
    typedef enum : uint32_t {
        FaultType_NoFault  = (0 << 0),  //!< No fault

        // Status Register 1
        FaultType_FETLC_OC = (1 << 0),  //!< FET Low side, Phase C Over Current fault
        FaultType_FETHC_OC = (1 << 1),  //!< FET High side, Phase C Over Current fault
        FaultType_FETLB_OC = (1 << 2),  //!< FET Low side, Phase B Over Current fault
        FaultType_FETHB_OC = (1 << 3),  //!< FET High side, Phase B Over Current fault
        FaultType_FETLA_OC = (1 << 4),  //!< FET Low side, Phase A Over Current fault
        FaultType_FETHA_OC = (1 << 5),  //!< FET High side, Phase A Over Current fault
        FaultType_OTW      = (1 << 6),  //!< Over Temperature Warning fault
        FaultType_OTSD     = (1 << 7),  //!< Over Temperature Shut Down fault
        FaultType_PVDD_UV  = (1 << 8),  //!< Power supply Vdd Under Voltage fault
        FaultType_GVDD_UV  = (1 << 9),  //!< DRV8376 Vdd Under Voltage fault
        FaultType_FAULT    = (1 << 10),

        // Status Register 2
        FaultType_GVDD_OV  = (1 << 23)  //!< DRV8376 Vdd Over Voltage fault
    } FaultType_e;

    Drv8376(Stm32Gpio ncs_gpio,
            Stm32Gpio nsleep_gpio, Stm32Gpio nfault_gpio,
            Stm32Gpio miso_gpio, Stm32Gpio mosi_gpio, Stm32Gpio sclk_gpio, Stm32Gpio ilim_gpio)
            : ncs_gpio_(ncs_gpio),
              nsleep_gpio_(nsleep_gpio), nfault_gpio_(nfault_gpio),
              miso_gpio_(miso_gpio), mosi_gpio_(mosi_gpio), sclk_gpio_(sclk_gpio), ilim_gpio_(ilim_gpio) {}

    /**
     * @brief Prepares the gate driver's configuration.
     *
     * If the gate driver was in ready state and the new configuration is
     * different from the old one then the gate driver will exit ready state.
     *
     * In any case changes to the configuration only take effect with a call to
     * init().
     */
    bool config(float requested_gain, float* actual_gain);
    
    /**
     * @brief Initializes the gate driver to the configuration prepared with
     * config().
     *
     * Returns true on success or false otherwise (e.g. if the gate driver is
     * not connected or not powered or if config() was not yet called).
     */
    bool init();

    /**
     * @brief Monitors the nFAULT pin.
     *
     * This must be run at an interval of <8ms from the moment the init()
     * functions starts to run, otherwise it's possible that a temporary power
     * loss is missed, leading to unwanted register values.
     * In case of power loss the nFAULT pin can be low for as little as 8ms.
     */
    void do_checks();

    /**
     * @brief Returns true if and only if the DRV8376 chip is in an initialized
     * state and ready to do switching and current sensor opamp operation.
     */
    bool is_ready() final;

    /**
     * @brief This has no effect on this driver chip because the drive stages are
     * always enabled while the chip is initialized
     */
    bool set_enabled(bool enabled) final { return true; }

    FaultType_e get_error();

private:
    // Configuration
    Stm32Gpio ncs_gpio_;
    Stm32Gpio nsleep_gpio_;
    Stm32Gpio nfault_gpio_;
    Stm32Gpio miso_gpio_;
    Stm32Gpio mosi_gpio_;
    Stm32Gpio sclk_gpio_;
    Stm32Gpio ilim_gpio_;

    // We don't put these buffers on the stack because we place the stack in
    // a RAM section which cannot be used by DMA.
    uint16_t tx_buf_, rx_buf_;

    enum {
        kStateUninitialized,
        kStateStartupChecks,
        kStateReady,
    } state_ = kStateUninitialized;
};


#endif // __DRV8376_HPP
