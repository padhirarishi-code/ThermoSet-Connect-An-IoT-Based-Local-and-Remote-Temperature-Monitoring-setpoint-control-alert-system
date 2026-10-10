# ThermoSet Connect

### An IoT-Based Local and Remote Temperature Monitoring, Set-Point Control, and Alert System

**Author:** Rishi Sai Kumar Padira
**MCU:** LPC2148 (ARM7)
**Cloud Platform:** ThingSpeak

---

## 1. Objective

ThermoSet Connect continuously monitors temperature and lets the user configure a
temperature **set point** two ways — **locally** via a 4x4 keypad, or **remotely**
via the cloud. The set point is stored in EEPROM so it survives a power cycle, and
the system raises both a **local alert** (buzzer) and a **cloud alert** (ThingSpeak)
whenever the measured temperature exceeds it. Useful for laboratories, server rooms,
cold-storage facilities and industrial equipment.

## 2. Features

- Live temperature display (LM35 + onboard ADC) alongside time/date (onboard RTC)
- Temperature uploaded to ThingSpeak automatically every few minutes
- **Local** set-point entry via switch + keypad menu
- **Remote** set-point entry via a dedicated ThingSpeak channel, polled automatically
- Set point persists in EEPROM -- survives power loss
- Local buzzer alert + cloud alert when temperature exceeds the set point
- Scrolling welcome message on boot

## 3. Hardware Requirements

| Component | Purpose |
|---|---|
| LPC2148 (ARM7 microcontroller) | Main controller |
| LM35 | Temperature sensor |
| AT24C256 (I2C EEPROM) | Set-point persistence |
| 4x4 matrix keypad | Local set-point/time entry |
| Push switch | Opens the local menu (EINT0) |
| 16x2 LCD (8-bit mode) | Display |
| Buzzer | Local alert |
| ESP01 (ESP8266) WiFi module | Cloud connectivity |
| USB-UART converter / DB-9 cable | Flashing + debugging |

## 4. Software Requirements

- Keil uVision (C51/ARM) -- project build
- Flash Magic -- flashing the compiled .hex to the LPC2148
- A ThingSpeak account (free tier) with two channels (see Section 6)

## 5. Circuit / Pin Connections

| Component | Signal | LPC2148 Pin | Notes |
|---|---|---|---|
| **LCD** | RS | P0.18 | control |
| | RW | P0.20 | control |
| | EN | P0.19 | control |
| | D0-D7 | P0.8 - P0.15 | 8-bit mode, all 8 data lines required |
| **Keypad (4x4)** | Row0-Row3 | P1.16 - P1.19 | outputs |
| | Col0-Col3 | P1.20 - P1.23 | inputs |
| **LM35** | OUT | P0.28 (AIN1) | ADC channel 1 |
| **EEPROM (AT24C256)** | SCL | P0.2 | I2C0 |
| | SDA | P0.3 | I2C0 |
| **Switch** | signal | P0.16 | EINT0, falling-edge |
| **Buzzer** | signal | P0.30 | GPIO output |
| **ESP01** | TX -> LPC2148 RX | P0.1 (RXD0) | cross TX/RX |
| | RX <- LPC2148 TX | P0.0 (TXD0) | UART0 @ 9600 baud |
| **RTC** | -- | internal | built into the LPC2148, no external wiring |

> *(Add your own circuit diagram / breadboard photo here -- see Section 11.)*

## 6. ThingSpeak Setup

Create **two** ThingSpeak channels:

1. **Main data channel** -- Field1: Temperature, Field2: Alert temperature, Field3: Current set point (device **writes** here).
2. **Set-point entry channel** -- Field1: the set point you want the device to adopt (device **reads** here; you write to it from your phone/PC to change the limit remotely).

## 7. Configuration

Before building, open `esp01.h` and fill in:

| Macro | What it's for |
|---|---|
| `WIFI_SSID`, `WIFI_PASSWORD` | Your WiFi network credentials |
| `TS_WRITE_API_KEY` | Write API key of the main data channel |
| `SP_CHANNEL_ID`, `SP_READ_API_KEY` | Channel ID + Read API key of the set-point entry channel |

> If you publish this repo publicly, replace your real WiFi/ThingSpeak keys in
> `esp01.h` with placeholders first, so credentials aren't exposed on GitHub.

## 8. Project Structure

```
ThermoSet_Connect/
|-- main.c                      -- main application logic
|-- adc.c / adc.h                -- LM35 reading via ADC
|-- lcd.c / lcd_defines.h        -- LCD driver
|-- cust_lcd.c / cust_lcd.h      -- custom LCD character
|-- lcd_display.c / lcd_display.h / menu.h -- set-point/time menu, numeric keypad entry
|-- keypad.c / keypad_defines.h  -- 4x4 matrix keypad driver
|-- rtc.c / rtc.h                -- onboard RTC
|-- i2c.c / i2c.h                -- I2C bus driver (EEPROM)
|-- eeprom.c / eeprom.h          -- AT24C256 byte read/write
|-- uart.c / uart.h              -- UART0 driver + ISR (ESP01 communication)
|-- esp01.c / esp01.h            -- ESP01 AT-command driver + ThingSpeak upload/read
|-- esp01_call.c                 -- init sequence, upload wrapper, cloud set-point sync
|-- interrupt.c / interrupt.h    -- EINT0 (local set-point switch)
|-- delay.c / delay.h
|-- clock.h
`-- README.md
```

## 9. How It Works

**Startup:** peripherals initialize, a welcome message scrolls on the LCD, the set
point is read back from EEPROM (defaulting to 32C only if EEPROM is blank/invalid),
then the ESP01 joins WiFi.

**Main loop:**
- Temperature + time/date refresh continuously on the LCD.
- Every few minutes, temperature uploads to the ThingSpeak main channel.
- Every couple of minutes, the set-point entry channel is checked; a changed value is
  adopted and saved to EEPROM.
- Pressing the switch opens a keypad menu to edit time/date, set a new local set
  point, or view the current one.
- When temperature exceeds the set point: the buzzer blinks locally, and a cloud
  alert is sent once per over-threshold event.

## 10. Build & Flash

1. Open the project in Keil, targeting LPC2148, with all .c files added.
2. Build (0 errors).
3. Flash the generated .hex with Flash Magic over the USB-UART/DB-9 connection
   (ESP01 disconnected from UART0 during flashing).
4. Reconnect the ESP01, power on, and confirm the LCD shows the welcome message,
   then live temperature/time, then "WiFi connected".

## 11. BLOCK DIAGRAM
<img width="831" height="557" alt="Screenshot 2026-10-11 000418" src="https://github.com/user-attachments/assets/3ba33abd-49ec-4224-bf04-4e81868e9bdf" />
<img width="1280" height="960" alt="thermoset" src="https://github.com/user-attachments/assets/d577941c-3cd1-4dec-8257-bc455fe96909" />
<img width="1280" height="960" alt="ESP01" src="https://github.com/user-attachments/assets/a665e371-fbea-487c-b283-861c7d04c4b1" />
<img width="1280" height="960" alt="AT24C256" src="https://github.com/user-attachments/assets/b918725c-e19e-4ba3-936c-e1da112200ef" />


## 12. Notes on Development

This build started from a reference IoT data-logger codebase and was adapted and
debugged specifically for this spec -- including fixing the set point not persisting
across reboots, implementing the remote/cloud set-point path from scratch, and fixing
several ESP01 AT-command reliability issues found during real hardware testing
(waiting for the '>' prompt correctly, a response-string typo, and stale-connection
cleanup before retries).
