#include <sstream> // osstringstream
#include <string>

class Event {
    //representation of the change that occured in the system per account

    public:
        std::string action;
        std::string account;
        std::string amount;

        Event(){};        
        void apply() {};
        const std::string toString() {
            std::ostringstream os;
            os << "Action: " << action << " Account: " << account << " Amount: " << amount << std::endl;        
            return os.str();
        };
};
