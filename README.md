# Women Safety Smartwatch

An ESP32-based emergency safety system designed to provide quick assistance during dangerous situations.

## Features

- SOS emergency button
- GSM SMS alert
- Emergency phone call
- GPS location tracking
- OLED status display
- Emergency buzzer
- LED emergency indicator
- Automatic emergency response

## Hardware Used

- ESP32 Development Board
- SIM900A GSM Module
- GPS Module
- OLED Display
- Push Button
- Buzzer
- LED
- Resistor
- Power Supply

## System Flow

SOS Button
↓
Emergency Activated
↓
GPS Location Obtained
↓
Emergency SMS Sent
↓
Emergency Call
↓
Buzzer + LED Activated

## Pin Connections

### GSM

| GSM | ESP32 |
|---|---|
| TX | GPIO 27 |
| RX | GPIO 26 |
| GND | GND |

### GPS

| GPS | ESP32 |
|---|---|
| TX | GPIO 16 |
| RX | GPIO 17 |
| GND | GND |
| VCC | Appropriate supply |

### OLED

| OLED | ESP32 |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| GND | GND |
| VCC | 3.3V |

### SOS Button

| Component | ESP32 |
|---|---|
| Button | GPIO 4 |
| Other pin | GND |

### LED
GPIO 13 → LED → 220Ω resistor → GND

Buzzer
GPIO 23 -> BUZZER->GND
