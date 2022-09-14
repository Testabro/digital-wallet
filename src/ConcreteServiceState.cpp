#include <iostream>       // std::cout
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ConcreteServiceState.h"
#include "MessageQueue.h"

void ServiceListen::toggle(Service* service) {
    std::cout << "Listen toggle" << std::endl; // DEBUG output
    auto command = service->_command_queue.receive();
    // listen -> validate
    std::cout << "Listen toggle SUCCESS" << std::endl; // DEBUG output
    service->setState(ServiceValidate::getInstance());
    service->process(command);
}

ServiceState& ServiceListen::getInstance() {
    std::cout << "Get listen state" << std::endl; // DEBUG output
	static ServiceListen singleton;
	return singleton;
}

void ServiceValidate::process(Service* service, Command command) {
    std::cout << "Validate toggle" << std::endl; // DEBUG output
    std::string balance;

    service->_status = service->_accountDB->Get(service->_read_options, command.getAccount1(), &balance);
    assert(service->_status.ok());

    if (command.getAction() == "TRANSFER" && std::stoi(balance) >= std::stoi(command.getAmount())) {
        // validate -> apply
        std::cout << "Validate toggle SUCCESS" << std::endl; // DEBUG output
        service->setState(ServiceApply::getInstance());
        service->process(command);
    }

    std::cout << "Command is not valid" << std::endl; // DEBUG output
    service->setState(ServiceListen::getInstance()); 
}

ServiceState& ServiceValidate::getInstance() {
	static ServiceValidate singleton;
	return singleton;
}

void ServiceApply::process(Service* service, Command command) {
    std::cout << "Apply toggle" << std::endl; // DEBUG output

    Event eventA = Event();
    Event eventB = Event();

    //credit event
    eventA.account = command.getAccount1();
    eventA.amount = command.getAmount();
    eventA.action = command.getAction();
    eventA.parent_tx_id = command.getID();

    //debit event
    eventB.account = command.getAccount2();
    eventB.amount = command.getAmount();
    eventB.action = command.getAction();
    eventB.parent_tx_id = command.getID();

    //Update State
    std::string balance;
    service->_status = service->_accountDB->Get(service->_read_options, eventA.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceA = std::stoi(balance);

    service->_status = service->_accountDB->Get(service->_read_options, eventB.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceB = std::stoi(balance);

    int balance1 = accountBalanceA - std::stoi(command.getAmount());
    int balance2 = accountBalanceB + std::stoi(command.getAmount());

    service->_status = service->_accountDB->Put(rocksdb::WriteOptions(), eventA.account, std::to_string(balance1));
    assert(service->_status.ok());

    service->_status = service->_accountDB->Put(rocksdb::WriteOptions(), eventB.account, std::to_string(balance2));
    assert(service->_status.ok());

    //Add to event log
    {
        std::ofstream ofs("/tmp/event-log.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << eventA << eventB;
    }

    // apply -> listen
    service->setState(ServiceListen::getInstance());
}

ServiceState& ServiceApply::getInstance() {
	static ServiceApply singleton;
	return singleton;
}
