#include "ConcreteServiceState.h"
#include <iostream>       // std::cout
#include <string>

void ServiceListen::toggle(Service* service) {
    std::cout << "Listen toggle" << std::endl; // DEBUG output
    if (!service->_command_queue->empty()) {
        // listen -> validate
        std::cout << "Listen toggle SUCCESS" << std::endl; // DEBUG output
	    service->setState(ServiceValidate::getInstance());
        return;
    }
    std::cout << "Command queue is empty" << std::endl; // DEBUG output
}

ServiceState& ServiceListen::getInstance() {
    std::cout << "Get listen state" << std::endl; // DEBUG output
	static ServiceListen singleton;
	return singleton;
}


void ServiceValidate::toggle(Service* service) {
    std::cout << "Validate toggle" << std::endl; // DEBUG output
    Command command = service->_command_queue->front();
    std::string balance;

    service->_status = service->_accountDB->Get(service->_read_options, "account1", &balance);
    assert(service->_status.ok());
    if (command.getAction() == "TRANSFER" && std::stoi(balance) >= std::stoi(command.getAmount())) {
        // validate -> apply
        std::cout << "Validate toggle SUCCESS" << std::endl; // DEBUG output
        service->setState(ServiceApply::getInstance());
        return;
    }

    std::cout << "Command is not valid" << std::endl; // DEBUG output
    service->setState(ServiceListen::getInstance()); 
}

ServiceState& ServiceValidate::getInstance() {
	static ServiceValidate singleton;
	return singleton;
}

void ServiceApply::toggle(Service* service) {
    std::cout << "Apply toggle" << std::endl; // DEBUG output
    //Get command from the front of the queue
    Command command = service->_command_queue->front();
    //Remove the command from the queue
    service->_command_queue->pop();
    Event eventA = Event();
    Event eventB = Event();

    //credit event
    eventA.account = command.getAccount1();
    eventA.amount = command.getAmount();

    //debit event
    eventB.account = command.getAccount2();
    eventB.amount = command.getAmount();

    //Update State
    std::string balance;
    service->_status = service->_accountDB->Get(service->_read_options, eventA.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceA = std::stoi(balance);

    service->_status = service->_accountDB->Get(service->_read_options, eventA.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceB = std::stoi(balance);

    int balance1 = accountBalanceA - std::stoi(command.getAmount());
    int balance2 = accountBalanceB + std::stoi(command.getAmount());

    service->_status = service->_accountDB->Put(rocksdb::WriteOptions(), eventA.account, std::to_string(balance1));
    assert(service->_status.ok());

    service->_status = service->_accountDB->Put(rocksdb::WriteOptions(), eventB.account, std::to_string(balance2));
    assert(service->_status.ok());

    //Add to event log
    service->_event_queue->push(eventA);
    service->_event_queue->push(eventA);

    // apply -> listen
    service->setState(ServiceListen::getInstance());
}

ServiceState& ServiceApply::getInstance() {
	static ServiceApply singleton;
	return singleton;
}
