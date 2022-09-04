# digital-wallet
C++ backend wallet

## Prereqs
sudo apt install libc6-dev g++ libboost-all-dev

## Build
g++ --std=c++17 -pthread ConcreteServiceState.cpp Service.cpp main.cpp  -lrt

## Interfaces
using crow c++ API (https://github.com/CrowCpp/Crow)
