# EmbeddedActuatorNode

Firmware for the Actuating node of the CUERT car: an STM32G431 that receives motion orders over CAN from the Jetson, verifies actual dynamic states using real-time sensor measurements from the Sensing node, and controls the vehicle actuators. Its sibling repository is the Sensing repo, which shares the same layout, tooling, and workflow.

Target board: NUCLEO-G431RB (STM32G431RBT6). Current state: blinks the on-board LED (LD2, PA5) every 500 ms (1 Hz).

## Actuators owned by this node

| Parameter / Subsystem | Part / Driver | Interface |
|---|---|---|
| Propulsion / Throttle | DC throttle motor | PWM / Analog drive |
| Steering actuation | Leadshine EM882S + NEMA stepper | PULSE / DIR / ENA |
| Active braking | ST EVLDRIVE102BH + Sensored BLDC | 3-phase PWM + Hall feedback |
| Transmission | Gear / Shifter solenoid | GPIO digital output |
| Supportive HMI | Dashboard & status indicators (ASS) | GPIO / UART |

## Installation

Linux (Ubuntu 22.04 or newer, Arch Linux / EndeavourOS) or WSL2:

```bash
# Ubuntu / Debian / WSL2:
sudo apt-get update
sudo apt-get install -y git gcc-arm-none-eabi cmake ninja-build clang-format openocd

# EndeavourOS / Arch Linux:
sudo pacman -S --needed git arm-none-eabi-gcc arm-none-eabi-newlib cmake ninja clang openocd
```

Windows and macOS users can install the same tools: Arm GNU Toolchain 12 or newer, CMake 3.22 or newer, Ninja, and OpenOCD. VS Code with the CMake Tools and Cortex-Debug extensions is the recommended editor.

## Setup

```bash
git clone https://github.com/CUERT-dev/EmbeddedActuatorNode.git
cd EmbeddedActuatorNode
git checkout Developing
cmake --preset release
cmake --build --preset release
```

The build produces `build/actuating.elf`, `build/actuating.bin`, and `build/actuating.map`. Connect the Nucleo over USB and flash with:

```bash
./scripts/flash.sh
```

Success looks like the green LD2 LED blinking at 1 Hz (500 ms interval).

## Branches

| Branch | Content | Receives PRs from |
|---|---|---|
| `Developing` | Active development | `Feature/*`, `Fix/*`, `Docs/*`, `Chore/*` (lowercase prefixes also accepted) |
| `Maintaining` | Code under hardware and integration testing | `Developing` only |
| `Validating` | Verified code ready for hand-over to the team | `Maintaining` only |
| `main` | Released code, the default branch | `Validating` only |

Code moves in one direction: work branch, then `Developing`, then `Maintaining`, then `Validating`, then `main`. The `flow` CI check rejects any pull request that skips a stage, and any push to a branch outside `Developing`, `Maintaining`, and `Validating`.

## Development workflow

1. Take an issue in Linear and move it from Todo to In Progress.
2. Branch from `Developing` using `Feature/<issue-id>-<short-name>` or `Fix/<issue-id>-<short-name>`, for example `Feature/EMB-31-throttle-driver`.
3. Commit with Conventional Commits (`feat:`, `fix:`, `docs:`, `chore:`).
4. Run `clang-format -i` on changed files and build locally before pushing.
5. Open a pull request into `Developing`. CI must be green and one reviewer must approve.
6. When a feature set is ready for hardware testing, open a pull request from `Developing` into `Maintaining`, then from `Maintaining` into `Validating` after it passes, then from `Validating` into `main`. Use merge commits for these promotions so the branches stay in step.
7. Move the Linear issue to Done after the merge into `Developing`, or after `Validating` if the issue requires validation.

Nobody pushes directly to `Developing`, `Maintaining`, or `Validating`. Every change goes through a pull request with a green CI run and approval. `Validating` and `main` additionally require approval from a code owner.

## CI

`.github/workflows/ci.yml` runs on pushes to `Developing`, `Maintaining`, and `Validating`, and on pull requests into those three branches and `main`:

- `flow`: rejects pull requests and pushes that break the branch order.
- `format`: clang-format check on the C sources in `drivers/`, `middleware/`, and `tests/`, excluding the vendor code in `drivers/CMSIS`, `drivers/STM32G4xx_HAL_Driver`, and `drivers/BSP`.
- `build`: CMake and Ninja build with `arm-none-eabi-gcc`, then uploads the ELF, BIN, and map as the `actuating-firmware` artifact.

`.github/workflows/auto-reviewers.yml` requests reviewers automatically on pull requests.

## Repository layout

### Root files

| File | Purpose |
|---|---|
| `CMakeLists.txt` | Top-level CMake build for the firmware |
| `CMakePresets.json` | Configure and build presets, including `release` |
| `EmbeddedActuatorNode.ioc` | STM32CubeMX project: pin map, clocks, and peripheral configuration |
| `.mxproject` | STM32CubeMX project metadata |
| `startup_stm32g431xx.s` | CubeMX-generated startup assembly for the STM32G431 |
| `STM32G431xx_FLASH.ld` | CubeMX-generated linker script for the STM32G431 |
| `.clang-format` | Formatting rules enforced by the `format` check |
| `.editorconfig` | Editor indentation and line-ending defaults |
| `.gitattributes` | Line-ending and diff handling for repository files |
| `.gitignore` | Build outputs and local files kept out of Git |
| `README.md` | This file |

### Folders

| Folder | Content | Purpose |
|---|---|---|
| `Core/` | `Inc/`, `Src/` | STM32CubeMX initialization, interrupt handlers, and the application entry point in `Core/Src/main.c` |
| `drivers/` | `BSP/`, `CMSIS/`, `STM32G4xx_HAL_Driver/`, `.gitkeep` | ST vendor code copied in by STM32CubeMX; actuator drivers are added here one per actuator |
| `middleware/` | `.gitkeep` | Reserved for CAN protocol definitions, RTOS wrappers, safety supervisors, and slew limiters |
| `tests/` | `.gitkeep` | Reserved for host-side unit tests of logic that does not touch hardware |
| `docs/` | Architecture & setup docs | Architecture notes, the CAN message map, wiring tables, and developer guide |
| `scripts/` | `flash.sh`, `setup-repo.sh` | `flash.sh` programs the board, `setup-repo.sh` applies the branch protection rules |
| `cmake/` | `arm-none-eabi.cmake`, `gcc-arm-none-eabi.cmake`, `starm-clang.cmake`, `stm32cubemx/` | Toolchain files for Arm GNU and ST Clang, plus CubeMX-generated CMake sources |
| `EWARM/` | `EmbeddedActuatorNode.ewd`, `.ewp`, `Project.eww`, `startup_stm32g431xx.s`, `stm32g431xx_flash.icf`, `stm32g431xx_sram.icf` | IAR Embedded Workbench project generated by CubeMX |
| `.github/` | `CODEOWNERS`, `pull_request_template.md`, `workflows/ci.yml`, `workflows/auto-reviewers.yml` | Code owners, pull request template, CI and reviewer automation |

Files under `drivers/BSP`, `drivers/CMSIS`, `drivers/STM32G4xx_HAL_Driver`, `cmake/stm32cubemx`, and `EWARM` are produced by STM32CubeMX. Change the `.ioc` and regenerate instead of editing them by hand.

## Working with an AI assistant

Give the assistant this README and the shared setup PDF. Layer rules: the application may call `middleware` and `drivers`; `drivers` reach the hardware through the CubeMX HAL. Code is comment-free and formatted by `.clang-format`; every change must pass `cmake --build --preset release` with zero warnings.
