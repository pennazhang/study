#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f /tmp/.qt5.6.1_apt_done ]; then
    apt update || true

    apt install -y qtchooser
    touch /tmp/.qt5.6.1_apt_done
fi
