#include <string>

class Command {
    //action to be performed to change the state of the system;
    public:
        Command(std::string account1, std::string account2, std::string amount, std::string action) {
             _account1=account1;
             _account2=account2;
             _amount=amount;
             _action=action;
        };
        std::string getAction() { return _action; };
        std::string getAccount1() { return _account1; };
        std::string getAccount2() { return _account2; };
        std::string getAmount() { return _amount; };

    private:
        std::string _account1;
        std::string _account2;
        std::string _amount;
        std::string _action;
};