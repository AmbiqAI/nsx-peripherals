# nsx-peripherals

`nsx-peripherals` provides small reusable drivers and helper code for common
board-level peripherals in NSX applications.

Contents:
- power configuration helpers
- button handling helpers
- PSRAM and NVM support where available

The public API is exported from `includes-api/`. This repo is intended to remain
lightweight and board-agnostic where possible.
