cd ~/workspace/study/qt/demoGui/project
rm -rf out Makefile || true
qmake
make
./out/release/bin/demoGui
