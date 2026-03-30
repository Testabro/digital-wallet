#include <iostream>
#include <iomanip>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <filesystem>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Config.h"
#include "Service.h"
#include "../lib/crowapi/crow_all.h"

static Config config;
static std::mutex seed_mutex;

void generateSeedFile( const std::string& seed_file ) {
    if ( !std::filesystem::exists( seed_file ) ) {
        int init_num = 0;
        //Create seed file for count
        {
            std::ofstream ofs( seed_file );
            boost::archive::text_oarchive oa(ofs);
            oa << init_num;
        }
    }
}

std::string genAccountID( const std::string& seed_file ) {
    std::lock_guard<std::mutex> lock(seed_mutex);

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
    std::filesystem::create_directories("./data");

    crow::SimpleApp app;

    Service service = Service();

    CROW_ROUTE(app, "/api/1.0/wallet/create_account")
    ([&service](const crow::request& req) {
        std::ostringstream os;

        auto startAmountParam = req.url_params.get("startAmount");
        if (startAmountParam == nullptr) {
            os << "Missing required parameter: startAmount" << std::endl;
            return crow::response(400, os.str());
        }

        int startAmount;
        try {
            startAmount = std::stoi(startAmountParam);
        } catch (const std::exception&) {
            os << "Invalid startAmount: must be an integer" << std::endl;
            return crow::response(400, os.str());
        }

        if (startAmount < 0) {
            os << "Invalid startAmount: must not be negative" << std::endl;
            return crow::response(400, os.str());
        }

        std::string accountID = genAccountID(config.seed_file);
        rocksdb::Slice key = accountID;
        std::string value = std::to_string(startAmount);

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
            return crow::response(400, os.str());
        }
        std::string fromAccount = req.url_params.get("fromAccount");
        std::string toAccount = req.url_params.get("toAccount");
        std::string amountStr = req.url_params.get("amount");

        int amount;
        try {
            amount = std::stoi(amountStr);
        } catch (const std::exception&) {
            os << "Invalid amount: must be an integer" << std::endl;
            return crow::response(400, os.str());
        }

        if (amount <= 0) {
            os << "Invalid amount: must be a positive integer" << std::endl;
            return crow::response(400, os.str());
        }

        Command command = Command(fromAccount, toAccount, std::to_string(amount), "TRANSFER");
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
        if (std::filesystem::exists( config.event_log )) {
            {
                std::ifstream ifs(config.event_log);
                boost::archive::text_iarchive ia(ifs);
                ia >> event1 >> event2;
            }
        }

        os << event1.toString() << std::endl << event2.toString() << std::endl;

        return os.str();
    });

    app.port(config.port).multithreaded().run();

}
