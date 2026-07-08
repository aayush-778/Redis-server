#include "../include/RedisServer.h"
#include "../include/RedisCommandHandler.h"
#include "../include/RedisDatabase.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <cstring>
#include <signal.h>
using namespace std;

static RedisServer *global_server = nullptr;

void signalHandler(int signum) {
    if (global_server) {
        cout << "\nCaught signal" << signum << ", shutting down...\n";
        global_server->shutdown();
    }
    exit(signum);
}

void RedisServer::setupSignalHandler() {
    signal(SIGINT, signalHandler);
}

RedisServer::RedisServer(int port) : port(port), server_socket(-1), running(true) {
    global_server = this;
    setupSignalHandler();
}

void RedisServer::shutdown() {
    running = false;
    if (server_socket != -1) {
        if (!RedisDatabase::getInstance().dump("dump.my_rdb"))
            cerr << "Error dumping database\n";
        else
            cout << "Database dumped to dump.my_rdb\n";
        close(server_socket);
    }
    cout << "Server shutdown complete!\n";
}

void RedisServer::run() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error creating server socket" << endl;
        return;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error binding service socket" << endl;
        return;
    }

    if (listen(server_socket, 10) < 0) {
        cerr << "Error listening on server socket" << endl;
        return;
    }

    cout << "Redis server listening on port: " << port << endl;

    vector<thread> threads;
    RedisCommandHandler cmdHandler;

    while (running) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            if (running)
                cerr << "Error accepting Client connection" << endl;
            break;
        }

        threads.emplace_back([client_socket, &cmdHandler]() {
            char buffer[1024];
            while(true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
                if(bytes <= 0) break;
                
                string request(buffer, bytes);
                string response = cmdHandler.processCommand(request);
                send(client_socket, response.c_str(), response.size(), 0);
            }
            close(client_socket); });

        for (auto &t : threads) {
            if (t.joinable())
                t.join();
        }

        // Before shutdown, persist the database
        if (!RedisDatabase::getInstance().dump("dump.my_rdb"))
            cerr << "Error dumping database\n";
        else
            cout << "Database dumped to dump.my_rdb\n";
    }
}