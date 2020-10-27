#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.dbus_apt_done ]; then
    # development
    apt update || true
    apt install -y git iproute2 iputils-ping x11-utils dos2unix zip unzip  x11-apps wget cpio dos2unix iputils-ping net-tools iproute2 nano rsync
    apt install -y build-essential
	apt install -y dbus d-feet libgtk2.0-dev libdbus-glib-1-dev
    touch ~/.dbus_apt_done
fi
