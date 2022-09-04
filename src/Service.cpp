#include "Service.h"
#include "ConcreteServiceState.h"
#include <iostream>

Service::Service(std::queue<Command>& command_queue, std::queue<Event>& event_queue, std::map<std::string,std::string>& state_map)
{
	std::cout << "Create service"; // DEBUG output
	_command_queue = &command_queue;
	_event_queue = &event_queue;
	_state_map = &state_map;
	// Service is listening initially
	currentState = &ServiceListen::getInstance();
}

void Service::setState(ServiceState& newState)
{
	currentState->exit(this);  // do stuff before we change state
	currentState = &newState;  // change state
	currentState->enter(this); // do stuff after we change state
}

void Service::toggle()
{
	std::cout << "Service toggle" << std::endl; // DEBUG output
	// Delegate the task of determining the next state to the current state!
	currentState->toggle(this);
}