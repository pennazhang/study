rm -rf *.o adaptor proxy
g++ adaptor.cpp -ldbus-c++-1 -o adaptor
g++ proxy.cpp -ldbus-c++-1 -o proxy
