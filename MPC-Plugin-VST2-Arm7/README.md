# MPC Live Gen 1 VST2 Effects

Clean-room native stereo effects for Akai MPC Live Gen 1 / MPC OS 3.x.

## Plugins

- **Console Channel** — four-page channel strip: input, dynamics, EQ, output.
- **Scream Drive** — multi-mode distortion with tone/body shaping.

## Target and host contract

Built against [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins), targeting ARMv7 (Gen 1 / Force). The framework's VST2 engine contract is 44.1 kHz, interleaved signed 16-bit stereo, 128-frame blocks. Effects implement `mpc_engine_t.process`; each instance owns its DSP state.

## Status

Initial project scaffold. No binary is claimed to be built or hardware-tested until the ARMv7 build and MPC installation are verified.

## Build prerequisites

- Clone this repository and the Open MPC VST framework.
- Docker with ARM emulation (QEMU/binfmt) and Python 3.
- Run each plugin's `build.sh` against the framework's `tools/build_port.sh`.

See each plugin directory for controls, parameter index lock, build configuration and implementation notes.
