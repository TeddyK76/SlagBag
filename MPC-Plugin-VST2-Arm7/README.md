# MPC Live Gen 1 Effects

Two original stereo insert effects targeting the Open MPC VST2 ARMv7 framework:

- **Console Channel** — four native MPC pages: input, dynamics, EQ, output.
- **Scream Drive** — ten nonlinear drive modes, three-band tone and resonant body.

The framework engine contract is 44.1 kHz, interleaved signed 16-bit stereo, up to 128 frames per call. Each effect owns its DSP state per instance.

## Build

Clone [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins), install Docker with ARM/QEMU support and Python 3, then run from either plugin directory:

```sh
MPC_VST=/path/to/mpc-vst-plugins ./build.sh
```

The build script creates the ARMv7 VST2 binary, generated MPC skin, and plugin-list entry under that plugin's `build/` directory. Install using the framework's documented installer workflow.

## Verification

Source and configurations are committed. ARMv7 compilation, MPC installation, audio validation and touchscreen testing are **not yet verified**. The current DSP is an original lightweight first implementation and should be auditioned/tuned on the device before release.
