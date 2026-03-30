#include <iostream>
#include <filesystem>

#include "Config.h"
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
    static Config config;

    //Init DB with options for account balances
    _options.create_if_missing = true;
    //options.error_if_exists = true; // DEBUG
    std::filesystem::create_directories(std::filesystem::path(config.db_path).parent_path());
    _status = rocksdb::DB::Open(_options, config.db_path, &_accountDB);
    if (!_status.ok()) throw std::runtime_error("Failed to open database: " + _status.ToString());
}
