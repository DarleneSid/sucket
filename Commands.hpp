// Commands.hpp
#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <map>
#include "Channel.hpp" // Include your Channel struct definition

// Declare channels as an external variable

void processMessage(const std::string& message, int clientSockfd);
int findClientByNick(const std::string& nick);

#endif // COMMANDS_HPP

