#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f ~/.junebox_apt_done ]; then
    # development
	apt-get -y install git build-essential cmake autogen autoconf libtool m4 pkg-config qtchooser bc device-tree-compiler python wget cpio unzip rsync gnupg2
    touch ~/.junebox_apt_done
fi
