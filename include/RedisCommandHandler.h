#ifndef REDIS_COMMAND_HANDLER_H

#include <string>

class RedisCommandHandler {
    public: 
        RedisCommandHandler();
        // process the command from the client and convert it into RESP-formatted response
        std::string processCommand(const std::string& commandLine);
};

#endif