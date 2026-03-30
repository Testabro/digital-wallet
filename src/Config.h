#pragma once
#include <string>
#include <cstdlib>

struct Config {
    std::string db_path = "./data/walletdb";
    std::string seed_file = "./data/account-seed.dat";
    std::string event_log = "./data/event-log.txt";
    int port = 18080;

    Config() {
        if (auto env = std::getenv("WALLET_DB_PATH")) db_path = env;
        if (auto env = std::getenv("WALLET_SEED_FILE")) seed_file = env;
        if (auto env = std::getenv("WALLET_EVENT_LOG")) event_log = env;
        if (auto env = std::getenv("WALLET_PORT")) port = std::atoi(env);
    }
};
