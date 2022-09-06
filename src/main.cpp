#include <iostream>       // std::cout
#include <queue>          // std::queue
#include <sstream>        // osstringstream
#include <assert.h>         //for test case

#include "Service.h"
#include "../lib/crowapi/crow_all.h"

int main()
{
    //Init API to process commands
    crow::SimpleApp app;

    std::queue<Command> command_queue;
    std::queue<Event> event_queue;
    
    //Init Service
    Service service = Service(command_queue,event_queue);

    // TEST setup some dummy accounts
    rocksdb::Slice key1 = "account1";
    std::string value1 = "100";
    rocksdb::Slice key2 = "account2";
    std::string value2 = "100";

    // TEST modify the database
    service._status = service._accountDB->Put(rocksdb::WriteOptions(), key1, value1);
    assert(service._status.ok());
    service._status = service._accountDB->Put(rocksdb::WriteOptions(), key2, value2);
    assert(service._status.ok());


    //TEST BALENCE XFER LOGIC- Create a command object then add it to the command file
    CROW_ROUTE(app, "/api/1.0/wallet/balance_transfer")
    ([&command_queue,&service] {
        Command command = Command("account1","account2","10","TRANSFER");
        command_queue.push(command);
        //TEST State change to process command
        service.toggle(); // listen -> validate
        service.toggle(); // validate -> apply
        service.toggle(); // apply -> listen
        return "Request Receieved";
    });

    // /api/1.0/wallet/get_balance
    CROW_ROUTE(app, "/api/1.0/wallet/get_balance")([&service](){
        std::ostringstream os;
        std::string value;
        // std::string value2;         
        // rocksdb::ReadOptions _read_options;

        // TEST Get value account 1
        service._status = service._accountDB->Get(service._read_options, "account1", &value);
        assert(service._status.ok()); 
        os << "Account 1: " << value << std::endl;

        // // TEST Get value account 2
        service._status = service._accountDB->Get(service._read_options, "account2", &value);
        assert(service._status.ok()); 
        os << "Account 2: " << value << std::endl;

        return os.str();
    });

        CROW_ROUTE(app, "/api/1.0/wallet/events")([&event_queue](){
        std::ostringstream os;
        std::queue<Event> q_copy = event_queue;
        while (!q_copy.empty())
        {
            Event front = q_copy.front();
            os << front.toString() << std::endl;
            q_copy.pop();
        }
        return os.str();
    });

    app.port(18080).multithreaded().run();
}