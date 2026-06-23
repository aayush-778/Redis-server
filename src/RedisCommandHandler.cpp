#include "../include/RedisCommandHandler.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

// RESP parser
// *2\r\n$4\r\n\PING\r\n$4\r\nTEST\r\n
// *2 -> array has two elements
// $4 -> next string has 4 characters
// PING
// TEST

vector<string> parseRespCommand(const string& input) {
    vector<string> tokens;
    if(input.empty()) return tokens;

    // If input doesnt start with a '*', fallback to splitting by whitespace
    if(input[0] != '*') {
        istringstream iss(input);
        string token;
        while(iss >> token)
            tokens.push_back(token);
        return tokens;        
    }

    size_t pos = 0;
    if(input[pos] != '*') return tokens;
    pos++; // skip '*'

    // crlf = Carriage Return (\r), Line Feed (\n)
    size_t crlf = input.find("\r\n", pos);
    if(crlf == string::npos) return tokens;

    int numElements = stoi(input.substr(pos, crlf - pos));
    pos = crlf + 2;

    for(int i=0; i<numElements; i++) {
        if(pos >= input.size() || input[pos] != '$') break;
        pos++; // skip '$'

        crlf = input.find("\r\n", pos);
        if(crlf == string::npos) break;
        int len = stoi(input.substr(pos, crlf - pos));
        pos = crlf + 2;

        if(pos + len > input.size()) break;
        string token = input.substr(pos, len);
        tokens.push_back(token);
        pos += len + 2; // skip token and crlf
    }

    return tokens;
}

RedisCommandHandler:: RedisCommandHandler() {}

string RedisCommandHandler::processCommand(const string& commandLine) {
    // Use RESP parser
    auto tokens = parseRespCommand(commandLine);
    if(tokens.empty()) return "-Error: Empty Command\r\n";

    string cmd = tokens[0];
    transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    ostringstream response;

    // Connect to the server

    // Check commands

    return response.str();
}
