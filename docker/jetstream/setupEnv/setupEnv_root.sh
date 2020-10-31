#!/bin/bash

# Run this script with sudo (called from install_dev)

set -e
MYDIR=`dirname "$0"`; MYDIR=`realpath "$MYDIR"`; export MYDIR

cd "$MYDIR"

# install dependencies
if [ ! -f /tmp/.jetstream_apt_done ]; then

    #install for jetstream
    apt install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat libsdl1.2-dev xterm rpm2cpio lzop liblz4-tool libssl-dev g++-multilib lib32z1-dev pv
    sudo pip install jira
    sudo apt-get install texlive-latex-base
fi
