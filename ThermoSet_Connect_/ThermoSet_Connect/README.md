# ThermoSet Connect
### An IoT-Based Local and Remote Temperature Monitoring, Set-Point Control, and Alert System

## Objective
ThermoSet Connect continuously monitors temperature, supports **local (keypad)** and
**remote (cloud-based)** set-point configuration, stores the set point in EEPROM so it
survives a power cycle, and raises local + cloud alerts when the temperature exceeds
the configured limit. Useful for laboratories, server rooms, cold-storage facilities
and industrial equipment.

## Hardware Requirements
- LPC2148 (ARM7 microcontroller)
- LM35 temperature sensor
- AT24C256 EEPROM (I2C)
- 4x4 matrix keypad
- Switch (triggers the local set-point edit menu via EINT0)
- 16x2 LCD
- Buzzer
- ESP01 WiFi module
- DB-9 cable / USB-UART converter

## Software Requirements
- Keil C Compiler
- Embedded C
- Flash Magic

## Project Structure
```
ThermoSet_Connect/
├── main.c              -- main application logic
├── adc.c / adc.h        -- LM35 reading via ADC
├── lcd.c                -- LCD driver
├── lcd_defines.h
├── cust_lcd.c / .h      -- custom LCD character
├── lcd_display.c        -- set-point/time edit menu + numeric keypad entry
├── menu.h
├── keypad.c / keypad_defines.h  -- 4x4 matrix keypad driver
├── rtc.c / rtc.h        -- onboard RTC
├── i2c.c / i2c.h        -- I2C bus driver (EEPROM + RTC)
├── eeprom.c / eeprom.h  -- AT24C256 byte/page read-write
├── uart.c / uart.h      -- UART0 driver + ISR (ESP01 communication)
├── esp01.c              -- ESP01 AT-command driver + ThingSpeak upload/read
├── esp01_call.c         -- init sequence, upload wrapper, cloud set-point sync
├── esp01.h              -- WiFi/ThingSpeak config (edit before building!)
├── interrupt.c / interrupt.h -- EINT0 (local set-point switch)
├── delay.c / delay.h
├── clock.h
└── README.md
```

## Before You Build
Open **esp01.h** and fill in your own values:

| Macro | What it's for |
|---|---|
| `WIFI_SSID`, `WIFI_PASSWORD` | Your WiFi network credentials |
| `TS_WRITE_API_KEY` | Write API key of your **main data channel** (temperature, alerts, set-point log) |
| `SP_CHANNEL_ID`, `SP_READ_API_KEY` | Channel ID + Read API key of a **second, dedicated channel** used only for remote set-point entry |

Create two ThingSpeak channels:
1. **Main data channel** — Field1: Temperature, Field2: Alert temperature, Field3: Current set point.
2. **Set-point entry channel** — Field1: the set point you want the device to adopt. Update this field from your phone/PC whenever you want to change the limit remotely.

## How It Works

### Startup
1. All peripherals are initialized (LCD, UART, ADC, RTC, I2C, keypad, EINT0).
2. The set point is **read back from EEPROM**. If EEPROM has never been written
   (or holds an out-of-range value), a default of 32°C is used and saved.
3. ESP01 is initialized and joined to the configured WiFi network.

### Main Loop
- Reads the LM35 temperature and displays it, along with the RTC time/date, on the LCD.
- **Every `TEMP_UPLOAD_INTERVAL` minutes (default 3):** uploads the current temperature
  to Field1 of the main data channel.
- **Every `SP_POLL_INTERVAL` minutes (default 2):** reads Field1 of the set-point entry
  channel. If it differs from the set point currently in use, the new value is adopted
  and written to EEPROM — this is the "remote set point" path, and it deliberately does
  *not* poll on every loop iteration, to avoid flooding the cloud with requests.
- **Local set point:** pressing the switch fires an EINT0 interrupt; the main loop then
  opens a keypad menu (`1.EDIT` time/date, `2.SP` set a new set point, `3.DISPSP` view
  the current set point, `4.EXIT`). A new set point entered this way is written straight
  to EEPROM and logged to the cloud (Field3).
- **Alert:** whenever temperature exceeds the set point, the buzzer blinks a short
  pattern on every loop pass, and a cloud alert (Field2) is sent once, on the rising
  edge of the condition (not repeatedly while it stays exceeded).

## Enhancements Made to the Reference Code
This build started from a reference "Cloud-Connected Environmental Data Logger"
codebase and was adapted specifically for the ThermoSet Connect spec:
- Removed the MQ-2 gas/smoke sensor logic (not part of this spec) and its buzzer/interrupt wiring.
- The set point is now **actually read back from EEPROM on boot** (previously hardcoded to 32 every time).
- Added `esp01_readThingspeakField()` and `sync_cloud_setpoint()` to implement the
  **remote/cloud set-point path** using a dedicated ThingSpeak channel, polled on its own timer.
- Removed a leftover debug-simulation shortcut and a stray syntax error in the ESP01 driver
  that would have prevented the AT-command handshake from ever really running.
- Reworked field usage so upload/read purposes are unambiguous (Field1 = temperature,
  Field2 = alert, Field3 = set point log on the main channel; separate channel for remote entry).
- Buzzer alert changed from "stays on continuously" to a blink pattern, and cloud alert
  upload is now edge-triggered instead of re-sent on every loop iteration.

## Build Steps (Keil)
1. Create a new Keil project targeting LPC2148, add all the `.c` files above.
2. Build. Fix any include-path issues if your Keil setup differs.
3. Flash using Flash Magic over the USB-UART converter.
4. Power up, confirm LCD shows time/date + temperature, and that "WiFi connected"
   appears after boot.
5. Test locally: press the switch, use option `2.SP` to change the set point, confirm
   it survives a power cycle.
6. Test remotely: update Field1 on your set-point entry channel from ThingSpeak, wait
   up to `SP_POLL_INTERVAL` minutes, and confirm the LCD/EEPROM adopts the new value.

*** ALL THE BEST ***
