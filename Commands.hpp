// Commands.hpp
#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <sstream> // For stringstream
#include <set>
#include "Channel.hpp"

bool isOperator(int clientSockfd);
void processMessage(const std::string& message, int clientSockfd);
int findClientByNick(const std::string& nick);
void createChannel(int clientSockfd, const std::string& channelName);

#endif // COMMANDS_HPP

