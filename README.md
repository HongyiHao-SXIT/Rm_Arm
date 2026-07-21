# Rm_Arm — LeArm Intelligent Robotic Arm System

A comprehensive open-source robotic arm control platform built around the **Hiwonder LeArm** 6-DOF robotic arm, powered by the **ESP32** microcontroller. This repository includes low-level firmware, multi-mode control interfaces, inverse kinematics, computer vision, voice interaction, chassis extensions, and a rich set of application demos.

## Table of Contents

- [Overview](#overview)
- [Repository Structure](#repository-structure)
- [Hardware](#hardware)
- [Features](#features)
- [Control Modes](#control-modes)
- [Getting Started](#getting-started)
- [Dependencies](#dependencies)
- [Pin Configuration](#pin-configuration)
- [Action Groups](#action-groups)
- [Kinematics](#kinematics)
- [Application Modules](#application-modules)
- [License](#license)

---

## Overview

Rm_Arm provides a full-stack solution for controlling the **Hiwonder LeArm**, a 6-degree-of-freedom desktop robotic arm. The firmware runs on an **ESP32** and supports **six distinct control modes**, including Bluetooth (mobile APP), PC host computer (BLE serial), PS2 wireless gamepad, offline pre-programmed action playback, WiFi TCP remote control, and glove-based teleoperation.

The system is extended with **AI vision** (color detection/tracking, face recognition, tag classification), **voice control**, **conveyor belt coordination**, **Mecanum/tracked chassis integration**, and more — all organized as modular functional units.

The vision pipeline (color blob detection) runs on an external camera MCU (e.g., OpenMV / MaixPy) and communicates with the ESP32 via **I2C**, sending real-time target coordinates.

---

## Repository Structure

```
Rm_Arm/
├── Infra_code/                    # Core ESP32 firmware (PlatformIO project)
│   ├── platformio.ini             # PlatformIO configuration (ESP32, Arduino framework)
│   ├── main.py                    # Vision pipeline (OpenMV/Hiwonder camera) — color blob detection + I2C tx
│   └── src/
│       ├── Config.h               # Hardware pin definitions, servo type selection
│       ├── Hiwonder.hpp           # Peripheral drivers: LED, Buzzer, Button, Battery, Flash, Servos
│       ├── Robot_arm.hpp          # LeArm high-level API: coordinate control, actions, claw/roll
│       ├── LeArm_ESP32_Arduino_factory260227.ino  # Main firmware entry point
│       ├── IIC.cpp/hpp            # I2C slave communication (receives vision data)
│       ├── robot_arm/
│       │   ├── kinematics.cpp/hpp # Forward & inverse kinematics solver
│       │   ├── Servo.cpp/h        # PWM servo driver
│       │   ├── SerialServo.cpp    # Serial bus servo (Lobot protocol) driver
│       │   ├── Pwmservo.cpp       # PWM servo control layer
│       │   ├── Robot_arm.cpp      # Arm action engine, coordinate control, offset management
│       │   └── Flash_ctl.cpp      # SPI Flash read/write/erase for action group storage
│       ├── PC_BLE/                # PC / BLE (Bluetooth) control protocol
│       ├── PS2/                   # PS2 wireless joystick driver
│       ├── WIFI/                  # WiFi AP + TCP server control
│       ├── battery/               # Battery voltage monitoring
│       ├── buzzer/                # Buzzer driver
│       ├── key/                   # Button input driver
│       └── led/                   # LED driver
│
└── Functions/                     # Application demo modules
    ├── AI_Vision/                 # Computer vision applications
    │   ├── Color_Sorting/         # Color-based object sorting
    │   ├── Color_Tracking/        # Real-time color tracking
    │   ├── Face_Recognition/      # Face recognition
    │   ├── Face_Tracking/         # Face tracking
    │   ├── Tag_Classification/    # AprilTag / QR code classification
    │   └── Waste_Sorting/         # AI waste sorting system
    ├── AI_Voice/                  # Voice interaction applications
    │   ├── Ultrasonic_Alert/      # Ultrasonic distance alert
    │   ├── Voice_Control/         # Voice command control
    │   ├── Voice_Vision_Color_Sorting/  # Voice + Vision color sorting
    │   └── Voice_Vision_Integrated/     # Full voice + vision integration
    ├── Arm_Conveyor/              # Conveyor belt systems
    │   ├── Conveyor_Dual_LeArm/   # Dual-arm conveyor coordination
    │   └── Joystick_Remote/       # Joystick remote for conveyor
    ├── Arm_Mecanum_Chassis/       # Mecanum wheel omnidirectional chassis
    │   ├── APP_Control/           # Mobile APP control
    │   └── Joystick_Remote/       # PS2 joystick remote
    ├── Arm_Slide_Rail/            # Linear slide rail extensions
    │   ├── Joystick_Remote/       # Joystick remote for slide rail
    │   └── Slide_Rail_Color_Sorting/  # Slide rail color sorting demo
    ├── Arm_Tracked_Chassis/       # Tracked chassis systems
    │   ├── APP_Control/           # Mobile APP control
    │   └── Joystick_Control/      # PS2 joystick control
    ├── Basic_Dev/                 # Basic development tutorials & demos
    │   ├── Action_Group/          # Action group recording/playback
    │   ├── APP_Control/           # BLE APP control basics
    │   ├── Button_Control/        # Physical button interaction
    │   ├── Buzzer_Control/        # Buzzer usage
    │   ├── Host_Computer_Control/ # PC host computer serial control
    │   ├── Joystick_Control/      # PS2 joystick basics
    │   ├── Kinematics/            # Inverse kinematics demo
    │   ├── LED_Blink/             # LED control
    │   ├── PWM_Servo_Control/     # PWM servo direct control
    │   └── Serial_Servo_Control/  # Serial bus servo direct control
    ├── Glove_Control/             # Gesture glove teleoperation
    ├── Secondary_Projects/        # Additional peripheral projects
    │   ├── IMU_Control/           # IMU (inertial) control
    │   ├── IR_Control/            # Infrared remote control
    │   ├── OLED_Display/          # OLED screen display
    │   ├── Touch_Control/         # Touch sensor control
    │   ├── Ultrasonic_Ranging/    # Ultrasonic distance sensing
    │   └── Voice_Control/         # Standalone voice control
    ├── Serial_Communication/      # Serial (UART) communication examples
    └── Sync_Teach_Pendant/        # Synchronized teach pendant (master-slave)
```

---

## Hardware

| Component             | Description                              |
| --------------------- | ---------------------------------------- |
| **MCU**               | ESP32 (ESP32-DevKitC or equivalent)      |
| **Robotic Arm**       | Hiwonder LeArm, 6-DOF                    |
| **Servos**            | Serial bus servos (Lobot protocol) or PWM servos (configurable) |
| **Vision Camera**     | OpenMV Cam / MaixPy (Hiwonder) with I2C output |
| **PS2 Receiver**      | Wireless PS2 gamepad receiver            |
| **BLE Module**        | Onboard BLE for mobile APP / PC control  |
| **WiFi**              | ESP32 built-in WiFi (AP mode)            |
| **SPI Flash**         | External flash for action group storage  |

---

## Features

- **6-DOF full inverse kinematics** — Control the end-effector by Cartesian coordinates (x, y, z, pitch)
- **6 control modes** — Bluetooth APP, PC host, PS2 joystick, offline playback, WiFi TCP, gesture glove
- **Dual servo support** — Works with both PWM servos and Lobot serial bus servos
- **Action group system** — Record, save to Flash, and replay complex motion sequences
- **Servo offset calibration** — Persistent per-servo offset adjustment stored in Flash
- **AI vision pipeline** — Color blob detection with coordinate transmission via I2C
- **Modular driver architecture** — Cleanly separated peripheral drivers for LED, buzzer, button, battery, flash
- **Extensive application library** — 30+ demo projects covering vision, voice, chassis, conveyor, and more

---

## Control Modes

Toggle between modes using the physical button on the ESP32 board. Each mode is indicated by a distinct LED blink pattern.

| Mode | Button Press | LED Pattern              | Description                                     |
| ---- | ------------ | ------------------------ | ----------------------------------------------- |
| 0    | Default      | Blink 2000ms on/off      | **APP Mode** — BLE control via mobile app        |
| 1    | Press once   | Blink 1000ms on/off      | **PC Mode** — Host computer BLE serial control   |
| 2    | Press twice  | Blink 200ms on/off       | **PS2 Mode** — Wireless PS2 gamepad control      |
| 3    | Press 3 times| Blink 50ms on/off        | **Offline Mode** — Pre-programmed action playback|
| 4    | Press 4 times| Blink 100ms on, 900ms off| **WiFi Mode** — TCP server remote control        |

---

## Getting Started

### Prerequisites

- [PlatformIO IDE](https://platformio.org/) (VS Code extension recommended) or [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 board support
- USB-to-Serial driver (CP210x or CH340)

### Build & Upload Firmware

```bash
# Clone the repository
git clone <repo-url>
cd Rm_Arm/Infra_code

# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Open serial monitor
pio device monitor --baud 115200
```

### Vision Module Setup

The `main.py` script runs on an OpenMV/Hiwonder camera module. Flash it via the camera's IDE (OpenMV IDE or MaixPy IDE). It detects 5 colors (Red, Green, Blue, Yellow, Purple) and transmits blob coordinates to the ESP32 over I2C.

### Servo Type Selection

Edit `src/Config.h` to select your servo type:

```cpp
#define SERVO_TYPE  TYPE_SERIAL_SERVO  // For Lobot serial bus servos
// #define SERVO_TYPE  TYPE_PWM_SERVO  // For PWM servos
```

---

## Dependencies

Defined in `platformio.ini`:

| Library        | Version  | Purpose                       |
| -------------- | -------- | ----------------------------- |
| ArduinoJson    | ^6.21.3  | JSON serialization/parsing    |
| PubSubClient   | ^2.8     | MQTT client (WiFi mode)       |
| ESP32Servo     | ^0.12.0  | PWM servo control on ESP32    |

---

## Pin Configuration

All pin definitions are centralized in `Infra_code/src/Config.h`:

| Function       | ESP32 Pin |
| -------------- | --------- |
| Servo 1        | GPIO 19   |
| Servo 2        | GPIO 18   |
| Servo 3        | GPIO 5    |
| Servo 4        | GPIO 4    |
| Servo 5        | GPIO 0    |
| Servo 6 (Claw) | GPIO 15   |
| I2C SDA        | GPIO 17   |
| I2C SCL        | GPIO 16   |
| BLE TX         | GPIO 1    |
| BLE RX         | GPIO 3    |
| Bus Servo TX   | GPIO 12   |
| Bus Servo RX   | GPIO 35   |
| Bus Servo EN   | GPIO 14   |
| Battery ADC    | GPIO 39   |
| Button         | GPIO 36   |
| LED            | GPIO 26   |
| Buzzer         | GPIO 27   |
| USB TX         | GPIO 34   |
| Flash CLK      | GPIO 23   |
| Flash DI       | GPIO 22   |
| Flash DO       | GPIO 21   |
| Flash CS       | GPIO 2    |
| BLE Power CTL  | GPIO 25   |

**WiFi AP Settings:**
- SSID: `LeArm-AP`
- Password: `12345678`
- TCP Port: `5000`

---

## Action Groups

The action group system allows recording and replaying complex multi-joint motion sequences:

- **Up to 255 action groups** stored in external SPI Flash
- Each group occupies **8 KB** and can contain up to **255 action frames**
- Each frame stores all 6 servo positions plus timing (21 bytes per frame)
- Actions can be triggered programmatically, via button (offline mode), or remotely over BLE/WiFi/PS2
- Functions: `action_group_run()`, `action_group_save()`, `action_group_stop()`, `action_group_erase()`

---

## Kinematics

The arm implements a 4-link inverse kinematics solver (`robot_arm/kinematics.cpp`):

- **Forward Kinematics** (`fkine`): Given joint angles (θ1–θ4), compute end-effector (x, y, z)
- **Inverse Kinematics** (`ikine`): Given target (x, y, z, α), solve for joint angles
- Joint angle constraints are enforced to prevent invalid positions
- The base rotation (joint 1) maps to x/y plane, while joints 2–4 control height and reach
- Independent claw (open/close, 0°–90°) and roll (wrist rotation, -90°–90°) control

---

## Application Modules

### AI Vision
Color tracking, face detection/recognition, AprilTag classification, and waste sorting — all leveraging the I2C vision pipeline with the camera module.

### AI Voice
Voice command recognition integrated with robotic arm control, including combined voice+vision scenarios for interactive color sorting.

### Chassis Extensions
- **Mecanum Chassis**: Omnidirectional movement with APP or joystick remote control
- **Tracked Chassis**: Tank-style tracked base with APP or joystick control
- **Slide Rail**: Linear rail extension for extended workspace color sorting
- **Conveyor Belt**: Dual-arm + conveyor coordination for automated sorting lines

### Basic Development
Step-by-step tutorials covering each subsystem: LED, buzzer, button, servos, kinematics, action groups, and all control modes (APP, PC, PS2).

### Secondary Projects
Peripheral integrations: IMU gesture control, IR remote, OLED display feedback, touch sensors, ultrasonic ranging, and standalone voice modules.

### Glove Control
Wearable gesture glove for intuitive teleoperation of the robotic arm.

### Teach Pendant
Synchronized master-slave teach pendant for recording and precisely replaying arm movements.

---

## License

Copyright (c) 2024 Hiwonder. All rights reserved.

This project is provided for educational and development purposes with the Hiwonder LeArm platform.