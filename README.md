# digital-wallet  
C++ backend wallet service

## Prereqs  
Base source  
```
sudo apt-get install libc6-dev g++ libboost-all-dev cmake
```
Rocksdb Integration  
see install guide for reference https://github.com/facebook/rocksdb/blob/main/INSTALL.md  
```
sudo apt-get install libsnappy-dev sudo apt-get install libbz2-dev
```  

## Build  
```
mkdir build  
```
```
cd build  
```
```
cmake ..  
```
```
make  
```

### Alternative Build  
```
g++ --std=c++17 -pthread -o wallet-service ConcreteServiceState.cpp Service.cpp main.cpp -lrt  
```
Using the RockDB integration
```
g++ --std=c++17 -o wallet-service ../src/ConcreteServiceState.cpp ../src/Service.cpp ../src/main.cpp -L. -lrocksdb -lsnappy -lpthread -lbz2 -lz -lrt -ldl
```
## Integrations  
using crow c++ API (https://github.com/CrowCpp/Crow)  
using boost.io for mmapped command and event files  
using rocksdb for key-value store of account and balences  
