#pragma once
#include <string>
#include <iostream>
#include <fstream>
// #include <boost/archive/binary_oarchive.hpp>
// #include <boost/archive/binary_iarchive.hpp>
#include <boost/random.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>


class Command {
    //action to be performed to change the state of the system;
    public:
        Command() = default;
        Command(std::string account1, std::string account2, std::string amount, std::string action) :
           _account1(account1), _account2(account2), _amount(amount), _action(action){ _tx_id=generateID(); }
        friend std::ostream& operator<<(std::ostream& os, const Command command) {
           return os << command._tx_id << " " << command._account1 << " " << command._account2 
              << " " << command._amount << " " << command._action;
        }

        std::string getID() { return _tx_id; }; //DEBUG
        std::string getAction() { return _action; };
        std::string getAccount1() { return _account1; };
        std::string getAccount2() { return _account2; };
        std::string getAmount() { return _amount; };
        // Generate random id; TODO create id manager process and delagate this
        std::string generateID() {
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            const std::string uuid_string = boost::lexical_cast<std::string>(uuid);
            return uuid_string;
        };

    private:
        std::string _tx_id;
        std::string _account1;
        std::string _account2;
        std::string _amount;
        std::string _action;

        // friend class boost::serialization::access;
        // template<class Archive>
        // void serialize(Archive &ar,
        // const unsigned int version){
        //     ar & _id;
        //     ar & _account1;
        //     ar & _account2;
        //     ar & _amount;
        //     ar & _action;
        // }
};