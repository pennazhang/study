export P4USER=PZhang
export P4PORT=10.90.66.25:3501
export DEPOT_TOOLS_UPDATE=0
export PATH=$PATH:${HOME}/workspace/gclient/depot-tools

#export dbg='-D'
#export cache='-S http://provideo-build.ad.harman.com/files/mirror/jarvis/ish/sstate'

#cd ~/workspace/AMX/js-gclient/elina-distro
#. bld.imx8mmddr3lval-cdl.log

# The following commands are added after the docker image was built.
#cd ~/workspace/imx-yocto-bsp
#source setup-environment build-xwayland

# The following commands are added for yocto SDK.
source /home/pzhang/workspace/cacheData/elina_sdk/environment-setup-aarch64-elina-linux
export ARCH=arm64
echo CROSS_COMPILE=$CROSS_COMPILE
export ConfigManager_DIR="/home/pzhang/workspace/cacheData/elina_1.16.16/sysroots/aarch64-elina-linux/usr/lib/cmake/ConfigManager"
export JarvisCivet_DIR="/home/pzhang/workspace/cacheData/elina_1.16.16/sysroots/aarch64-elina-linux/usr/lib/cmake"
echo "Javis jetstream SDK environment has been set."
