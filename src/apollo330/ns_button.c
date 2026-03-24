//*****************************************************************************
//
//! @file button.c
//!
//! @brief Utility for reading board buttons and GPIO-backed trigger inputs.
//!
//! Purpose: Reading board buttons and other simple GPIO-backed inputs.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// ${copyright}
//
// This is part of revision ${version} of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_bsp.h"
#include "am_mcu_apollo.h"
#include "am_util.h"
#include "ns_core.h"
#include "ns_peripherals_button.h"

/**
 * @brief GPIO ISR handler
 *
 */
void am_gpio0_001f_isr(void) {
    uint32_t ui32IntStatus;
    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_001F_IRQn, ui32IntStatus);
}

// #ifndef NS_GPIO0_203F_IRQn
// #define NS_GPIO0_203F_IRQn GPIO0_203F_IRQn
// void am_gpio0_203f_isr(void) {
//     uint32_t ui32IntStatus;
//     // Clear the GPIO Interrupt (write to clear).
//     AM_CRITICAL_BEGIN
//     am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, true, &ui32IntStatus);
//     am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, ui32IntStatus);
//     AM_CRITICAL_END
//     am_hal_gpio_interrupt_service(GPIO0_203F_IRQn, ui32IntStatus);
// }
// #endif

void am_gpio0_405f_isr(void) {
    uint32_t ui32IntStatus;
    // Clear the GPIO Interrupt (write to clear).
    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_405F_IRQn, true, &ui32IntStatus);
    am_hal_gpio_interrupt_irq_clear(GPIO0_405F_IRQn, ui32IntStatus);
    AM_CRITICAL_END
    am_hal_gpio_interrupt_service(GPIO0_405F_IRQn, ui32IntStatus);
}

extern int volatile *g_ns_peripheral_button_flags[NS_BUTTON_MAX_INPUTS];
extern void ns_button_input_handler_0(void *pArg);
extern void ns_button_input_handler_1(void *pArg);
extern void ns_button_input_handler_2(void *pArg);

static am_hal_gpio_handler_t ns_button_handler_for_index(uint32_t index) {
    switch (index) {
    case 0:
        return (am_hal_gpio_handler_t)ns_button_input_handler_0;
    case 1:
        return (am_hal_gpio_handler_t)ns_button_input_handler_1;
    case 2:
        return (am_hal_gpio_handler_t)ns_button_input_handler_2;
    default:
        return NULL;
    }
}

static am_hal_gpio_pincfg_t ns_button_input_cfg(ns_button_input_mode_t mode) {
    am_hal_gpio_pincfg_t cfg = am_hal_gpio_pincfg_input;
    if (mode == NS_BUTTON_INPUT_MODE_INPUT_PULLUP) {
        cfg.GP.cfg_b.ePullup = AM_HAL_GPIO_PIN_PULLUP_100K;
    }
    return cfg;
}

uint32_t ns_button_platform_init(ns_button_config_t *cfg) {
    uint32_t ui32IntStatus;
    uint32_t gpio_num = 0;
    uint32_t input_count = 0;
    uint32_t idx = 0;
    uint32_t GpioIntMask = 0;
    ns_button_input_t inputs[NS_BUTTON_MAX_INPUTS] = {0};
    bool legacy_button_cfg[NS_BUTTON_MAX_INPUTS] = {false};

    input_count = cfg->input_count;
    if (input_count > NS_BUTTON_MAX_INPUTS) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (input_count == 0) {
#if defined(AM_PART_APOLLO510L) || defined(AM_PART_APOLLO330P)
        if (cfg->button_0_enable && idx < NS_BUTTON_MAX_INPUTS) {
            inputs[idx] = (ns_button_input_t){
                .enable = true,
                .gpio_num = AM_BSP_GPIO_BUTTON0,
                .mode = NS_BUTTON_INPUT_MODE_INPUT_PULLUP,
                .flag = cfg->button_0_flag,
            };
            legacy_button_cfg[idx++] = true;
        }
        if (cfg->button_1_enable && idx < NS_BUTTON_MAX_INPUTS) {
            inputs[idx] = (ns_button_input_t){
                .enable = true,
                .gpio_num = AM_BSP_GPIO_BUTTON1,
                .mode = NS_BUTTON_INPUT_MODE_INPUT_PULLUP,
                .flag = cfg->button_1_flag,
            };
            legacy_button_cfg[idx++] = true;
        }
#endif
        if (cfg->joulescope_trigger_enable && idx < NS_BUTTON_MAX_INPUTS) {
            inputs[idx++] = (ns_button_input_t){
                .enable = true,
#if defined(AM_PART_APOLLO330P)
                .gpio_num = 5,
#else
                .gpio_num = 14,
#endif
                .mode = NS_BUTTON_INPUT_MODE_INPUT,
                .flag = cfg->joulescope_trigger_flag,
            };
        }
        input_count = idx;
    } else {
        for (idx = 0; idx < input_count; ++idx) {
            inputs[idx] = cfg->inputs[idx];
        }
    }

    if (input_count == 0) {
        return NS_STATUS_SUCCESS;
    }

    AM_CRITICAL_BEGIN
    am_hal_gpio_interrupt_irq_status_get(GPIO0_001F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_001F_IRQn, GpioIntMask);
    am_hal_gpio_interrupt_irq_status_get(GPIO0_203F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_203F_IRQn, GpioIntMask);
    am_hal_gpio_interrupt_irq_status_get(GPIO0_405F_IRQn, false, &GpioIntMask);
    am_hal_gpio_interrupt_irq_clear(GPIO0_405F_IRQn, GpioIntMask);
    AM_CRITICAL_END

    for (idx = 0; idx < input_count; ++idx) {
        if (!inputs[idx].enable) {
            continue;
        }
        if (inputs[idx].flag == NULL) {
            return NS_STATUS_INVALID_CONFIG;
        }

        gpio_num = inputs[idx].gpio_num;
        g_ns_peripheral_button_flags[idx] = inputs[idx].flag;

        if (legacy_button_cfg[idx] && gpio_num == AM_BSP_GPIO_BUTTON0) {
            am_hal_gpio_pinconfig(gpio_num, g_AM_BSP_GPIO_BUTTON0);
        } else if (legacy_button_cfg[idx] && gpio_num == AM_BSP_GPIO_BUTTON1) {
            am_hal_gpio_pinconfig(gpio_num, g_AM_BSP_GPIO_BUTTON1);
        } else {
            am_hal_gpio_pincfg_t pin_cfg = ns_button_input_cfg(inputs[idx].mode);
            am_hal_gpio_pinconfig(gpio_num, pin_cfg);
        }

        ui32IntStatus = am_hal_gpio_interrupt_register(
            AM_HAL_GPIO_INT_CHANNEL_0, gpio_num, ns_button_handler_for_index(idx), NULL);
        if (ui32IntStatus != AM_HAL_STATUS_SUCCESS) {
            return ui32IntStatus;
        }
        ui32IntStatus = am_hal_gpio_interrupt_control(
            AM_HAL_GPIO_INT_CHANNEL_0, AM_HAL_GPIO_INT_CTRL_INDV_ENABLE, (void *)&gpio_num);
        if (ui32IntStatus != AM_HAL_STATUS_SUCCESS) {
            return ui32IntStatus;
        }
    }

    NVIC_SetPriority(GPIO0_001F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(GPIO0_203F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_SetPriority(GPIO0_405F_IRQn, AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(GPIO0_001F_IRQn);
    NVIC_EnableIRQ(GPIO0_203F_IRQn);
    NVIC_EnableIRQ(GPIO0_405F_IRQn);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}
