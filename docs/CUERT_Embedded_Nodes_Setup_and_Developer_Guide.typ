#set document(
  title: "CUERT Embedded Systems: Unified Developer & AI Assistant Guide",
  author: "Abdelwadoud & Eyad (CUERT Embedded & Control Team)"
)

#set page(
  paper: "a4",
  margin: (x: 1.8cm, top: 2.0cm, bottom: 2.0cm),
  header: context {
    if counter(page).get().first() > 1 [
      #grid(
        columns: (1fr, auto),
        align: (left + horizon, right + horizon),
        text(size: 8pt, fill: rgb("#475569"), weight: "bold")[CUERT Embedded & Control Sub-Team | STM32G431 Architecture],
        text(size: 8pt, fill: rgb("#64748b"))[Season 2026 / 2027]
      )
      #v(1pt)
      #line(length: 100%, stroke: 0.5pt + rgb("#cbd5e1"))
    ]
  },
  footer: context {
    line(length: 100%, stroke: 0.5pt + rgb("#cbd5e1"))
    v(2pt)
    grid(
      columns: (1fr, auto),
      align: (left + horizon, right + horizon),
      text(size: 8pt, fill: rgb("#64748b"))[CUERT Embedded Nodes Setup & Developer Guide (Rev 1.1.0)],
      text(size: 8pt, fill: rgb("#475569"), weight: "bold")[Page #counter(page).display("1 of 1", both: true)]
    )
  }
)

#set text(
  font: ("Liberation Sans", "DejaVu Sans", "Arial"),
  size: 9.5pt,
  fill: rgb("#1e293b"),
  spacing: 115%
)

#set par(justify: true, leading: 0.58em)

#show heading.where(level: 1): it => block(below: 8pt, above: 12pt)[
  #text(fill: rgb("#0f172a"), weight: "bold", size: 12.5pt)[#it.body]
  #v(1.5pt)
  #line(length: 100%, stroke: 1.2pt + rgb("#1e40af"))
]

#show heading.where(level: 2): it => block(below: 6pt, above: 10pt)[
  #text(fill: rgb("#1e40af"), weight: "bold", size: 11pt)[#it.body]
]

#show heading.where(level: 3): it => block(below: 5pt, above: 8pt)[
  #text(fill: rgb("#334155"), weight: "bold", size: 10pt)[#it.body]
]

#show raw.where(block: true): it => block(
  fill: rgb("#f8fafc"),
  stroke: 0.5pt + rgb("#cbd5e1"),
  inset: 6pt,
  radius: 3.5pt,
  width: 100%,
  breakable: false,
  text(font: ("Liberation Mono", "DejaVu Sans Mono"), size: 7.8pt, fill: rgb("#0f172a"))[#it]
)

#show raw.where(block: false): it => text(
  font: ("Liberation Mono", "DejaVu Sans Mono"),
  size: 8.5pt,
  fill: rgb("#0369a1"),
  weight: "medium"
)[#it]

// Title Banner
#align(center)[
  #block(
    fill: rgb("#f0f7ff"),
    stroke: 1pt + rgb("#93c5fd"),
    inset: (x: 12pt, y: 9pt),
    radius: 5pt,
    width: 100%
  )[
    #text(fill: rgb("#1e3a8a"), size: 14pt, weight: "bold")[CUERT Embedded Systems: Unified Developer & AI Assistant Guide]
    
    #v(2pt)
    #text(fill: rgb("#1d4ed8"), size: 10pt, weight: "medium")[Cross-Repository Setup, Architecture & Workflow for Sensing and Actuation Nodes]
    
    #v(3pt)
    #text(fill: rgb("#475569"), size: 8pt)[
      *Cairo University Eco-Racing Team (CUERT) — Embedded & Control Sub-Team* \
      *Season 2026 / 2027* | Document Version: 1.1.0 | Target Hardware: STM32G431 Microcontrollers (NUCLEO-G431RB)
    ]
  ]
]

#v(2pt)

= 1. Executive System Architecture

CUERT's vehicle architecture divides embedded execution into two real-time nodes communicating over a deterministic 500 kbps automotive *CAN Bus*, interfaced with an onboard *NVIDIA Jetson* computer for perception and autonomous trajectory commands.

#v(1pt)

#block(
  fill: rgb("#f8fafc"),
  inset: (x: 6pt, y: 5pt),
  radius: 4pt,
  stroke: 0.5pt + rgb("#cbd5e1"),
  width: 100%,
  breakable: false
)[
  #text(font: ("Liberation Mono", "DejaVu Sans Mono"), size: 6.4pt, fill: rgb("#0f172a"))[
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
  ]
]

#v(1pt)

=== CAN Bus Message Mapping (Base `0x100`)

#table(
  columns: (1fr, 0.7fr, 1fr, 1.6fr, 3.7fr),
  fill: (col, row) => if row == 0 { rgb("#1e3a8a") } else if calc.odd(row) { rgb("#f8fafc") } else { none },
  stroke: 0.5pt + rgb("#cbd5e1"),
  align: (col, row) => if row == 0 { center + horizon } else { left + horizon },
  inset: 4pt,
  table.header(
    text(fill: white, weight: "bold", size: 7.5pt)[CAN ID],
    text(fill: white, weight: "bold", size: 7.5pt)[DLC],
    text(fill: white, weight: "bold", size: 7.5pt)[Cycle Rate],
    text(fill: white, weight: "bold", size: 7.5pt)[Direction],
    text(fill: white, weight: "bold", size: 7.5pt)[Content / Payload Description]
  ),
  [`0x100`], [6 B], [50 Hz], [Sensing $->$ Bus], [IMU Accelerometer ($a_x, a_y, a_z$ in int16_t)],
  [`0x101`], [6 B], [50 Hz], [Sensing $->$ Bus], [IMU Gyroscope ($omega_x, omega_y, omega_z$ in int16_t)],
  [`0x102`], [8 B], [10 Hz], [Sensing $->$ Bus], [GNSS Coordinates (Latitude, Longitude in int32_t)],
  [`0x103`], [8 B], [10 Hz], [Sensing $->$ Bus], [Speed measure (100 ms periodic), wheel RPM, motor RPM],
  [`0x104`], [4 B], [20 Hz], [Sensing $->$ Bus], [Steering rack angle & magnetometer heading],
  [`0x105`], [8 B], [50 Hz], [Jetson $->$ Actuators], [Target demand (Throttle, Steering, Brake, Safety Flags, CRC)],
  [`0x106`], [4 B], [50 Hz], [Actuators $->$ Bus], [Actuator state feedback (BLDC brake pressure, stepper position)]
)

#pagebreak()

= 2. Repository Overview & Hardware Roles

#grid(
  columns: (1fr, 1fr),
  gutter: 12pt,
  [
    #block(
      fill: rgb("#f8fafc"),
      stroke: 0.5pt + rgb("#cbd5e1"),
      inset: 8pt,
      radius: 4pt,
      width: 100%
    )[
      #text(fill: rgb("#1e40af"), weight: "bold", size: 10pt)[Repository 1: EmbeddedSensingNode] \
      #v(1pt)
      *URL:* #link("https://github.com/CUERT-dev/EmbeddedSensingNode")[CUERT-dev/EmbeddedSensingNode] \
      *Lead Engineer:* Eyad \
      *Primary Role:* Perception, telemetry, sensor fusion

      #v(2pt)
      *Key Sensors & Shields:*
      - *GNSS1A1:* Satellite navigation & ground velocity
      - *ASM330LHHHTR:* Automotive 6-axis IMU (pitch, roll, yaw)
      - *TLE4922:* High-precision wheel speed hall encoder
      - *AS5600:* 12-bit contactless rotary motor speed encoder
      - *LTS2MDLTR:* Digital 3-axis compass magnetometer
      - *Rack Sensor:* Precision steering rack linear pot
      - *Wi-Fi Module:* High-speed pitlane telemetry uplink
    ]
  ],
  [
    #block(
      fill: rgb("#f8fafc"),
      stroke: 0.5pt + rgb("#cbd5e1"),
      inset: 8pt,
      radius: 4pt,
      width: 100%
    )[
      #text(fill: rgb("#1e40af"), weight: "bold", size: 10pt)[Repository 2: EmbeddedActuatorNode] \
      #v(1pt)
      *URL:* #link("https://github.com/CUERT-dev/EmbeddedActuatorNode")[CUERT-dev/EmbeddedActuatorNode] \
      *Lead Engineer:* Abdelwadoud ("Boda") \
      *Primary Role:* Motion execution & closed-loop verification

      #v(2pt)
      *Key Actuators & Drivers:*
      - *DC Throttle:* High-current PWM propulsion drive
      - *Leadshine EM882S Stepper:* Microstepping steering
      - *ST EVLDRIVE102BH + BLDC:* Active electronic braking
        #text(size: 8pt, fill: rgb("#475569"))[(with onboard Hall speed/position feedback to MCU)]
      - *Gear / Shifter:* Solenoid transmission control
      - *HMI Indicators:* Autonomous status indicators (ASS)
    ]
  ]
)

#v(2pt)

= 3. Required Toolchains & Environment Setup

Both human developers and AI coding assistants must install and verify the following dependencies:

#table(
  columns: (2fr, 1.2fr, 1.2fr, 3.4fr),
  fill: (col, row) => if row == 0 { rgb("#1e3a8a") } else if calc.odd(row) { rgb("#f8fafc") } else { none },
  stroke: 0.5pt + rgb("#cbd5e1"),
  align: (col, row) => if row == 0 { center + horizon } else { left + horizon },
  inset: 4.5pt,
  table.header(
    text(fill: white, weight: "bold", size: 7.5pt)[Tool],
    text(fill: white, weight: "bold", size: 7.5pt)[Minimum],
    text(fill: white, weight: "bold", size: 7.5pt)[Recommended],
    text(fill: white, weight: "bold", size: 7.5pt)[Purpose]
  ),
  [*Arm GNU Toolchain*], [`13.2`], [`14.2.Rel1`], [Cross-compiling STM32 C/ASM code (`arm-none-eabi-gcc`)],
  [*CMake*], [`3.22`], [`3.28+`], [Target firmware build orchestration],
  [*Host C Compiler*], [GCC 11 / Clang 14], [GCC 14], [Running host unit test harnesses],
  [*Make / Ninja*], [Make 4.3 / Ninja 1.11], [Ninja / Make 4.4], [High-speed build script execution],
  [*Clang-Format*], [`14.0`], [`18.0+`], [Automated code styling enforcement (LLVM 4-space)],
  [*OpenOCD*], [`0.11`], [`0.12+`], [Hardware debugging & target flashing via ST-LINK]
)

=== Installation Instructions

#grid(
  columns: (1fr, 1fr),
  gutter: 10pt,
  [
    *EndeavourOS / Arch Linux:*
    ```bash
    sudo pacman -S --needed git cmake ninja \
      arm-none-eabi-gcc arm-none-eabi-newlib \
      clang openocd
    ```
  ],
  [
    *Ubuntu 22.04 / 24.04 / Debian:*
    ```bash
    sudo apt-get update && sudo apt-get install -y \
      git gcc-arm-none-eabi cmake ninja-build \
      clang-format openocd
    ```
  ]
)

=== Toolchain Verification
```bash
arm-none-eabi-gcc --version && cmake --version && ninja --version && clang-format --version
```

#pagebreak()

= 4. Unified Folder Structure

Both repositories share the same STM32CubeMX + HAL + CMake directory layout:

#block(
  fill: rgb("#f8fafc"),
  stroke: 0.5pt + rgb("#cbd5e1"),
  inset: 7pt,
  radius: 4pt,
  width: 100%,
  breakable: false
)[
  #text(font: ("Liberation Mono", "DejaVu Sans Mono"), size: 7.0pt, fill: rgb("#0f172a"))[
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
├── drivers/                     # Vendor BSP/CMSIS/HAL + node-specific sensor/actuator drivers
├── middleware/                  # Safety supervisors, CAN parsers, sensor fusion, slew limiters
├── scripts/
│   ├── flash.sh                 # OpenOCD target flashing script for NUCLEO-G431RB
│   └── setup-repo.sh            # GitHub branch protection & review enforcement script
└── tests/                       # Host-side unit tests for hardware-independent logic
```
  ]
]

= 5. Developer Workflow & Git Branching Policy

#align(center)[
  #block(fill: rgb("#f1f5f9"), inset: (x: 10pt, y: 5pt), radius: 4pt, stroke: 0.5pt + rgb("#94a3b8"))[
    `[Feature / Fix / Docs / Chore] ──► [Developing] ──► [Maintaining] ──► [Validating] ──► [main]` \
    `      Work Branches                (Integration)   (HW & Integration)  (Acceptance)   (Releases)`
  ]
]

#v(1pt)

*Branch Hierarchy & Access Control:*
- *`main`:* Protected release branch receiving PRs from `Validating` only. Direct commits strictly forbidden.
- *`Validating`:* Final vehicle validation and team hand-off. Requires code owner review (`@abdelwadoud299` / `@Eyad-Attwa231`).
- *`Maintaining`:* Hardware-in-the-loop and integration testing branch. Receives PRs from `Developing` only.
- *`Developing`:* Active engineering branch where work branches (`Feature/*`, `Fix/*`, `Docs/*`, `Chore/*`, upper or lower) land via approved PRs.

*Automated Multi-Gate CI (`ci.yml` & `auto-reviewers.yml`):*
1. *Gate 1 (`flow`):* Rejects direct pushes outside `Developing`, `Maintaining`, `Validating`, and rejects PRs that skip hierarchy steps.
2. *Gate 2 (`format`):* Runs `clang-format --dry-run --Werror` on `drivers/`, `middleware/`, and `tests/`, excluding vendor `CMSIS/`, `STM32G4xx_HAL_Driver/`, and `BSP/`.
3. *Gate 3 (`build`):* Invokes Ninja release preset with `arm-none-eabi-gcc` (`-Wall -Wextra -Werror`), emitting ELF, BIN, and MAP binaries as artifacts.
4. *Auto-Reviewers:* Requests `@ali44455` & `@2-Farah-1` on `Developing`/`Maintaining`, `@abdelwadoud299` & `@Eyad-Attwa231` on `Validating`, and `@basel-sh` & `@Salah-waheed` on `main`.

#pagebreak()

= 6. Standard Developer Command Reference

=== Task 1: Clean Build Firmware via CMake & Ninja
```bash
cmake --preset release
cmake --build --preset release
```
*Generated Binaries:*
- `EmbeddedActuatorNode`: `build/actuating.elf`, `build/actuating.bin`, `build/actuating.map`
- `EmbeddedSensingNode`: `build/sensing.elf`, `build/sensing.bin`, `build/sensing.map`

=== Task 2: Flashing NUCLEO-G431RB via OpenOCD
```bash
./scripts/flash.sh
# Equivalent OpenOCD direct command (Actuator example):
openocd -f interface/stlink.cfg -f target/stm32g4x.cfg \
        -c "program build/actuating.elf verify reset exit"
```

=== Task 3: Automated Code Formatting (LLVM Standard)
```bash
# Verify formatting without modifying files (CI Gate 2 check):
git ls-files -z -- 'drivers/*.[ch]' 'middleware/*.[ch]' 'tests/*.[ch]' \
  ':!drivers/CMSIS/**' ':!drivers/STM32G4xx_HAL_Driver/**' ':!drivers/BSP/**' |
  xargs -0 -r clang-format --dry-run --Werror

# Auto-format project sources in-place before committing:
git ls-files -z -- 'drivers/*.[ch]' 'middleware/*.[ch]' 'tests/*.[ch]' \
  ':!drivers/CMSIS/**' ':!drivers/STM32G4xx_HAL_Driver/**' ':!drivers/BSP/**' |
  xargs -0 -r clang-format -i
```

=== Task 4: Enforcing GitHub Branch Protection Rules
```bash
bash scripts/setup-repo.sh CUERT-dev/EmbeddedActuatorNode
```

= 7. Initial Commissioning Baseline (Nucleo Blinky)

As the baseline acceptance deliverable, both nodes initialize with clean STM32CubeMX + HAL commissioning firmware (`BSP_LED_Init`, `BSP_PB_Init`, `LPUART1` COM1 at 115200 baud) toggling onboard user LED *LD2*:

#table(
  columns: (2.2fr, 1.8fr, 4fr),
  fill: (col, row) => if row == 0 { rgb("#1e3a8a") } else if calc.odd(row) { rgb("#f8fafc") } else { none },
  stroke: 0.5pt + rgb("#cbd5e1"),
  align: (col, row) => if row == 0 { center + horizon } else { left + horizon },
  inset: 4.5pt,
  table.header(
    text(fill: white, weight: "bold", size: 7.5pt)[Repository],
    text(fill: white, weight: "bold", size: 7.5pt)[Hardware Pin],
    text(fill: white, weight: "bold", size: 7.5pt)[Blink Specification & Rate]
  ),
  [*EmbeddedActuatorNode*], [`PA5` (LD2, Active High)], [1 Hz (`HAL_Delay(500)`) via CubeMX HAL + Nucleo BSP],
  [*EmbeddedSensingNode*], [`PA5` (LD2, Active High)], [`HAL_Delay(1100)` (`main`/`Validating`) / `1000` (`Developing`) via CubeMX HAL + Nucleo BSP]
)

#v(14pt)
#line(length: 100%, stroke: 0.8pt + rgb("#cbd5e1"))
#v(3pt)
#align(center)[
  #text(size: 8.5pt, fill: rgb("#334155"), weight: "bold")[
    Prepared by Abdelwadoud & Eyad — CUERT Embedded & Control Team \
  ]
  #v(1.5pt)
  #text(size: 7.5pt, fill: rgb("#64748b"))[
    Cairo University Eco-Racing Team (CUERT) — Formula Student Autonomous \
    Cairo University Faculty of Engineering (CUFE)
  ]
]
