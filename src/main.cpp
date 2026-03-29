#include <iostream>
#include <queue>
#include <sstream>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "Service.h"
#include "../lib/crowapi/crow_all.h"

void generateSeedFile( std::string seed_file ) {
    if ( !boost::filesystem::exists( seed_file ) ) {
        int init_num = 0;
        //Create seed file for count
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
    //Add new incremented account number to seed file
    {
        std::ofstream ofs(seed_file);
        boost::archive::text_oarchive oa(ofs);
        oa << account_num;
    }     

    std::ostringstream os;

    os << "7" << std::setw(8) << std::setfill('0') << account_num;
    return os.str();
}

int main()
{
    crow::SimpleApp app;

    Service service = Service();
   
    CROW_ROUTE(app, "/api/1.0/wallet/create_account")
    ([&service](const crow::request& req) {
        std::ostringstream os;

        if (req.url_params.get("startAmount") == nullptr) {
            os << "Missing required parameter: startAmount" << std::endl;
            return crow::response{os.str()};
        }

        std::string accountID = genAccountID("/tmp/account-seed.dat");
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

        if (req.url_params.get("fromAccount") == nullptr ||
            req.url_params.get("toAccount") == nullptr ||
            req.url_params.get("amount") == nullptr) {
            os << "Missing required parameters: fromAccount, toAccount, amount" << std::endl;
            return crow::response{os.str()};
        }
        std::string fromAccount = req.url_params.get("fromAccount");
        std::string toAccount = req.url_params.get("toAccount");
        std::string amount = req.url_params.get("amount");

        Command command = Command(fromAccount, toAccount, amount, "TRANSFER");        
        service._command_queue.send(std::move(command));

        // Wait until the state machine is in LISTEN state, then trigger processing
        std::unique_lock<std::mutex> lk(service.cv_m);
        service.cv.wait(lk, [&service]{ return service.currentState->getStateName() == "LISTEN"; });
        service.toggle();
        os << "Return Status: " << service._status.ToString() << std::endl;
        return crow::response{os.str()};
    });

    CROW_ROUTE(app, "/api/1.0/wallet/get_balance")([&service](const crow::request& req){
        std::ostringstream os;
        std::string value;

        if (req.url_params.get("accountID") == nullptr) {
            os << "Missing required parameter: accountID" << std::endl;
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
        Event event1 = Event();
        Event event2 = Event();
        if (boost::filesystem::exists( "/tmp/event-log.txt" )) {
            {
                std::ifstream ifs("/tmp/event-log.txt");
                boost::archive::text_iarchive ia(ifs);
                ia >> event1 >> event2;
            }
        }

        os << event1.toString() << std::endl << event2.toString() << std::endl;

        return os.str();
    });

    app.port(18080).multithreaded().run();
  
}