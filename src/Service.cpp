#include <iostream>

#include "Service.h"
#include "ConcreteServiceState.h"

Service::Service() {
	std::cout << "Create service" << std::endl; // DEBUG output

	// Service is listening initially
	currentState = &ServiceListen::getInstance();
	  //Start the state machine on an async thread
	init();
}

void Service::setState(ServiceState& newState) {
	currentState = &newState;  // change state
}

void Service::toggle() {
	std::cout << "Service toggle" << std::endl; // DEBUG output
	// Delegate the task of determining the next state to the current state!
	currentState->toggle(this);
}

void Service::process() {
	std::cout << "Service process" << std::endl; // DEBUG output
	// Delegate the task of determining the next state to the current state!
	currentState->process(this);
}

void Service::init() {
    //Init DB with options for account balances  
    _options.create_if_missing = true;
    //options.error_if_exists = true; // DEBUG
    _status = rocksdb::DB::Open(_options, "/tmp/testdb", &_accountDB); //Can move to Service::init()
    assert(_status.ok());//Can move to Service::init()
}
