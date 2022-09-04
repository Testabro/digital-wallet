#include <iostream>       // std::cout
#include <queue>          // std::queue
#include <sstream>        // osstringstream
#include "Service.h"
#include "../lib/crow_all.h"

int main()
{
    crow::SimpleApp app;
    std::queue<Command> command_queue;
    std::queue<Event> event_queue;
    std::map<std::string,std::string> state_map;
    state_map["account1"] = "100";
    state_map["account2"] = "100";

    Service service = Service(command_queue,event_queue,state_map);

    //TEST BALENCE XFER LOGIC- Create a command object then add it to the command file
    CROW_ROUTE(app, "/api/1.0/wallet/balance_transfer")
    ([&command_queue,&service] {
        Command command = Command("account1","account2","10","TRANSFER");
        command_queue.push(command);
        //TEST State change to process command
        service.toggle(); // listen -> validate
        service.toggle(); // validate -> apply
        service.toggle(); // apply -> listen
        return "Request Receieved";
    });


    CROW_ROUTE(app, "/api/1.0/wallet/get_balance")([&state_map](){
        std::ostringstream os;
        os << "Account 1: " << state_map["account1"] << std::endl;
        os << "Account 2: " << state_map["account2"] << std::endl;
        return os.str();
    });

        CROW_ROUTE(app, "/api/1.0/wallet/events")([&event_queue](){
        std::ostringstream os;
        std::queue<Event> q_copy = event_queue;
        while (!q_copy.empty())
        {
            Event front = q_copy.front();
            os << front.toString() << std::endl;
            q_copy.pop();
        }
        return os.str();
    });

    app.port(18080).multithreaded().run();
}