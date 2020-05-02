#!/bin/bash
set -e

cd /git/study/qt/study/QTScript/shellDebugQt/project
rm -rf Makefile out
qmake
make
