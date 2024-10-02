#include "Commands.hpp"
#include <iostream>
#include <algorithm>
#include <sstream> // For stringstream


// Global map to store channels and their details
std::map<std::string, Channel> channels;

// Function to handle the KICK command
void handleKick(int clientSockfd, const std::string& targetNick, const std::string& channelName) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    int targetClientSockfd = findClientByNick(targetNick);
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

// Function to handle the INVITE command
void handleInvite(int clientSockfd, const std::string& targetNick, const std::string& channelName) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    int targetClientSockfd = findClientByNick(targetNick);
    if (targetClientSockfd == -1) {
        std::cout << "Client not found." << std::endl;
        return;
    }

    // Add client to the channel
    channels[channelName].clients.push_back(targetClientSockfd);
    std::cout << "Client " << targetNick << " has been invited to " << channelName << std::endl;
}

// Function to handle the TOPIC command
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

// Function to handle the MODE command
void handleMode(int clientSockfd, const std::string& channelName, const std::string& mode, const std::string& param) {
    if (!isChannelOperator(clientSockfd, channelName)) {
        std::cout << "You are not an operator in this channel." << std::endl;
        return;
    }

    Channel& channel = channels[channelName];

    if (mode == "i") {
        channel.inviteOnly = !channel.inviteOnly;
        std::cout << "Invite-only mode set to " << (channel.inviteOnly ? "on" : "off") << " for " << channelName << std::endl;
    } else if (mode == "t") {
        std::cout << "Topic restrictions mode not yet implemented." << std::endl;
    } else if (mode == "k") {
        channel.key = param;
        std::cout << "Channel key set to: " << param << std::endl;
    } else if (mode == "o") {
        int targetClientSockfd = findClientByNick(param);
        if (targetClientSockfd != -1 && isClientInChannel(targetClientSockfd, channelName)) {
            channel.operators.push_back(targetClientSockfd);
            std::cout << "User " << param << " is now an operator in " << channelName << std::endl;
        } else {
            std::cout << "User not found in channel." << std::endl;
        }
    } else if (mode == "l") {
		std::stringstream ss(param);
		int userLimit;
		ss >> userLimit;
		channel.userLimit = userLimit;
        std::cout << "User limit set to " << param << " for " << channelName << std::endl;
    }
}


int findClientByNick(const std::string& nick) {
    (void)nick;  // This tells the compiler to ignore the unused parameter warning
	return -1; // Dummy return

    // Implementation goes here...
}

bool isChannelOperator(int clientSockfd, const std::string& channelName) {
    (void)clientSockfd;
    (void)channelName;
	return true; // Dummy return
    // Implementation goes here...
}

bool isClientInChannel(int clientSockfd, const std::string& channelName) {
    (void)clientSockfd;
    (void)channelName;
	return true; // Dummy return
    // Implementation goes here...
}


// // Dummy utility functions (implement these based on your system)
// int findClientByNick(const std::string& nick) {
//     // Return client socket ID based on the nickname
//     return -1; // Dummy return
// }

// bool isChannelOperator(int clientSockfd, const std::string& channelName) {
//     // Check if the client is an operator in the channel
//     return true; // Dummy return
// }

// bool isClientInChannel(int clientSockfd, const std::string& channelName) {
//     // Check if the client is in the channel
//     return true; // Dummy return
// }
