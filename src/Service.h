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
        void process(); //Delagate to current state
        // This will get called by the current state
	    void setState(ServiceState& newState);

        rocksdb::DB* _accountDB; //Init a rockdb handle for database access
        rocksdb::Status _status;
        rocksdb::ReadOptions _read_options;
        rocksdb::Options _options; 
        MessageQueue<Command> _command_queue;
        ServiceState *currentState;
        Command command_to_process;
        std::condition_variable cv;
        std::mutex cv_m; 
                
    private:
        void init();
};
