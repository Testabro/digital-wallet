#pragma once
#include <queue>
#include <string>

#include "../lib/rocksdb/db.h"  //rocksdb
#include "ServiceState.h"
#include "Event.hpp"
#include "Command.hpp"

class CommandQueue {
    public:
        std::queue<Command> _command_queue;
        void enqueue(Command command) {
            _command_queue.push(command);
            // Notify();
        }
        Command dequeue() {
            Command front = _command_queue.front();
            _command_queue.pop();
            return front;
        }
        Command& peek() {
            return _command_queue.front();
        }
};
class ServiceState;

class Service {

    public:
        Service();
        inline ServiceState* getCurrentState() const { return currentState; }
        void toggle();
        // This will get called by the current state
	    void setState(ServiceState& newState);

        rocksdb::DB* _accountDB; //Init a rockdb handle for database access
        rocksdb::Status _status;
        rocksdb::ReadOptions _read_options;
        rocksdb::Options _options; 
        CommandQueue _command_queue;                
        ServiceState *currentState;        
    private:
        void init();
};
