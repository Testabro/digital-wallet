#pragma once
#include <sstream> // osstringstream
#include <string>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

class Event {
    //representation of the change that occured in the system per account

    public:
        std::string action;
        std::string account;
        std::string amount;
        std::string parent_cmd_id;

        Event(){};
        friend std::ostream& operator<<(std::ostream& os, const Event event) {
           return os << event.parent_cmd_id << " " << event.account << " " << event.amount << " " << event.action;
        }

        void apply() {};
        const std::string toString() {
            std::ostringstream os;
            os << "Action: " << action << " Account: " << account << " Amount: " << amount << std::endl;        
            return os.str();
        };
        
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive &ar,
        const unsigned int version){
            ar & action;
            ar & account;
            ar & amount;
            ar & parent_cmd_id;
        }
};
