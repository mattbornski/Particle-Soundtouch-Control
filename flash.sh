#!/bin/bash

# https://docs.particle.io/tutorials/device-os/led/electron/#dfu-mode-device-firmware-upgrade-
# Long story short:
# Hold "mode", tap "reset"
# It should start blinking magenta, then blink yellow.
# When it blinks yellow, release mode, you're ready.
# If it doesn't do those things, hard power cycle and try again.

PREFIX="Saved firmware to: "
PLATFORM="argon"
FIRMWARE="0.8.0-rc.27"

OUTPUT=$(particle compile $PLATFORM --target $FIRMWARE .)
if [ "$?" != "0" ] ; then
  echo "Something went wrong!"
  echo -e "$OUTPUT"
  exit 1
fi
BIN_FILE=$(echo $OUTPUT | grep -oE "$PREFIX(.*\.bin)")
BIN_FILE=${BIN_FILE#$PREFIX}
echo $BIN_FILE

# particle flash is poorly designed and only accepts same-directory bin files
pushd $(dirname $BIN_FILE)
particle flash --usb $(basename $BIN_FILE)
popd

./monitor.sh
