#include "Commands.hpp"
#include "Channel.hpp"
#include <iostream>
#include <algorithm>
#include <sstream> // For stringstream

// Define the global channels variable
std::map<std::string, Channel> channels;

// Implementation of your functions like handleKick, handleInvite, etc.



void processMessage(const std::string& message, int clientSockfd) {
    std::string command;
    std::string args;

    // Find the first space in the message to separate the command and arguments
    size_t spacePos = message.find(' ');
    if (spacePos != std::string::npos) {
        command = message.substr(0, spacePos);    // The command is the first word
        args = message.substr(spacePos + 1);      // The arguments are everything after the first space
    } else {
        command = message; // If no space, the entire message is the command
    }

    if (command == "KICK") {
        size_t spacePos2 = args.find(' ');
        if (spacePos2 != std::string::npos) {
            std::string channelName = args.substr(0, spacePos2);
            std::string targetNick = args.substr(spacePos2 + 1);
            if (!channelName.empty() && !targetNick.empty()) {
                handleKick(clientSockfd, targetNick, channelName);
            } else {
                std::cerr << "Error: Missing arguments for KICK command." << std::endl;
            }
        } else {
            std::cerr << "Error: Missing arguments for KICK command." << std::endl;
        }
    }
    else if (command == "INVITE") {
        size_t spacePos2 = args.find(' ');
        if (spacePos2 != std::string::npos) {
            std::string targetNick = args.substr(0, spacePos2);
            std::string channelName = args.substr(spacePos2 + 1);
            if (!targetNick.empty() && !channelName.empty()) {
                handleInvite(clientSockfd, targetNick, channelName);
            } else {
                std::cerr << "Error: Missing arguments for INVITE command." << std::endl;
            }
        } else {
            std::cerr << "Error: Missing arguments for INVITE command." << std::endl;
        }
    }
    else if (command == "TOPIC") {
        size_t spacePos2 = args.find(' ');
        std::string channelName = args.substr(0, spacePos2);
        std::string newTopic = args.substr(spacePos2 + 1); // Everything after the channel name is the topic
        if (!channelName.empty()) {
            handleTopic(clientSockfd, channelName, newTopic);
        } else {
            std::cerr << "Error: Missing arguments for TOPIC command." << std::endl;
        }
    }
    else if (command == "MODE") {
        size_t spacePos1 = args.find(' ');
        if (spacePos1 == std::string::npos) {
            std::cerr << "Error: Missing channel name for MODE command." << std::endl;
            return;
        }
    // Extract the channel name
        std::string channelName = args.substr(0, spacePos1);
        std::string remainingArgs = args.substr(spacePos1 + 1);
    // Now find the mode, which should be the next token
        size_t spacePos2 = remainingArgs.find(' ');
        std::string mode = remainingArgs.substr(0, spacePos2);
    // If there's another space, the param is after the mode
        std::string param;
        if (spacePos2 != std::string::npos) {
            param = remainingArgs.substr(spacePos2 + 1);
        }
    // Ensure mode and channelName are not empty
        if (!channelName.empty() && !mode.empty()) {
            handleMode(clientSockfd, channelName, mode, param);
        } else {
            std::cerr << "Error: Missing arguments for MODE command." << std::endl;
        }
    }
    else {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}
