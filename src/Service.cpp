#include <iostream>

#include "Service.h"
#include "ConcreteServiceState.h"

Service::Service(std::queue<Command>& command_queue)
{
	std::cout << "Create service" << std::endl; // DEBUG output
	_command_queue = &command_queue;
	// _event_queue = &event_queue; REMOVE

	// Service is listening initially
	currentState = &ServiceListen::getInstance();
	init();
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

void Service::init()
{
    //Init DB with options for account balances  
    _options.create_if_missing = true;
    //options.error_if_exists = true; // DEBUG
    _status = rocksdb::DB::Open(_options, "/tmp/testdb", &_accountDB); //Can move to Service::init()
    assert(_status.ok());//Can move to Service::init()
}