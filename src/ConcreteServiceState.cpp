#include <iostream>       // std::cout
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ConcreteServiceState.h"
#include "MessageQueue.h"

void ServiceListen::toggle(Service* service) {
    std::cout << "Listen toggle" << std::endl; // DEBUG output
    service->command_to_process = service->_command_queue.receive();
    // listen -> validate
    std::cout << "Listen toggle SUCCESS" << std::endl; // DEBUG output
    service->setState(ServiceValidate::getInstance());
    service->process();
}

ServiceState& ServiceListen::getInstance() {
    std::cout << "Get listen state" << std::endl; // DEBUG output
	static ServiceListen singleton;
	return singleton;
}

void ServiceValidate::process(Service* service) {
    std::cout << "Validate toggle" << std::endl; // DEBUG output
    std::string balance;

    //VALIDATE A TRANSFER COMMAND
        service->_status = service->_accountDB->Get(service->_read_options, service->command_to_process.getAccount2(), &balance);
    if (!service->_status.ok()) { 
        std::cout << "Account not found" << std::endl;
        service->setState(ServiceListen::getInstance());
        return;        
    }
    assert(service->_status.ok());

    service->_status = service->_accountDB->Get(service->_read_options, service->command_to_process.getAccount1(), &balance);
    if (!service->_status.ok()) { 
        std::cout << "Account not found" << std::endl;
        service->setState(ServiceListen::getInstance());
        return;        
    }
    assert(service->_status.ok());

    if (std::stoi(balance) <= std::stoi(service->command_to_process.getAmount())) {
        std::cerr << "Balance too low" << std::endl;
        service->setState(ServiceListen::getInstance());
        return;
    }

    if (service->command_to_process.getAction() == "TRANSFER") {
        // validate -> apply
        std::cout << "Validate toggle SUCCESS" << std::endl; // DEBUG output
        service->setState(ServiceApply::getInstance());
        service->process();
        return; // TODO consider a way to no have previous state in call stack; I consider this a defect
    }

    std::cout << "Command is not valid" << std::endl; // DEBUG output    
    service->setState(ServiceListen::getInstance()); 
}

ServiceState& ServiceValidate::getInstance() {
	static ServiceValidate singleton;
	return singleton;
}

void ServiceApply::process(Service* service) {
    std::cout << "Apply toggle" << std::endl; // DEBUG output

    Event eventA = Event();
    Event eventB = Event();

    //credit event
    eventA.account = service->command_to_process.getAccount1();
    eventA.amount = service->command_to_process.getAmount();
    eventA.action = service->command_to_process.getAction();
    eventA.parent_tx_id = service->command_to_process.getID();

    //debit event
    eventB.account = service->command_to_process.getAccount2();
    eventB.amount = service->command_to_process.getAmount();
    eventB.action = service->command_to_process.getAction();
    eventB.parent_tx_id = service->command_to_process.getID();

    //Update State
    std::string balance;
    service->_status = service->_accountDB->Get(service->_read_options, eventA.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceA = std::stoi(balance);

    service->_status = service->_accountDB->Get(service->_read_options, eventB.account, &balance);
    assert(service->_status.ok());    
    int accountBalanceB = std::stoi(balance);

    int balance1 = accountBalanceA - std::stoi(service->command_to_process.getAmount());
    int balance2 = accountBalanceB + std::stoi(service->command_to_process.getAmount());

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
