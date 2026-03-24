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

static const am_hal_gpio_pincfg_t *ns_button_input_cfg(ns_button_input_mode_t mode) {
    if (mode == NS_BUTTON_INPUT_MODE_INPUT_PULLUP) {
        return &g_AM_HAL_GPIO_INPUT_PULLUP;
    }
    return &g_AM_HAL_GPIO_INPUT;
}

/**
 * @brief GPIO Button0 ISR handler
 *
 */
void am_gpio_isr(void) {
    // Read and clear the GPIO interrupt status.
#if defined(AM_PART_APOLLO3P)
    AM_HAL_GPIO_MASKCREATE(GpioIntStatusMask);
    am_hal_gpio_interrupt_status_get(false, pGpioIntStatusMask);
    am_hal_gpio_interrupt_clear(pGpioIntStatusMask);
    am_hal_gpio_interrupt_service(pGpioIntStatusMask);
#elif defined(AM_PART_APOLLO3)
    uint64_t ui64Status;
    am_hal_gpio_interrupt_status_get(false, &ui64Status);
    am_hal_gpio_interrupt_clear(ui64Status);
    am_hal_gpio_interrupt_service(ui64Status);
#else
    #error Unknown device.
#endif
}

uint32_t ns_button_platform_init(ns_button_config_t *cfg) {
    uint32_t input_count = 0;
    uint32_t idx = 0;
    ns_button_input_t inputs[NS_BUTTON_MAX_INPUTS] = {0};
    bool legacy_button_cfg[NS_BUTTON_MAX_INPUTS] = {false};

    input_count = cfg->input_count;
    if (input_count > NS_BUTTON_MAX_INPUTS) {
        return NS_STATUS_INVALID_CONFIG;
    }

    if (input_count == 0) {
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
        if (cfg->joulescope_trigger_enable && idx < NS_BUTTON_MAX_INPUTS) {
            inputs[idx++] = (ns_button_input_t){
                .enable = true,
                .gpio_num = 24,
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

    for (idx = 0; idx < input_count; ++idx) {
        uint32_t gpio_num = 0;
        if (!inputs[idx].enable) {
            continue;
        }
        if (inputs[idx].flag == NULL) {
            return NS_STATUS_INVALID_HANDLE;
        }

        gpio_num = inputs[idx].gpio_num;
        g_ns_peripheral_button_flags[idx] = inputs[idx].flag;

        if (legacy_button_cfg[idx] && gpio_num == AM_BSP_GPIO_BUTTON0) {
            am_hal_gpio_pinconfig(gpio_num, g_AM_BSP_GPIO_BUTTON0);
        } else if (legacy_button_cfg[idx] && gpio_num == AM_BSP_GPIO_BUTTON1) {
            am_hal_gpio_pinconfig(gpio_num, g_AM_BSP_GPIO_BUTTON1);
        } else {
            am_hal_gpio_pinconfig(gpio_num, *ns_button_input_cfg(inputs[idx].mode));
        }

        AM_HAL_GPIO_MASKCREATE(GpioIntMask);
        am_hal_gpio_interrupt_register(gpio_num, ns_button_handler_for_index(idx));
        am_hal_gpio_interrupt_clear(AM_HAL_GPIO_MASKBIT(pGpioIntMask, gpio_num));
        am_hal_gpio_interrupt_enable(AM_HAL_GPIO_MASKBIT(pGpioIntMask, gpio_num));
    }

    NVIC_EnableIRQ(GPIO_IRQn);

    am_hal_interrupt_master_enable();
    return NS_STATUS_SUCCESS;
}
