if [ ! -d ~/workspace/pc_simulator ];then
    mkdir -p ~/workspace
    cd ~/workspace
    git clone --recursive https://gitclone.com/github.com/littlevgl/pc_simulator.git
fi

cd ~/workspace/pc_simulator
cmake CMakeLists.txt
make -j8
./bin/main