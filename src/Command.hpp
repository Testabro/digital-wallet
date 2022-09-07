#include <string>
#include <ctime>
#include <boost/random.hpp>

class Command {
    //action to be performed to change the state of the system;
    public:
        Command() = default;
        Command(std::string account1, std::string account2, std::string amount, std::string action) :
           _account1(account1), _account2(account2), _amount(amount), _action(action){ _id=generateID(); }

        std::string getID() { return _id; }; //DEBUG
        std::string getAction() { return _action; };
        std::string getAccount1() { return _account1; };
        std::string getAccount2() { return _account2; };
        std::string getAmount() { return _amount; };
        // Generate random id; TODO create id manager process and delagate this
        std::string generateID() {
            std::time_t now = std::time(0);
            boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
            return std::to_string(gen());
        };

    private:
        std::string _id;
        std::string _account1;
        std::string _account2;
        std::string _amount;
        std::string _action; 
};