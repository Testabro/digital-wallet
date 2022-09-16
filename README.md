# digital-wallet  
C++ backend wallet service. This is a backend service that provides high volume REST-like API functions to client processes. It is meant to be a stand alone service were multiple instances can be managed by an orchestrator in the future.

## Prereqs  
Base source  
```
sudo apt-get install libc6-dev g++ libboost-all-dev cmake
```
Rocksdb Integration  
see install guide for reference https://github.com/facebook/rocksdb/blob/main/INSTALL.md  
```
sudo apt-get install libsnappy-dev libbz2-dev
```  

## Build  
```
mkdir build  
```
```
cd build  
```
```
git clone https://github.com/facebook/rocksdb.git
```
```
cd rocksdb
```
```
make static_lib
```
```
mv librocksdb.a ..
```
```
cd .. & cmake ..  
```
```
make  
```

### Alternative Build  
Using the RockDB integration  
```
 g++ --std=c++17 -o wallet-service ../src/ConcreteServiceState.cpp ../src/Service.cpp ../src/main.cpp -L. -lrocksdb -lsnappy -lpthread -lbz2 -lz -lrt -ldl -lboost_serialization
```
## Integrations  
using crow c++ API (https://github.com/CrowCpp/Crow)  
using boost.io for mmapped command and event files  
using rocksdb for key-value store of account and balences

## Design

![Event Sourceing](event-sourcing.png =250x250)  
<img src="event-sourcing.png " width="400" height="400">

![Service State Machine](service-statemachine.png =250x250)


