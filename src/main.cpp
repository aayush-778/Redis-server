#include "../include/RedisServer.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

int main(int argc, char* argv[]) {
    int port = 6379; // default
    if(argc>=2) port = stoi(argv[1]);

    RedisServer server(port);

    // Background persistance: dump the database every 300 sec. (5 minutes save database)
    thread persistanceThread([]() {
        while(true) {
            this_thread::sleep_for(chrono::seconds(300));
            // dump the database
        }
    });
    persistanceThread.detach();

    server.run();

    return 0;
}