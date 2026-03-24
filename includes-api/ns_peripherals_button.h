//*****************************************************************************
//
//! @file button.h
//!
//! @brief Utility for reading board buttons and GPIO-backed trigger inputs.
//!
//! Purpose: Reading board buttons and other simple GPIO-backed inputs.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2022, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_4_0_1-bef824fa27 of the AmbiqSuite Development Package.
//
//*****************************************************************************
#ifndef NS_PERIPHERAL_BUTTON
#define NS_PERIPHERAL_BUTTON

#ifdef __cplusplus
extern "C" {
#endif
#include "ns_core.h"

#define NS_BUTTON_V0_0_1                                                                           \
    { .major = 0, .minor = 0, .revision = 1 }
#define NS_BUTTON_V1_0_0                                                                           \
    { .major = 1, .minor = 0, .revision = 0 }
#define NS_BUTTON_OLDEST_SUPPORTED_VERSION NS_BUTTON_V0_0_1
#define NS_BUTTON_CURRENT_VERSION NS_BUTTON_V1_0_0

extern const ns_core_api_t ns_button_V0_0_1;
extern const ns_core_api_t ns_button_V1_0_0;
extern const ns_core_api_t ns_button_oldest_supported_version;
extern const ns_core_api_t ns_button_current_version;
#define NS_BUTTON_API_ID 0xCA0003

typedef enum {
    NS_BUTTON_INPUT_MODE_INPUT = 0,
    NS_BUTTON_INPUT_MODE_INPUT_PULLUP = 1,
} ns_button_input_mode_t;

typedef struct {
    bool enable;                ///< Enable this input
    uint32_t gpio_num;          ///< GPIO number to monitor
    ns_button_input_mode_t mode; ///< Basic input pin configuration
    int volatile *flag;         ///< Flag set to 1 when the input fires
} ns_button_input_t;

#define NS_BUTTON_MAX_INPUTS 3

typedef struct {
    ns_core_api_t const *api;              ///< API prefix
    uint32_t input_count;                  ///< Number of entries in `inputs` to use
    ns_button_input_t inputs[NS_BUTTON_MAX_INPUTS]; ///< Generic input definitions
    bool button_0_enable;                  ///< Deprecated compatibility field
    bool button_1_enable;                  ///< Deprecated compatibility field
    bool joulescope_trigger_enable;        ///< Deprecated compatibility field
    int volatile *button_0_flag;           ///< Deprecated compatibility field
    int volatile *button_1_flag;           ///< Deprecated compatibility field
    int volatile *joulescope_trigger_flag; ///< Deprecated compatibility field
} ns_button_config_t;

/**
 * @brief Initialize button monitoring per the configuration struct
 *
 * @param ns_button_config_t* : button configuration struct
 *
 * @return uint32_t
 */
extern uint32_t ns_peripheral_button_init(ns_button_config_t *);

#ifdef __cplusplus
}
#endif

#endif // NS_PERIPHERAL_BUTTON
