#include <iostream>

#include "Service.h"
#include "ConcreteServiceState.h"

Service::Service() {
	currentState = &ServiceListen::getInstance();
	init();
}

void Service::setState(ServiceState& newState) {
	currentState = &newState;  // change state
	currentState->onEnter(this);
}

void Service::toggle() {
	currentState->toggle(this);
}

void Service::process() {
	currentState->process(this);
}

void Service::init() {
    //Init DB with options for account balances  
    _options.create_if_missing = true;
    //options.error_if_exists = true; // DEBUG
    _status = rocksdb::DB::Open(_options, "/tmp/testdb", &_accountDB);
    assert(_status.ok());
}
