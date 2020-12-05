export PATH=$PATH:/tmp/bin

# The following commands are added after the docker image was built.
cd ~/workspace/imx-yocto-419
source setup-environment build-xwayland

# The following commands are added for yocto SDK.
source /home/pzhang/workspace/cacheData/fsl-imx-sdk/environment-setup-aarch64-poky-linux
export ARCH=arm64
echo CROSS_COMPILE=$CROSS_COMPILE
echo "IMX8 SDK environment has been set."