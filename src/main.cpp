#include <iostream>       // std::cout
#include <queue>          // std::queue
#include <sstream>        // osstringstream
#include <assert.h>       //for test case
#include <string>         //str()
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Service.h"
#include "../lib/crowapi/crow_all.h"


#include <thread>
#include <future>
#include <mutex>


int main()
{
    //Init API to process commands
    crow::SimpleApp app;

    // TEST EVENT LOGGING W/ MEM MAPPED FILE
    // Event event = Event();
    // event.action = "test_action";
    // event.account = "test_account";
    // event.amount = "test_amount";
    // event.parent_cmd_id = "test_parent_cmd_id";

    // std::cout << event << std::endl;
    // {
    //     // std::ofstream ofs(FILENAME, ios::app);
    //     // boost::archive::text_oarchive oa(ofs);
    //     // oa << number_of_objects;
    //     // for (size_t i = 0; i != number_of_objects; ++i) {
    //     //     oa << data[i];
    //     // }
    //     std::ofstream ofs("/tmp/event-log.bin");
    //     boost::archive::binary_oarchive oa(ofs);
    //     oa << event;
    // }
    // Event new_event;
    // std::ifstream ifs("/tmp/event-log.bin");
    // {
    //     // std::ifstream ifs(FILENAME);
    //     // boost::archive::text_iarchive ia(ifs);
    //     // size_t number_of_objects;
    //     // ia >> number_of_objects;
    //     // // allocate number_of_objects objects
    //     // for (size_t i = 0; i != number_of_objects; ++i){ 
    //     //     ia >> data[i];
    //     // }
    //     boost::archive::binary_iarchive ia(ifs);
    //     ia >> new_event;
    // }
    // std::cout << new_event << std::endl;
  
    //Init Service
    Service service = Service();

    // TEST setup some dummy accounts
    rocksdb::Slice key1 = "accountA";
    std::string value1 = "100";
    rocksdb::Slice key2 = "accountB";
    std::string value2 = "100";

    // TEST modify the database
    service._status = service._accountDB->Put(rocksdb::WriteOptions(), key1, value1);
    assert(service._status.ok());
    service._status = service._accountDB->Put(rocksdb::WriteOptions(), key2, value2);
    assert(service._status.ok());
   

    //TEST BALENCE XFER LOGIC- Create a command object then add it to the command file
    CROW_ROUTE(app, "/api/1.0/wallet/balance_transfer")
    ([&service](const crow::request& req) {
        std::ostringstream os;

        // To get a simple string from the url params
        // To see it in action /params?foo='blabla'
        os << "Params: " << req.url_params << "\n\n"; 
        os << "The key 'fromAccount' was " << (req.url_params.get("fromAccount") == nullptr ? "not " : "") << "found.\n";
        os << "The key 'toAccount' was " << (req.url_params.get("toAccount") == nullptr ? "not " : "") << "found.\n";
        os << "The key 'amount' was " << (req.url_params.get("amount") == nullptr ? "not " : "") << "found.\n";
        if (
            req.url_params.get("fromAccount") == nullptr ||
            req.url_params.get("toAccount") == nullptr ||
            req.url_params.get("amount") == nullptr
        ){
            return crow::response{os.str()};
        }
        std::string fromAccount = req.url_params.get("fromAccount");
        std::string toAccount = req.url_params.get("toAccount");
        std::string amount = req.url_params.get("amount");
        //TODO: validate input parameters are in correct format

        Command command = Command(fromAccount, toAccount, amount, "TRANSFER");
        
        service._command_queue.send(std::move(command));

        //TEST State change to process command
        service.toggle(); // listen -> validate


        return crow::response{os.str()};
    });

    // /api/1.0/wallet/get_balance
    CROW_ROUTE(app, "/api/1.0/wallet/get_balance")([&service](){
        std::ostringstream os;
        std::string value;

        // TEST Get value account 1
        service._status = service._accountDB->Get(service._read_options, "accountA", &value);
        assert(service._status.ok()); 
        os << "Account 1: " << value << std::endl;

        // // TEST Get value account 2
        service._status = service._accountDB->Get(service._read_options, "accountB", &value);
        assert(service._status.ok()); 
        os << "Account 2: " << value << std::endl;

        return os.str();
    });

        CROW_ROUTE(app, "/api/1.0/wallet/events")([](){
        std::ostringstream os;
        //TEST open event log through mem mapped file
        Event new_event1 = Event();
        Event new_event2 = Event();
        std::ifstream ifs("/tmp/event-log.txt");
        {
            boost::archive::text_iarchive ia(ifs);
            ia >> new_event1 >> new_event2;
        }
        os << new_event1.toString() << std::endl << new_event2.toString();
        return os.str();
    });

    app.port(18080).multithreaded().run();
  
}