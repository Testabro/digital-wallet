#pragma once
#include "ServiceState.h"
#include "Event.hpp"
#include "Command.hpp"
#include <map>
#include <queue>
#include <string>

class ServiceState;

class Service {

    public:
        Service();
        Service(std::queue<Command>& command_queue, std::queue<Event>& event_queue, std::map<std::string,std::string>& state_map);
        inline ServiceState* getCurrentState() const { return currentState; }
        void toggle();
        // This will get called by the current state
	    void setState(ServiceState& newState);
        std::map<std::string, std::string> *_state_map;
        std::queue<Event> *_event_queue;
        std::queue<Command> *_command_queue;                
        ServiceState *currentState;
};
