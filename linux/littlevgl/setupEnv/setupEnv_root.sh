#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.littlevgl_apt_done ]; then
    # development
    apt update
    apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip  x11-apps
    apt install -y build-essential libsdl2-dev
    ln -s /git/study/bin/runCommand.sh /bin/runCommand.sh
    touch ~/.littlevgl_apt_done
fi
