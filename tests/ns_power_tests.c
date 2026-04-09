#include "unity/unity.h"
#include "ns_peripherals_power.h"
#include "ns_core.h"

static ns_power_config_t custom_cfg;

static ns_core_config_t core_cfg = {
        .api = &ns_core_V1_0_0,
    };
static void reset_custom_cfg() {
    custom_cfg.perf_mode = NS_PERF_HIGH;
    custom_cfg.api = &ns_power_V1_0_0;
    custom_cfg.need_audadc = true;
    custom_cfg.need_ssram = true;
    custom_cfg.need_crypto = false;
    custom_cfg.need_ble = true;
    custom_cfg.need_usb = true;
    custom_cfg.need_iom = true;
    custom_cfg.need_uart = true;
    custom_cfg.small_tcm = false;
    custom_cfg.need_tempco = false;
    custom_cfg.need_itm = true;
}

void ns_power_tests_pre_test_hook() {
    reset_custom_cfg();
}

void ns_power_tests_post_test_hook() {
    // post hook if needed
}

void ns_power_config_null_test() {
    ns_core_init(&core_cfg);
    uint32_t status = ns_power_config(NULL);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_HANDLE, status);
}

// Init each predefined power setting
void ns_power_config_test() {
    int status = ns_power_config(&ns_power_all_on);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_power_minimal);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    status = ns_power_config(&ns_power_audio);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

}


void ns_power_config_invalid_api_test() {
    custom_cfg.api = NULL;
    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
    ns_core_api_t invalid_api = {
        .apiId = 0,
        .version = NS_CORE_V0_0_1,
    };

    custom_cfg.api = &invalid_api;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(NS_STATUS_INVALID_VERSION, status);
}

// Test each power mode configuration including invalid one
void ns_power_config_power_mode_test() {
    reset_custom_cfg();
    custom_cfg.perf_mode = 0;
    ns_lp_printf("what\n");
    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    custom_cfg.perf_mode = 1;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    custom_cfg.perf_mode = 2;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

    // Not a valid power mode
    custom_cfg.perf_mode = 3;
    status = ns_power_config(&custom_cfg);
    TEST_ASSERT_NOT_EQUAL(AM_HAL_STATUS_SUCCESS, status);

}

void ns_power_config_all_true_test() {
    reset_custom_cfg();
    custom_cfg.need_audadc = true;
    custom_cfg.need_ssram = true;
    custom_cfg.need_crypto = true;
    custom_cfg.need_ble = true;
    custom_cfg.need_usb = true;
    custom_cfg.need_iom = true;
    custom_cfg.need_uart = true;
    custom_cfg.small_tcm = true;
    custom_cfg.need_tempco = true;
    custom_cfg.need_itm = true;

    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}

void ns_power_config_all_false_test() {
    reset_custom_cfg();
    custom_cfg.need_audadc = false;
    custom_cfg.need_ssram = false;
    custom_cfg.need_crypto = false;
    custom_cfg.need_ble = false;
    custom_cfg.need_usb = false;
    custom_cfg.need_iom = false;
    custom_cfg.need_uart = false;
    custom_cfg.small_tcm = false;
    custom_cfg.need_tempco = false;
    custom_cfg.need_itm = false;

    uint32_t status = ns_power_config(&custom_cfg);
    TEST_ASSERT_EQUAL(AM_HAL_STATUS_SUCCESS, status);
}
