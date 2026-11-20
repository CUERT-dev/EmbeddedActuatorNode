# CUERT Embedded Systems: Unified Developer & AI Assistant Guide
### Cross-Repository Setup, Architecture & Workflow for Sensing and Actuation Nodes
**Cairo University Eco-Racing Team (CUERT) — Embedded & Control Sub-Team**  
**Season 2026 / 2027**  
*Document Version: 1.0.0 | Target Hardware: STM32G431 Microcontrollers*

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

| CAN ID | DLC | Cycle Rate | Direction | Content / Payload |
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
| **Host C Compiler** | GCC 11 / Clang 14 | GCC 14 | Running host unit test harnesses (Unity) |
| **Make / Ninja** | Make 4.3 / Ninja 1.11 | Make 4.4 | Build script execution |
| **Cppcheck** | `2.12` | `2.14+` | Automated static code analysis & safety audits |
| **OpenOCD** | `0.11` | `0.12+` | Hardware debugging and target flashing via ST-LINK |

### Installation Instructions

#### Linux (Arch Linux / EndeavourOS / Ubuntu / Debian)
```bash
# EndeavourOS / Arch Linux:
sudo pacman -S --needed base-devel cmake cppcheck arm-none-eabi-gcc arm-none-eabi-newlib openocd

# Ubuntu 22.04 / 24.04:
sudo apt-get update && sudo apt-get install -y \
    build-essential cmake cppcheck gcc-arm-none-eabi libnewlib-arm-none-eabi openocd
```

#### Windows (Native / MSYS2 / PowerShell)
1. Install **Arm GNU Toolchain 14.2.Rel1** from Arm Developer and add `bin/` to system `PATH`.
2. Install **CMake** and **Make** via `winget install Kitware.CMake GnuWin32.Make` or MSYS2.
3. Install **Cppcheck** from cppcheck.sourceforge.io and add to `PATH`.
4. Install **STM32CubeCLT** (Command-Line Tools) for official ST-LINK GDB server and programmer CLI.

#### Verification Command
Run this check to confirm all tools are correctly registered:
```bash
arm-none-eabi-gcc --version && cmake --version && cppcheck --version && make --version
```

---

## 4. Unified Folder Structure

To ensure consistency across the team, both repositories strictly follow the same automotive hierarchy:

```
<NodeRepository>/
├── .github/
│   └── workflows/
│       └── ci.yml               # 3-Gate automated CI pipeline (Ubuntu 24.04)
├── <NodeName>/                  # Firmware root (ActuatorNode or SensingNode)
│   ├── Core/
│   │   ├── Inc/                 # Application headers (main.h, stm32g4xx_it.h)
│   │   └── Src/                 # Application source (main.c, ISRs, MSP, syscalls)
│   ├── cmake/
│   │   ├── gcc-arm-none-eabi.cmake  # Architecture flags (-mcpu=cortex-m4 -mfpu=fpv4-sp-d16)
│   │   └── stm32cubemx/         # CubeMX generator wrapper
│   ├── CMakeLists.txt           # Firmware build specification
│   ├── STM32G431RBTx_FLASH.ld   # Memory layout (128K Flash, 32K RAM, 10K CCM)
│   └── startup_stm32g431xx.s    # Reset_Handler and vector table
├── docs/                        # Architecture diagrams and specifications
├── test/                        # Host-based automated unit test harness
│   ├── unity/                   # Unity test framework
│   ├── stubs/                   # Hardware abstraction mocks (GPIO, Timers, CAN)
│   ├── test_<module>.c          # Test suites
│   ├── test_runner.c            # Test runner entry point
│   └── Makefile                 # Host test execution script
├── .gitignore                   # Ignore build artifacts, .hex, .elf, and IDE files
└── README.md                    # Primary repository onboarding guide
```

---

## 5. Developer Workflow & Git Branching Policy

All team members and AI assistants must follow this standardized Git workflow:

```
[Feature Branch]  ──(PR + 3 Gates Pass)──►  [Testing]  ──(Merge)──►  [Maintaining]  ──►  [main]
Feature/<Ticket>                             (CI Checks)           (Accepted Core)       (Releases)
```

### 1. Branch Strategy
* `main`: Protected production branch. Only tagged releases.
* `Maintaining`: Protected primary development branch. Stable, reviewed code.
* `Testing`: Pre-release integration branch where automated CI gates run.
* `Feature/<Name>`: Transient development branch (e.g., `Feature/BlinkyNucleo`, `Feature/BldcBrakeDriver`).

### 2. Conventional Commits
All commit messages must use structured prefixes:
* `feat(<scope>): <short description>` — New feature or driver.
* `fix(<scope>): <short description>` — Bug fix.
* `test(<scope>): <short description>` — Unit tests or mock fixtures.
* `docs(<scope>): <short description>` — Documentation or README updates.
* `refactor(<scope>): <short description>` — Code restructure without logic changes.

### 3. Pull Request Requirements
Before merging into `Testing` or `Maintaining`:
1. **Gate 1:** Host Unit Tests (Unity) must pass with 0 failures (`make -C test test-stm32`).
2. **Gate 2:** Cppcheck static analysis must return exit code 0 (`--error-exitcode=1`).
3. **Gate 3:** Cross-compilation under Arm GCC 14.2 must succeed without warnings.
4. Peer review approval by the sibling node lead (Abdelwadoud $\leftrightarrow$ Eyad).

---

## 6. Standard Developer & AI Assistant Command Reference

### Task 1: Run Automated Host Unit Tests
```bash
make -C test test-stm32
```
*Expected Output:*
```
=======================================================
 Unity Automated Test Suite: test_runner.c
=======================================================
  PASS: test_Blinky_SingleStep_IncrementsCounterAndTogglesPin (line 21)
  PASS: test_Blinky_MultipleCycles_AlternatesPinState (line 22)
  PASS: test_Blinky_TimingSimulation_1HzRate (line 23)
-------------------------------------------------------
 Tests Executed: 3 | Failures: 0 | Ignored: 0
 [PASS] All unit tests completed successfully!
=======================================================
```

### Task 2: Run Cppcheck Static Analysis
```bash
cppcheck --enable=warning,performance,portability \
         --error-exitcode=1 \
         --inline-suppr \
         -I ActuatorNode/Core/Inc \
         -I test/stubs \
         ActuatorNode/Core/Src
```

### Task 3: Cross-Compile Target Firmware
```bash
cd ActuatorNode
cmake -B build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build -- -j$(nproc)
```
*Generated Binaries:*
* `build/ActuatorNode.elf`
* `build/ActuatorNode.hex`
* `build/ActuatorNode.bin`

### Task 4: Flash Nucleo-G431RB Board
```bash
openocd -f interface/stlink.cfg -f target/stm32g4x.cfg \
        -c "program build/ActuatorNode.elf verify reset exit"
```

---

## 7. Initial Commissioning Baseline (Nucleo Blinky)

As the baseline acceptance deliverable, both nodes initialize with clean hardware commissioning firmware blinking onboard user LED **LD2**:

* **Actuator Node:** LD2 connected to **`PA5`** (Green LED, active HIGH).
* **Sensing Node:** LD2 connected to **`PA5`** (or PC13 on legacy bluepills).
* **Blink Rate:** **1 Hz** (500 ms ON, 500 ms OFF) driven by HAL SysTick delay.

---

## 8. PDF Export Instructions

To generate the final submission PDF from this Markdown document, use any of the following standard methods:

### Option A: Using Pandoc with Weasyprint or XeLaTeX
```bash
pandoc docs/CUERT_Embedded_Nodes_Setup_and_Developer_Guide.md \
       -o CUERT_Embedded_Nodes_Setup_and_Developer_Guide.pdf \
       --pdf-engine=xelatex \
       -V geometry:margin=1in
```

### Option B: Using VS Code Markdown PDF Extension
1. Open this file in VS Code.
2. Press `Ctrl + Shift + P` $\to$ **Markdown PDF: Export (pdf)**.
3. The PDF will be saved in the `docs/` folder ready for upload.

### Option C: Chrome / Chromium Headless CLI
```bash
google-chrome --headless --disable-gpu --print-to-pdf=CUERT_Embedded_Nodes_Setup_and_Developer_Guide.pdf \
              docs/CUERT_Embedded_Nodes_Setup_and_Developer_Guide.md
```

---
*Prepared by Abdelwadoud & Eyad — CUERT Embedded & Control Team.*
