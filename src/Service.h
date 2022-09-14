#pragma once
#include <queue>
#include <string>

#include "../lib/rocksdb/db.h"  //rocksdb
#include "ServiceState.h"
#include "Event.hpp"
#include "Command.hpp"
#include "MessageQueue.h"

class ServiceState;

class Service {

    public:
        Service();
        inline ServiceState* getCurrentState() const { return currentState; }
        void toggle(); //Delagate to current state
        void process(Command command); //Delagate to current state
        // This will get called by the current state
	    void setState(ServiceState& newState);

        rocksdb::DB* _accountDB; //Init a rockdb handle for database access
        rocksdb::Status _status;
        rocksdb::ReadOptions _read_options;
        rocksdb::Options _options; 
        MessageQueue<Command> _command_queue;
        ServiceState *currentState;
                
    private:
        void init();
};
