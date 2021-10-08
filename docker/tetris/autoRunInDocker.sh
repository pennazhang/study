#export P4USER=PZhang
#export P4PORT=10.90.66.25:3501
#export DEPOT_TOOLS_UPDATE=0
export PATH=$PATH:/usr/share/maven/bin:/usr/share/ant/bin

#export dbg='-D'
#export cache='-S http://provideo-build.ad.harman.com/files/mirror/jarvis/ish/sstate'

#cd ~/workspace/AMX/js-gclient/elina-distro
#. bld.imx8mmddr3lval-cdl.log

# The following commands are added after the docker image was built.
#cd ~/workspace/imx-yocto-bsp
#source setup-environment build-xwayland

# The following commands are added for yocto SDK.
#source /home/pzhang/workspace/cacheData/elina_1.16.16/environment-setup-aarch64-elina-linux
#export ARCH=arm64
#echo CROSS_COMPILE=$CROSS_COMPILE
#export ConfigManager_DIR="/home/pzhang/workspace/cacheData/elina_1.16.16/sysroots/aarch64-elina-linux/usr/lib/cmake/ConfigManager"
#export JarvisCivet_DIR="/home/pzhang/workspace/cacheData/elina_1.16.16/sysroots/aarch64-elina-linux/usr/lib/cmake"

git config --global user.email "percy.zhang@harman.com"
git config --global user.name "pzhang"
git config --global --add review.http://uss.amx.internal:8080/.username "pzhang"
git config --global color.ui "auto"

sudo sysctl -w net.ipv4.tcp_window_scaling=0

echo ""
echo "---------------------------------------------------------------------"
echo "To install Android SDK build-tools, run the following command: cd /opt/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86/tools && ./android."
echo ""
echo "---------------------------------------------------------------------"
echo "To build Modero, run the following command: cd ~/tetris/Modero-X && ant -f moderox.xml -DBASE=/home/pzhang/tetris -DJOBS=-j8 -DREPO_JOBS=-j8 -DmajorVersion=1 -DminorVersion=8 -DMAVEN3_PATH=/opt/apache-maven-3.1.1 -DJAVA_7_HOME=/usr/lib/jvm/java-7-openjdk-amd64 -DANDROID_SDK_PATH=/opt/android-sdk_LB_ModeroE-G5_Tetris_1_8_1_linux-x86 all-eng sign-eng"
echo ""
