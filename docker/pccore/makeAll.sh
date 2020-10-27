#! /bin/bash

export DBUILD_NUMBER=5

export GIT_DIR=/git/pccore
export WORKSPACE=/tmp
ln -s $GIT_DIR $WORKSPACE/source_tree || true

echo "======================================================================="
echo "Prepare binary tree ..."
rm -rf $WORKSPACE/binary_tree/ $WORKSPACE/binaries/
mkdir -p $WORKSPACE/binary_tree/infinity
mkdir -p $WORKSPACE/binary_tree/compact
mkdir -p $WORKSPACE/binary_tree/server
mkdir -p $WORKSPACE/binary_tree/nephila
mkdir -p $WORKSPACE/binary_tree/tools

echo "Prepare release directory for binaries ..."
mkdir -p $WORKSPACE/binaries/release




echo "======================================================================="
echo "Prepare makefiles ..."

cd $WORKSPACE/binary_tree/infinity
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_NUMBER=$BUILD_NUMBER $WORKSPACE/source_tree -DBUILD_INFINITY=ON

cd $WORKSPACE/binary_tree/compact
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_NUMBER=$BUILD_NUMBER $WORKSPACE/source_tree -DBUILD_INFINITY_COMPACT=ON

cd $WORKSPACE/binary_tree/server
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_NUMBER=$BUILD_NUMBER $WORKSPACE/source_tree -DBUILD_INFINITY_SERVER=ON 

cd $WORKSPACE/binary_tree/nephila
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_NUMBER=$BUILD_NUMBER $WORKSPACE/source_tree -DBUILD_NEPHILA=ON 

cd $WORKSPACE/binary_tree/tools
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_NUMBER=$BUILD_NUMBER $WORKSPACE/source_tree -DBUILD_TOOLS=ON 



echo "======================================================================="
echo "Build infinity release version ..."
cd $WORKSPACE/binary_tree/infinity
make -j 4 package

mv *.deb $WORKSPACE/binaries/release


echo "======================================================================="
echo "Build compact infinity release version ..."
cd $WORKSPACE/binary_tree/compact
make -j 4 package

mv *.deb $WORKSPACE/binaries/release


echo "======================================================================="
echo "Build infinity server release version ..."
cd $WORKSPACE/binary_tree/server
make -j 4 package

mv *.deb $WORKSPACE/binaries/release


echo "======================================================================="
echo "Build nephila release version ..."
cd $WORKSPACE/binary_tree/nephila
make -j 4 package

mv *.deb $WORKSPACE/binaries/release

echo "======================================================================="
echo "Build corelink tools ..."
cd $WORKSPACE/binary_tree/tools
make -j 4 package
mv *.deb $WORKSPACE/binaries/tools		


echo "======================================================================="
echo "Build documentation..."
cd $WORKSPACE/source_tree/Documentation
make -j 4

