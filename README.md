# digital-wallet  
C++ backend wallet service

## Prereqs  
```
sudo apt install libc6-dev g++ libboost-all-dev cmake  
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
g++ --std=c++17 -pthread ConcreteServiceState.cpp Service.cpp main.cpp -lrt  
```
## Integrations  
using crow c++ API (https://github.com/CrowCpp/Crow)  
using boost.io for mmapped command and event files  
using rocksdb for key-value store of account and balences  
