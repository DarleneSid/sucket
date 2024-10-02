#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>
#include <map>

// Define a struct to represent a channel
struct Channel {
    std::string name;
    std::string topic;
    bool inviteOnly;
    int userLimit;
    std::string key;
    std::vector<int> clients;    // Store connected clients' socket IDs
    std::vector<int> operators;  // Store operator socket IDs
};

// Function declarations
void handleKick(int clientSockfd, const std::string& targetNick, const std::string& channelName);
void handleInvite(int clientSockfd, const std::string& targetNick, const std::string& channelName);
void handleTopic(int clientSockfd, const std::string& channelName, const std::string& newTopic = "");
void handleMode(int clientSockfd, const std::string& channelName, const std::string& mode, const std::string& param = "");

// Utility function declarations (optional)
int findClientByNick(const std::string& nick);
bool isChannelOperator(int clientSockfd, const std::string& channelName);
bool isClientInChannel(int clientSockfd, const std::string& channelName);

#endif // COMMANDS_HPP
