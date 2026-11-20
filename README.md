# EmbeddedActuatorNode

Firmware for the Actuating node of the CUERT car: an STM32G431 that receives motion orders over CAN from the Jetson, verifies actual dynamic states using real-time sensor measurements from the Sensing node, and controls the vehicle actuators. Its sibling repository is the Sensing repo, which shares the same layout, tooling, and workflow.

Target board: NUCLEO-G431RB. Current state: blinks the on-board LED (LD2, PA5) every 500 ms.

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

Windows and macOS users can install the same four tools: Arm GNU Toolchain 12 or newer, CMake 3.22 or newer, Ninja, and OpenOCD. VS Code with the CMake Tools and Cortex-Debug extensions is the recommended editor.

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

Success looks like the green LD2 LED blinking twice per second (1 Hz rate).

## Branches

| Branch | Content | Receives PRs from |
|---|---|---|
| `main` | README only, never receives code | none |
| `Developing` | Active development, blink firmware lives here | `feature/<issue-id>-<name>` and `fix/<issue-id>-<name>` |
| `Testing` | Code under hardware and integration testing | `Developing` only |
| `Validating` | Verified code ready for hand-over to the team | `Testing` only |

Code moves in one direction: feature branch, then `Developing`, then `Testing`, then `Validating`. The `flow` CI check rejects any pull request that skips a stage.

## Roles

| Role | GitHub permission | Can do |
|---|---|---|
| Developer | Write | Open PRs into `Developing`, review teammates |
| Tester | Write | Approve PRs into `Testing`, report results in Linear |
| Lead (code owner) | Maintain | Approve PRs into `Validating`, manage settings |
| Admin | Admin | Repository settings and access |

Nobody pushes directly to `main`, `Developing`, `Testing`, or `Validating`. Every change goes through a pull request with a green CI run and the approvals above.

## Development workflow

1. Take an issue in Linear and move it from Todo to In Progress.
2. Branch from `Developing` using `feature/<issue-id>-<short-name>`, for example `feature/EMB-31-throttle-driver`.
3. Commit with Conventional Commits (`feat:`, `fix:`, `docs:`, `chore:`).
4. Run `clang-format -i` on changed files and build locally before pushing.
5. Open a pull request into `Developing`. CI must be green and one reviewer must approve.
6. When a feature set is ready for hardware testing, open a pull request from `Developing` into `Testing`, then from `Testing` into `Validating` after it passes. Use merge commits for these promotions so the branches stay in step.
7. Move the Linear issue to Done after the merge into `Developing`, or after `Validating` if the issue requires validation.

Repository rules are applied by `scripts/setup-repo.sh`: all four branches are protected, direct and force pushes are blocked, pull requests need approval, the `format`, `build`, and `flow` checks must pass on the three code branches, and conversations must be resolved.

## CI

`.github/workflows/ci.yml` runs on pushes and pull requests to `Developing`, `Testing`, and `Validating`:

- `flow`: rejects pull requests that break the branch order.
- `format`: clang-format check on all C sources.
- `build`: CMake and Ninja build with `arm-none-eabi-gcc`, warnings as errors, then uploads the ELF, BIN, and map as the `actuating-firmware` artifact.

## Folder guide

| Folder | Purpose |
|---|---|
| `app/` | Application entry point and RTOS tasks (actuator control loops, CAN subscriber) |
| `bsp/` | Board support: clocks, GPIO, LED, delays, pin map for the Nucleo and the final PCB |
| `drivers/` | One driver per actuator (DC throttle, Leadshine EM882S stepper, EVLDRIVE102BH BLDC brake, shifter) |
| `middleware/` | CAN message parsing, closed-loop safety supervisors, slew rate limiters |
| `platform/` | Startup code and linker script for STM32G431RB |
| `tests/` | Host-side unit tests for logic that does not touch hardware |
| `docs/` | Architecture notes, CAN message map, wiring tables |
| `scripts/` | `flash.sh` for programming, `setup-repo.sh` for branches and protection rules |
| `cmake/` | Arm GCC toolchain file |
| `.github/` | CI workflow, pull request template, code owners |

## Working with an AI assistant

Give the assistant this README and the shared setup PDF. Layer rules: `app` may call `middleware`, `drivers`, and `bsp`; `drivers` may call `bsp` only; `bsp` touches registers directly. Code is comment-free and formatted by `.clang-format`; every change must pass `cmake --build --preset release` with zero warnings.
