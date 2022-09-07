#pragma once
// #include <map>
#include <queue>
#include <string>
#include <memory>         //uniq_ptr

#include "../lib/rocksdb/db.h"  //rocksdb
#include "ServiceState.h"
#include "Event.hpp"
#include "Command.hpp"

class ServiceState;

class Service {

    public:
        Service();
        Service(std::queue<Command>& command_queue);
        inline ServiceState* getCurrentState() const { return currentState; }
        void toggle();
        // This will get called by the current state
	    void setState(ServiceState& newState);
        // std::map<std::string, std::string> *_state_map;
        rocksdb::DB* _accountDB; //Init a rockdb handle for database access
        rocksdb::Status _status;
        rocksdb::ReadOptions _read_options;
        rocksdb::Options _options; 
        // std::queue<Event> *_event_queue;
        std::queue<Command> *_command_queue;                
        ServiceState *currentState;        
    private:
        void init();
};
