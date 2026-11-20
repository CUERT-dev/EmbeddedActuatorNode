# CUERT — Embedded Actuator Node

![MCU](https://img.shields.io/badge/MCU-STM32G431RBT6-blue)
![Board](https://img.shields.io/badge/Board-NUCLEO--G431RB-007EC6)
![CI](https://img.shields.io/badge/CI-GitHub%20Actions-brightgreen)
![Firmware](https://img.shields.io/badge/Firmware-Bare--Metal%20HAL%20%2F%20FreeRTOS-orange)
![Team](https://img.shields.io/badge/CUERT-Embedded%20%26%20Control-red)

Official firmware repository for the **Actuator Node** of CUERT's (Cairo University Eco-Racing Team) autonomous and drive-by-wire competition vehicles. The system runs on high-performance **STM32G431** microcontrollers to execute real-time motion commands received from the NVIDIA Jetson autonomy computer and verify safe physical execution against live telemetry from the **Sensing Node**.

---

## 1. System Overview: Actuation vs. Sensing

In CUERT's distributed autonomous architecture, responsibility is segregated across two high-speed nodes linked by a deterministic vehicle **CAN Bus**:

* **[EmbeddedSensingNode](https://github.com/CUERT-dev/EmbeddedSensingNode) (Sensing):** Acquires, filters, and broadcasts vehicle state (GNSS speed, 6-axis IMU, wheel-speed hall encoders, motor angle, compass, steering rack potentiometer).
* **[EmbeddedActuatorNode](https://github.com/CUERT-dev/EmbeddedActuatorNode) (Actuation - *This Repo*):** Translates high-level mission directives from the NVIDIA Jetson and safety supervisors into physical motion (DC Throttle, Stepper Steering, BLDC Braking, Shifter) while verifying actual dynamic execution against real-time sensing feedback.

```
                         ┌────────────────────────┐
                         │     NVIDIA Jetson      │
                         │   (Camera, LiDAR, AI)  │
                         └───────────┬────────────┘
                                     │ High-Level Orders
                                     ▼
═════════════════════════════════ CAN BUS ═════════════════════════════════════
              ▲                                                ▲
              │ Real-Time Sensor Telemetry                     │ Command & Closed-Loop
              │ (Speed measure every 100ms)                    │ Verification
              │                                                │
┌─────────────┴──────────────────────────┐  ┌──────────────────┴────────────────────────┐
│       STM32G431 (Sensing Node)         │  │       STM32G431 (Actuating Node)          │
│ • GNSS1A1 GPS Shield                   │  │ • DC Throttle                             │
│ • ASM330LHHHTR Automotive 6-Axis IMU   │  │ • Leadshine EM882S Stepper (Steering)     │
│ • TLE4922 Hall Wheel Speed Encoder     │  │ • EVLDRIVE102BH Sensored BLDC (Braking)   │
│ • AS5600 Magnetic Motor Speed Encoder  │  │   └─ Hall Sensor Feedback to MCU          │
│ • LTS2MDLTR Magnetometer / Compass     │  │ • Gear / Shifter Actuator                 │
│ • Rack Sensor Potentiometer (Steering) │  │ • Indicators & Supportive Dashboard       │
│ • Voltage, Current, Temperature        │  │                                           │
│ • Wi-Fi Telemetry Module               │  │                                           │
└────────────────────────────────────────┘  └───────────────────────────────────────────┘
```

---

## 2. Hardware Subsystems & Actuators

| Subsystem | Actuator / Driver | Interface | Description |
|---|---|---|---|
| **Traction / Throttle** | DC Throttle Motor | PWM / DAC | Regulates propulsion force and vehicle speed. |
| **Steering** | Leadshine EM882S + NEMA Stepper | PULSE / DIR / ENA | High-torque microstepping steering rack positioning with closed-loop potentiometer feedback. |
| **Active Braking** | ST EVLDRIVE102BH + Sensored BLDC | 3-Phase PWM + Hall | Regulated mechanical brake line pressure with hall sensor position & speed feedback. |
| **Transmission** | Gear / Shifter Solenoid Actuator | GPIO Digital Out | Neutral / Drive / Reverse electrical gear selection. |
| **HMI & Safety** | Dashboard & Supportive Indicators | GPIO / UART | Vehicle state LEDs, acoustic alarms, and autonomous status indicators (ASS). |

---

## 3. Repository Layout

```
EmbeddedActuatorNode/
├── .github/
│   └── workflows/
│       └── ci.yml               # Multi-gate automated CI pipeline (Ubuntu 24.04)
├── ActuatorNode/                # STM32G431 firmware project root
│   ├── Core/
│   │   ├── Inc/                 # Application headers (main.h, stm32g4xx_it.h)
│   │   └── Src/                 # Application source (main.c blinky, ISRs, MSP, syscalls)
│   ├── cmake/
│   │   ├── gcc-arm-none-eabi.cmake  # Cross-compilation toolchain configuration
│   │   └── stm32cubemx/         # CubeMX CMake generator integration
│   ├── CMakeLists.txt           # Primary CMake configuration targeting Cortex-M4
│   ├── STM32G431RBTx_FLASH.ld   # Target linker script (128 KB Flash, 32 KB SRAM)
│   └── startup_stm32g431xx.s    # Vector table & Reset_Handler assembly
├── docs/                        # Architecture diagrams, specifications, and shared guides
│   └── CUERT_Embedded_Nodes_Setup_and_Developer_Guide.md # Master cross-repo setup guide
├── test/                        # Host-based automated unit test framework
│   ├── unity/                   # Unity C unit test framework
│   ├── stubs/                   # STM32 HAL mock fixtures & peripherals
│   ├── test_blinky.c            # Commissioning unit test suite
│   ├── test_runner.c            # Test suite entry point
│   └── Makefile                 # Host test build and execution script
├── .gitignore                   # Ignores build outputs, CMake caches, and IDE artifacts
└── README.md                    # This document
```

---

## 4. Prerequisites & Installation

### Linux (EndeavourOS / Ubuntu / Debian)
```bash
# Install host build tools, CMake, and Cppcheck
sudo pacman -S base-devel cmake cppcheck arm-none-eabi-gcc arm-none-eabi-newlib openocd
# Or on Ubuntu / Debian:
sudo apt-get update && sudo apt-get install -y build-essential cmake cppcheck gcc-arm-none-eabi libnewlib-arm-none-eabi openocd
```

### Windows (MSYS2 / Native)
1. Install **Arm GNU Toolchain** (`arm-none-eabi-gcc` 14.2+): Add `bin/` to system `PATH`.
2. Install **CMake** ($\ge$ 3.22) and **Ninja** / **Make**.
3. Install **Cppcheck** ($\ge$ 2.14) for local static analysis.
4. Install **STM32CubeCLT** or **OpenOCD** for flashing via ST-LINK.

---

## 5. Development Workflow & Git Strategy

To maintain strict synchronization with `EmbeddedSensingNode`, both repositories follow an identical automotive branching model:

```
[Feature Branch] ──► [Developing] ──► [Testing] ──► [Validating] ──► [main]
(Local Working)     (Integration)    (CI Gates)   (Acceptance)    (Releases)
```

1. **Branch Roles:**
   * **`Developing`**: Active development and developer feature PRs.
   * **`Testing`**: Staging branch where automated CI gates run.
   * **`Validating`**: Formal acceptance testing, HIL verification, and hardware sign-off.
   * **`main`**: Production releases.
2. **Commit Conventions:**
   * Format commits following Conventional Commits:
     * `feat(actuator): implement EVLDRIVE102BH brake ramp control`
     * `fix(stepper): prevent reverse deadtime underflow`
     * `test(blinky): add toggle frequency verification test`
     * `docs(readme): update pinout mapping table`
3. **Pull Request Protocol:**
   * Push branch and open a PR targeting `Testing` or `Maintaining`.
   * All 3 CI Gates must pass before review:
     * **Gate 1:** Host Unit Tests (Unity).
     * **Gate 2:** Static Analysis (Cppcheck, 0 warnings).
     * **Gate 3:** Clean cross-compilation with Arm GCC 14.2.
   * At least one peer review approval (e.g. from Eyad or Abdelwadoud) required.

---

## 6. Building, Testing & Commissioning

### 1. Run Automated Unit Tests (Host)
Execute the host test suite without hardware attached:
```bash
make -C test test-stm32
```

### 2. Build Target Firmware (Arm GCC)
Cross-compile the STM32G431 firmware binary, ELF, and HEX:
```bash
cd ActuatorNode
cmake -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build -- -j$(nproc)
```
Artifacts will be generated in `ActuatorNode/build/`:
* `ActuatorNode.elf`
* `ActuatorNode.hex`
* `ActuatorNode.bin`

### 3. Flash to NUCLEO-G431RB Board
Connect the Nucleo board via USB and flash using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32g4x.cfg -c "program ActuatorNode/build/ActuatorNode.elf verify reset exit"
```
Or drag and drop `ActuatorNode.bin` directly onto the mounted `NODE_G431RB` mass storage volume.

---

## 7. Initial Commissioning Code (Blinky)

The initial baseline firmware tests board power, clock tree initialization, and toolchain integrity by blinking onboard green user LED **LD2** (`PA5` on NUCLEO-G431RB) at **1 Hz** (500 ms ON, 500 ms OFF).

---

## 8. Related Repositories

* **[EmbeddedSensingNode](https://github.com/CUERT-dev/EmbeddedSensingNode)** — Sibling sensing node (GNSS, IMU, wheel-speed, motor angle, telemetry).
* **[EmbeddedSystemsDocs](https://github.com/CUERT-dev/EmbeddedSystemsDocs)** — Central documentation hub, technical inspection rules, and season roadmap.
