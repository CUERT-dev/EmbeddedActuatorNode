# CUERT Embedded Systems: Unified Developer & AI Assistant Guide
### Cross-Repository Setup, Architecture & Workflow for Sensing and Actuation Nodes
**Cairo University Eco-Racing Team (CUERT) — Embedded & Control Sub-Team**  
**Season 2026 / 2027**  
*Document Version: 1.1.0 | Target Hardware: STM32G431 Microcontrollers (NUCLEO-G431RB)*

---

## 1. Executive System Architecture

CUERT's vehicle architecture divides embedded execution into two real-time nodes communicating over a deterministic 500 kbps automotive **CAN Bus**, interfaced with an onboard **NVIDIA Jetson** computer for perception and autonomous trajectory commands.

```
                          ┌────────────────────────┐
                          │     NVIDIA Jetson      │
                          │   (Camera, LiDAR, AI)  │
                          └───────────┬────────────┘
                                      │ High-Level Orders
                                      ▼
══════════════════════════════════ CAN BUS ══════════════════════════════════════
               ▲                                                ▲
               │ Real-Time Sensor Telemetry                     │ Command & Closed-Loop
               │ (Speed measure every 100ms)                    │ Verification
               │                                                │
┌──────────────┴──────────────────────────┐  ┌──────────────────┴─────────────────────────┐
│        STM32G431 (Sensing Node)         │  │       STM32G431 (Actuating Node)           │
│ • GNSS1A1 GPS Shield                    │  │ • DC Throttle                               │
│ • ASM330LHHHTR Automotive 6-Axis IMU    │  │ • Leadshine EM882S Stepper (Steering)      │
│ • TLE4922 Hall Wheel Speed Encoder      │  │ • EVLDRIVE102BH Sensored BLDC (Braking)     │
│ • AS5600 Magnetic Motor Speed Encoder   │  │   └─ Hall Sensor Feedback to MCU            │
│ • LTS2MDLTR Magnetometer / Compass      │  │ • Gear / Shifter Actuator                   │
│ • Rack Sensor Potentiometer (Steering)  │  │ • Indicators & Supportive Dashboard         │
│ • Voltage, Current, Temperature Sensors │  │                                             │
│ • Wi-Fi Telemetry Module                │  │                                             │
└─────────────────────────────────────────┘  └────────────────────────────────────────────┘
```

### CAN Bus Message Mapping (Base `0x100`)

| CAN ID | DLC | Cycle Rate | Direction | Content / Payload Description |
|---|---|---|---|---|
| `0x100` | 6 B | 50 Hz | Sensing $\to$ Bus | IMU Accelerometer ($a_x, a_y, a_z$ in int16_t) |
| `0x101` | 6 B | 50 Hz | Sensing $\to$ Bus | IMU Gyroscope ($\omega_x, \omega_y, \omega_z$ in int16_t) |
| `0x102` | 8 B | 10 Hz | Sensing $\to$ Bus | GNSS Coordinates (Latitude, Longitude in int32_t) |
| `0x103` | 8 B | 10 Hz | Sensing $\to$ Bus | Speed measure (100 ms periodic), wheel RPM, motor RPM |
| `0x104` | 4 B | 20 Hz | Sensing $\to$ Bus | Steering rack angle & magnetometer heading |
| `0x105` | 8 B | 50 Hz | Jetson $\to$ Actuators | Target demand (Throttle, Steering, Brake, Safety Flags, CRC) |
| `0x106` | 4 B | 50 Hz | Actuators $\to$ Bus | Actuator state feedback (BLDC brake pressure, stepper position) |

---

## 2. Repository Overview & Hardware Roles

### Repository 1: [EmbeddedSensingNode](https://github.com/CUERT-dev/EmbeddedSensingNode)
* **Lead Engineer:** Eyad
* **Primary Responsibility:** State perception, sensor fusion, noise filtering, and telemetry publishing.
* **Key Sensors & Shields:**
  * `GNSS1A1`: Satellite navigation shield streaming position and ground velocity.
  * `ASM330LHHHTR`: Automotive 6-axis IMU (pitch, roll, yaw acceleration/rates).
  * `TLE4922`: High-precision magnetic hall encoder measuring ground wheel speed.
  * `AS5600`: 12-bit contactless magnetic rotary encoder measuring motor speed before gear reduction.
  * `LTS2MDLTR`: Digital 3-axis magnetometer acting as electronic compass.
  * `Rack Sensor`: Precision linear potentiometer measuring instantaneous steering rack travel.
  * `Wi-Fi Module`: Telemetry uplink streaming vehicle data to pitlane dashboard.

### Repository 2: [EmbeddedActuatorNode](https://github.com/CUERT-dev/EmbeddedActuatorNode)
* **Lead Engineer:** Abdelwadoud ("Boda")
* **Primary Responsibility:** Motion execution, motor driver signaling, electronic braking, and closed-loop verification against sensing feedback.
* **Key Actuators & Drivers:**
  * **DC Throttle:** Electronic throttle motor controlled via high-current PWM drive.
  * **Leadshine EM882S Stepper:** High-torque microstepping drive executing rack steering commands.
  * **ST EVLDRIVE102BH + Sensored BLDC:** Precision electronic braking system with onboard hall sensor feedback.
  * **Gear / Shifter:** Solenoid-actuated directional transmission selector (Neutral/Drive/Reverse).
  * **Dashboard & Indicators:** Autonomous status indicators (ASS), acoustic alarms, and run-mode lights.

---

## 3. Required Toolchains & Environment Setup

Both human developers and AI coding assistants must install and verify the following dependencies:

### Toolchain Matrix

| Tool | Minimum Version | Recommended | Purpose |
|---|---|---|---|
| **Arm GNU Toolchain** | `13.2` | `14.2.Rel1` | Cross-compiling STM32 C/ASM code (`arm-none-eabi-gcc`) |
| **CMake** | `3.22` | `3.28+` | Target firmware build orchestration |
| **Host C Compiler** | GCC 11 / Clang 14 | GCC 14 | Running host unit test harnesses |
| **Make / Ninja** | Make 4.3 / Ninja 1.11 | Ninja / Make 4.4 | High-speed build script execution |
| **Clang-Format** | `14.0` | `18.0+` | Automated code styling enforcement (LLVM 4-space) |
| **OpenOCD** | `0.11` | `0.12+` | Hardware debugging and target flashing via ST-LINK |

### Installation Instructions

#### Linux (Arch Linux / EndeavourOS / Ubuntu / Debian)
```bash
# EndeavourOS / Arch Linux:
sudo pacman -S --needed git cmake ninja arm-none-eabi-gcc arm-none-eabi-newlib clang openocd

# Ubuntu 22.04 / 24.04 / Debian:
sudo apt-get update && sudo apt-get install -y \
    git gcc-arm-none-eabi cmake ninja-build clang-format openocd
```

#### Windows (Native / MSYS2 / PowerShell)
1. Install **Arm GNU Toolchain 14.2.Rel1** from Arm Developer and add `bin/` to system `PATH`.
2. Install **CMake** and **Ninja** via `winget install Kitware.CMake Ninja-build.Ninja`.
3. Install **LLVM Clang-Format** and add to `PATH`.
4. Install **OpenOCD** or STM32CubeCLT for target flashing.

#### Verification Command
```bash
arm-none-eabi-gcc --version && cmake --version && ninja --version && clang-format --version
```

---

## 4. Unified Folder Structure

To ensure consistency across the team, both repositories share the same STM32CubeMX + HAL + CMake layout:

```
<NodeRepository>/
├── .clang-format                # LLVM style: 4-space indent, Linux brace placement, 100 col limit
├── .editorconfig                # UTF-8, LF line endings, trailing whitespace trimming
├── .gitattributes               # Auto LF normalisation, binary PDF declaration
├── .github/
│   ├── CODEOWNERS               # Code owners (* @abdelwadoud299 @Eyad-Attwa231)
│   ├── pull_request_template.md # Mandatory PR checklist, Linear link, target gate confirmation
│   └── workflows/
│       ├── auto-reviewers.yml   # Auto-assigns reviewers per target branch
│       └── ci.yml               # 3-Gate automated CI pipeline (Ubuntu 24.04: flow, format, build)
├── .gitignore                   # Ignores build/, *.elf, *.bin, *.map, .vscode/, .idea/
├── .mxproject                   # STM32CubeMX project metadata
├── CMakeLists.txt               # Top-level firmware build definition (sensing / actuating)
├── CMakePresets.json            # Ninja release build preset (output to build/)
├── <NodeRepository>.ioc         # STM32CubeMX project: pin map, clocks, and peripheral config
├── README.md                    # Repository guide, subsystem table, architecture, and workflow
├── STM32G431xx_FLASH.ld         # CubeMX-generated linker script for STM32G431RB
├── startup_stm32g431xx.s        # CubeMX-generated startup assembly
├── Core/
│   ├── Inc/                     # main.h, stm32g4xx_hal_conf.h, stm32g4xx_it.h, nucleo conf
│   └── Src/                     # main.c, hal_msp.c, it.c, syscalls.c, sysmem.c, system_stm32g4xx.c
├── EWARM/                       # IAR Embedded Workbench project generated by CubeMX
├── cmake/
│   ├── arm-none-eabi.cmake      # Arm cross-toolchain wrapper
│   ├── gcc-arm-none-eabi.cmake  # Arm GNU GCC toolchain flags
│   ├── starm-clang.cmake        # ST Arm Clang toolchain option
│   └── stm32cubemx/             # CubeMX-generated CMakeLists.txt for HAL/BSP/Core sources
├── docs/                        # Architecture diagrams, specifications, and setup documentation
├── drivers/
│   ├── BSP/                     # ST Nucleo BSP drivers (vendor-generated)
│   ├── CMSIS/                   # ARM CMSIS headers & DSP/RTOS (vendor-generated)
│   ├── STM32G4xx_HAL_Driver/    # ST HAL/LL drivers (vendor-generated)
│   └── ...                      # Node-specific sensor / actuator drivers (one per part)
├── middleware/                  # Safety supervisors, CAN parsers, sensor fusion, slew limiters
├── scripts/
│   ├── flash.sh                 # OpenOCD target flashing script for NUCLEO-G431RB
│   └── setup-repo.sh            # GitHub branch protection & review enforcement script
└── tests/                       # Host-side unit tests for hardware-independent logic
```

---

## 5. Developer Workflow & Git Branching Policy

All team members and AI assistants must follow this standardized Git workflow:

```
[Feature / Fix / Docs / Chore] ──► [Developing] ──► [Maintaining] ──► [Validating] ──► [main]
      Work Branches                (Integration)   (HW & Integration)  (Acceptance)   (Releases)
```

### 1. Branch Strategy
* `main`: Protected production release branch. Receives PRs from `Validating` only. Direct commits strictly forbidden.
* `Validating`: Final verified firmware ready for team hand-off. Requires code owner review (`@abdelwadoud299` / `@Eyad-Attwa231`).
* `Maintaining`: Hardware-in-the-loop and integration testing branch. Receives PRs from `Developing` only.
* `Developing`: Primary active development branch where work PRs land.
* `Feature/<ticket>-<desc>` / `Fix/<ticket>-<desc>` (uppercase or lowercase prefixes `Feature/*`, `Fix/*`, `Docs/*`, `Chore/*`): Transient developer branches created from `Developing`.

### 2. Conventional Commits
All commit messages must use structured prefixes:
* `feat(<scope>): <short description>` — New feature or driver.
* `fix(<scope>): <short description>` — Bug fix.
* `docs(<scope>): <short description>` — Documentation or README updates.
* `chore(<scope>): <short description>` — Tooling, scripts, or CI updates.
* `refactor(<scope>): <short description>` — Code restructure without logic changes.

### 3. Pull Request Requirements & Auto-Reviewers
Before merging into each target:
1. **Gate 1 (`flow`):** Rejects pushes outside `Developing`, `Maintaining`, and `Validating`, and rejects PRs that skip hierarchy steps.
2. **Gate 2 (`format`):** Runs `clang-format --dry-run --Werror` over project C sources in `drivers/`, `middleware/`, and `tests/`, excluding vendor code (`drivers/CMSIS/**`, `drivers/STM32G4xx_HAL_Driver/**`, `drivers/BSP/**`).
3. **Gate 3 (`build`):** Cross-compiles under Arm GNU Toolchain (`arm-none-eabi-gcc`, `-Wall -Wextra -Werror`) with CMake/Ninja, emitting ELF, BIN, and MAP binaries as workflow artifacts.
4. **Automated Reviewers (`auto-reviewers.yml`):** Automatically requests `@ali44455` & `@2-Farah-1` on `Developing`/`Maintaining`, `@abdelwadoud299` & `@Eyad-Attwa231` on `Validating`, and `@basel-sh` & `@Salah-waheed` on `main`.

---

## 6. Standard Developer & AI Assistant Command Reference

### Task 1: Clean Build Firmware via CMake & Ninja
```bash
cmake --preset release
cmake --build --preset release
```
*Generated Binaries:*
* `EmbeddedActuatorNode`: `build/actuating.elf`, `build/actuating.bin`, `build/actuating.map`
* `EmbeddedSensingNode`: `build/sensing.elf`, `build/sensing.bin`, `build/sensing.map`

### Task 2: Flashing NUCLEO-G431RB Board via OpenOCD
```bash
./scripts/flash.sh
# Equivalent direct command (Actuator example):
openocd -f interface/stlink.cfg -f target/stm32g4x.cfg \
        -c "program build/actuating.elf verify reset exit"
```

### Task 3: Automated Code Formatting (LLVM Standard)
```bash
# Check code formatting without modifying files (CI Gate 2):
git ls-files -z -- \
  'drivers/*.[ch]' 'middleware/*.[ch]' 'tests/*.[ch]' \
  ':!drivers/CMSIS/**' ':!drivers/STM32G4xx_HAL_Driver/**' ':!drivers/BSP/**' |
  xargs -0 -r clang-format --dry-run --Werror

# Auto-format in-place before committing:
git ls-files -z -- \
  'drivers/*.[ch]' 'middleware/*.[ch]' 'tests/*.[ch]' \
  ':!drivers/CMSIS/**' ':!drivers/STM32G4xx_HAL_Driver/**' ':!drivers/BSP/**' |
  xargs -0 -r clang-format -i
```

### Task 4: Enforcing GitHub Branch Protection Rules
```bash
bash scripts/setup-repo.sh CUERT-dev/EmbeddedActuatorNode
```

---

## 7. Initial Commissioning Baseline (Nucleo Blinky)

As the baseline acceptance deliverable, both nodes initialize with clean STM32CubeMX + HAL commissioning firmware (`BSP_LED_Init`, `BSP_PB_Init`, `LPUART1` COM1 at 115200 baud) toggling onboard user LED **LD2**:

| Repository | Hardware Pin | Blink Specification & Rate |
|---|---|---|
| **EmbeddedActuatorNode** | `PA5` (LD2, Active High) | 1 Hz (`HAL_Delay(500)`) via CubeMX HAL + Nucleo BSP |
| **EmbeddedSensingNode** | `PA5` (LD2, Active High) | `HAL_Delay(1100)` (`main`/`Validating`) / `1000` (`Developing`) via CubeMX HAL + Nucleo BSP |

---

*Prepared by Abdelwadoud & Eyad — CUERT Embedded & Control Team.*  
*Cairo University Eco-Racing Team (CUERT) — Formula Student Autonomous.*
