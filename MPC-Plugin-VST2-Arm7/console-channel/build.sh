#!/bin/sh
set -eu
: "${MPC_VST:?Set MPC_VST to the cloned open-mpc/mpc-vst-plugins directory}"
here=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
"$MPC_VST/tools/build_port.sh" "$here/vst.json"
