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

const ns_core_api_t ns_button_V0_0_1 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_V1_0_0 = {.apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

const ns_core_api_t ns_button_oldest_supported_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V0_0_1};

const ns_core_api_t ns_button_current_version = {
    .apiId = NS_BUTTON_API_ID, .version = NS_BUTTON_V1_0_0};

int volatile *g_ns_peripheral_button_flags[NS_BUTTON_MAX_INPUTS];

static void ns_button_raise_flag(uint32_t index) {
    if (index >= NS_BUTTON_MAX_INPUTS) {
        return;
    }
    if (g_ns_peripheral_button_flags[index] != NULL) {
        *g_ns_peripheral_button_flags[index] = 1;
    }
}

void ns_button_input_handler_0(void *pArg) { ns_button_raise_flag(0); }
void ns_button_input_handler_1(void *pArg) { ns_button_raise_flag(1); }
void ns_button_input_handler_2(void *pArg) { ns_button_raise_flag(2); }

extern uint32_t ns_button_platform_init(ns_button_config_t *cfg);

uint32_t ns_peripheral_button_init(ns_button_config_t *cfg) {
    uint32_t ui32IntStatus = NS_STATUS_SUCCESS;
    uint32_t i = 0;

#ifndef NS_DISABLE_API_VALIDATION
    if (cfg == NULL) {
        return NS_STATUS_INVALID_HANDLE;
    }

    if (ns_core_check_api(
            cfg->api, &ns_button_oldest_supported_version, &ns_button_current_version)) {
        return NS_STATUS_INVALID_VERSION;
    }
#endif
    for (i = 0; i < NS_BUTTON_MAX_INPUTS; ++i) {
        g_ns_peripheral_button_flags[i] = NULL;
    }
    ui32IntStatus = ns_button_platform_init(cfg);

    return ui32IntStatus;
}
