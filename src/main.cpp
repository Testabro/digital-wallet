#include <iostream>       // std::cout
#include <queue>          // std::queue
#include <sstream>        // osstringstream
#include <assert.h>       //for test case
#include <string>         //str()

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "Service.h"
#include "../lib/crowapi/crow_all.h"

void generateSeedFile( std::string seed_file ) {
    if ( !boost::filesystem::exists( seed_file ) ) {
        int init_num = 0;
        //Add to event log
        {
            std::ofstream ofs( seed_file );
            boost::archive::text_oarchive oa(ofs);
            oa << init_num;
        }
    }
}

std::string genAccountID( std::string seed_file ) {
    //Init account generation seed file if it does not exist
    generateSeedFile( seed_file );

    int account_num = 0;
    {
        std::ifstream ifs(seed_file);
        boost::archive::text_iarchive ia(ifs);
        ia >> account_num;
    }    
    account_num++;
    //Add to event log
    {
        std::ofstream ofs(seed_file);
        boost::archive::text_oarchive oa(ofs);
        oa << account_num;
    }     

    std::ostringstream os;

    os << "7" << std::setw(8) << std::setfill('0') << account_num;
    std::cout << os.str();
    return os.str();
}

int main()
{
    //Init API to process commands
    crow::SimpleApp app;

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
   
    CROW_ROUTE(app, "/api/1.0/wallet/create_account")
    ([&service](const crow::request& req) {
        std::ostringstream os;

        // To get a simple string from the url params
        // To see it in action /params?foo='blabla'
        os << "Params: " << req.url_params << "\n\n"; 
        os << "The key 'startAmount' was " << (req.url_params.get("startAmount") == nullptr ? "not " : "") << "found.\n";
        if ( req.url_params.get("startAmount") == nullptr ){
            return crow::response{os.str()};
        }
        
        std::string accountID = genAccountID("/tmp/account-seed.dat");
        std::cout << accountID;
        rocksdb::Slice key = accountID;
        std::string value = req.url_params.get("startAmount");

        // Add the new account to the database
        service._status = service._accountDB->Put(rocksdb::WriteOptions(), key, value);
        assert(service._status.ok());

        os << "Account: " << accountID << " : Balance: " << value << std::endl;

        return crow::response{os.str()};
    });

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

        Command command = Command(fromAccount, toAccount, amount, "TRANSFER");        
        service._command_queue.send(std::move(command));

        //Wait until the STATE MACHINE(SERVICE) is in a LISTEN state then request to process the command
        std::unique_lock<std::mutex> lk(service.cv_m);
        std::cerr << "Waiting... \n";
        service.cv.wait(lk, [&service]{ return service.currentState->getStateName() == "LISTEN"; });
        service.toggle();

        std::cerr << "...finished waiting. StateMachine in listen state\n";
        os << "Return Status: " << service._status.ToString() << std::endl;
        return crow::response{os.str()};
    });

    CROW_ROUTE(app, "/api/1.0/wallet/get_balance")([&service](const crow::request& req){
        std::ostringstream os;
        std::string value;

        // To get a simple string from the url params
        // To see it in action /params?foo='blabla'
        os << "Params: " << req.url_params << "\n\n"; 
        os << "The key 'accountID' was " << (req.url_params.get("accountID") == nullptr ? "not " : "") << "found.\n";
         if ( req.url_params.get("accountID") == nullptr ){
            return crow::response{os.str()};
        }
        std::string accountID = req.url_params.get("accountID");

        service._status = service._accountDB->Get(service._read_options, accountID, &value);
        if (service._status.ok()) {
            os << "Account: " << accountID << " : Balance: " << value << std::endl;
        }
        if (!service._status.ok()) {
            std::cerr << "Account " << accountID << " not found."; 
            os << "Account not found" << std::endl;
        }        

        return crow::response{os.str()};
    });

    CROW_ROUTE(app, "/api/1.0/wallet/events")([](){
        std::ostringstream os;
        //TEST open event log through mem mapped file
        Event event1 = Event();
        Event event2 = Event();
        if (boost::filesystem::exists( "/tmp/event-log.txt" )) {
            {
                std::ifstream ifs("/tmp/event-log.txt");
                //TODO make it possible to query specific times, tx_id, etc.
                //   As is this will not scale and is A TEMPORARY PROOF OF FUNCTION
                boost::archive::text_iarchive ia(ifs);
                ia >> event1 >> event2;
            }
        }

        os << event1.toString() << std::endl << event2.toString() << std::endl;

        return os.str();
    });

    app.port(18080).multithreaded().run();
  
}