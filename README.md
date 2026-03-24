# nsx-peripherals

`nsx-peripherals` provides small reusable drivers and helper code for common
board-level peripherals in NSX applications.

Contents:
- power configuration helpers
- GPIO-backed button and trigger helpers
- PSRAM and NVM support where available

The public API is exported from `includes-api/`. This repo is intended to remain
lightweight and board-agnostic where possible.

For button monitoring, prefer the generic GPIO input API:

```c
volatile int user_button_flag = 0;

ns_button_config_t button_cfg = {
    .api = &ns_button_V1_0_0,
    .input_count = 1,
    .inputs = {
        {
            .enable = true,
            .gpio_num = AM_BSP_GPIO_BUTTON0,
            .mode = NS_BUTTON_INPUT_MODE_INPUT_PULLUP,
            .flag = &user_button_flag,
        },
    },
};

ns_peripheral_button_init(&button_cfg);
```

Deprecated compatibility fields such as `button_0_enable` and
`joulescope_trigger_enable` still exist for migration, but they are no longer
the preferred API shape.
