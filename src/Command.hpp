#include <string>
#include <boost/multiprecision/cpp_int.hpp>

class Command {
    //action to be performed to change the state of the system;
    public:
        Command(std::string account1, std::string account2, std::string amount, std::string action) {
             _account1 = account1;
             _account2 = account2;
             _amount = amount;
             _action = action;
             _id = generateID(); //DEBUG
        };
        std::string getID() { return _id; }; //DEBUG
        std::string getAction() { return _action; };
        std::string getAccount1() { return _account1; };
        std::string getAccount2() { return _account2; };
        std::string getAmount() { return _amount; };
        // Generate random 128-bit id; TODO create id manager process and delagate this
        std::string generateID() {
            boost::multiprecision::int128_t int64left = CreateRandomInt64ViaBoost()
            boost::multiprecision::int128_t int64right = CreateRandomInt64ViaBoost()
            boost::multiprecision::int128_t randomInt = int64left << 64 | int64right;
            return std::to_string(randomInt);
        };

    private:
        std::string _account1;
        std::string _account2;
        std::string _amount;
        std::string _action;
        std::string _id;
};