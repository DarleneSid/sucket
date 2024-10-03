// Channel.cpp
#include "Channel.hpp"
#include "Commands.hpp"
#include <iostream>
#include <algorithm> // For std::remove
#include <sstream>

extern std::map<std::string, Channel> channels; // Declare external map of channels

// Handle the KICK command
void handleKick(int clientSockfd, const std::string& targetNick, const std::string& channelName) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    int targetClientSockfd = findClientByNick(targetNick); // Implement this function
    if (targetClientSockfd == -1 || !isClientInChannel(targetClientSockfd, channelName)) {
        std::cout << "Client is not in the channel." << std::endl;
        return;
    }

    // Remove the client from the channel
    channels[channelName].clients.erase(
        std::remove(channels[channelName].clients.begin(), channels[channelName].clients.end(), targetClientSockfd),
        channels[channelName].clients.end()
    );

    std::cout << "Client " << targetNick << " has been kicked from " << channelName << std::endl;
}

// Handle the INVITE command
void handleInvite(int clientSockfd, const std::string& targetNick, const std::string& channelName) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    int targetClientSockfd = findClientByNick(targetNick); // Implement this function
    if (targetClientSockfd == -1) {
        std::cout << "Client not found." << std::endl;
        return;
    }

    // Add client to the channel
    channels[channelName].clients.push_back(targetClientSockfd);
    std::cout << "Client " << targetNick << " has been invited to " << channelName << std::endl;
}

// Handle the TOPIC command
void handleTopic(int clientSockfd, const std::string& channelName, const std::string& newTopic) {
    if (newTopic.empty()) {
        std::cout << "Current topic: " << channels[channelName].topic << std::endl;
    } else {
        if (!isChannelOperator(clientSockfd, channelName)) {
            std::cout << "You are not an operator and can't change the topic." << std::endl;
            return;
        }
        channels[channelName].topic = newTopic;
        std::cout << "Topic for " << channelName << " has been changed to: " << newTopic << std::endl;
    }
}

// Handle the MODE command
void handleMode(int clientSockfd, const std::string& channelName, const std::string& mode, const std::string& param) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    Channel& channel = channels[channelName];

    if (mode == "i") {
        channel.inviteOnly = !channel.inviteOnly;
        std::cout << "Invite-only mode set to " << (channel.inviteOnly ? "on" : "off") << " for " << channelName << std::endl;
    } else if (mode == "k") {
        channel.key = param;
        std::cout << "Channel key set to: " << param << std::endl;
    } else if (mode == "l") {
        std::stringstream ss(param);
        int userLimit;
        ss >> userLimit;
        channel.userLimit = userLimit;
        std::cout << "User limit set to " << userLimit << " for " << channelName << std::endl;
    }
}

// Dummy implementation for findClientByNick, isChannelOperator, and isClientInChannel
int findClientByNick(const std::string& nick) {
    (void)nick;  // This tells the compiler to ignore the unused parameter warning
    return -1; // Dummy return
}

bool isChannelOperator(int clientSockfd, const std::string& channelName) {
    (void)clientSockfd;
    (void)channelName;
    return true; // Dummy return
}

bool isClientInChannel(int clientSockfd, const std::string& channelName) {
    (void)clientSockfd;
    (void)channelName;
    return true; // Dummy return
}
