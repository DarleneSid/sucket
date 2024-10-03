#include <iostream>
#include <cstring>      // For memset, strerror
#include <cstdlib>      // For strtol
#include <sys/types.h>  // For socket, bind, listen, accept
#include <sys/socket.h> // For socket, bind, listen, accept
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close
#include <vector>
#include <poll.h>
#include <string>   // For std::string, find, subst
#include "Commands.hpp"
#include "Channel.hpp"


#define MAX_CLIENTS 1024

void handleClient(int clientSockfd, const std::string& password);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    // Use strtol for string to integer conversion (port number)
    char *endptr;
    long port = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number." << std::endl;
        return 1;
    }

    std::string password = argv[2];

    // Create a socket
    int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSockfd == -1) {
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1;
    }

    // Set up the server address struct
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));  // Use the port

    // Bind the socket
    if (bind(serverSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error: Could not bind socket." << std::endl;
        close(serverSockfd);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSockfd, 5) == -1) {
        std::cerr << "Error: Could not listen on socket." << std::endl;
        close(serverSockfd);
        return 1;
    }

    std::vector<pollfd> fds(MAX_CLIENTS);
    fds[0].fd = serverSockfd;
    fds[0].events = POLLIN;
    size_t nfds = 1;

    while (true) {
        int ret = poll(fds.data(), nfds, -1);
        if (ret < 0) {
            std::cerr << "Error: Poll failed." << std::endl;
            break;
        }

        for (size_t i = 0; i < nfds; ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == serverSockfd) {
                    // Accept new client connection
                    int clientSockfd = accept(serverSockfd, NULL, NULL);
                    if (clientSockfd >= 0) {
                        fds[nfds].fd = clientSockfd;
                        fds[nfds].events = POLLIN;
                        ++nfds;
                    }
                } else {
                    // Handle data from an existing client
                    char buffer[512];
                    ssize_t bytesReceived = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytesReceived <= 0) {
                        close(fds[i].fd);
                        fds[i] = fds[nfds - 1];
                        --nfds;
                    } else {
                        buffer[bytesReceived] = '\0';
                        std::string message(buffer);
                        std::cout << "Received: " << message << std::endl;
                        handleClient(fds[i].fd, password); // Process client data
                    }
                }
            }
        }
    }

    close(serverSockfd);
    return 0;
}

// void handleClient(int clientSockfd, const std::string& password) {
//     char buffer[512];
//     ssize_t bytesReceived = recv(clientSockfd, buffer, sizeof(buffer) - 1, 0);
//     if (bytesReceived <= 0) {
//         close(clientSockfd);
//         return;
//     }

//     buffer[bytesReceived] = '\0';
//     std::string message(buffer);

//     // Example: Basic password check
//     if (message.find("PASSWORD ") == 0) {
//         std::string clientPassword = message.substr(9);
//         if (clientPassword != password) {
//             std::cerr << "Incorrect password." << std::endl;
//             close(clientSockfd);
//             return;
//         } else {
//             std::cout << "Password correct. Client authenticated." << std::endl;
//         }
//     }
// }



void handleClient(int clientSockfd, const std::string& password) {
    // Authenticate the client with the password before processing further
    if (password.empty()) {
        std::cerr << "Error: Empty password provided." << std::endl;
        return;
    }
    
    // Assuming you send the password to the server for verification:
    std::string authCommand = "AUTH " + password + "\r\n";
    if (send(clientSockfd, authCommand.c_str(), authCommand.length(), 0) == -1) {
        std::cerr << "Error: Could not send authentication command." << std::endl;
        return;
    }

    // Then process the client's messages or actions
    char buffer[512];
    while (true) {
        int bytesReceived = recv(clientSockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error: Client disconnected or receive error." << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        processMessage(message, clientSockfd); // Assuming processMessage handles commands
    }

    // Close the socket when done
    close(clientSockfd);
}


// void handleClient(int clientSockfd) {
//     char buffer[1024];  // Buffer to receive data
//     std::string messageBuffer;  // String to accumulate messages

//     while (true) {
//         int bytesReceived = recv(clientSockfd, buffer, sizeof(buffer) - 1, 0);
//         if (bytesReceived <= 0) {
//             // Error or client disconnected
//             std::cerr << "Error: Failed to receive data or client disconnected." << std::endl;
//             break;
//         }

//         // Null-terminate the buffer and append to the message buffer
//         buffer[bytesReceived] = '\0';
//         messageBuffer += buffer;

//         // Process messages when newline(s) are found
//         size_t newlinePos;
//         while ((newlinePos = messageBuffer.find("\r\n")) != std::string::npos) {
//             std::string message = messageBuffer.substr(0, newlinePos);
//             processMessage(message, clientSockfd);

//             // Remove processed message from buffer
//             messageBuffer.erase(0, newlinePos + 2);  // Remove "\r\n"
//         }
//     }
//     close(clientSockfd);
// }


// void processMessage(const std::string& message, int clientSockfd) {
//     std::string command;
//     std::string args;

//     // Find the first space in the message to separate the command and arguments
//     size_t spacePos = message.find(' ');
//     if (spacePos != std::string::npos) {
//         command = message.substr(0, spacePos);    // The command is the first word
//         args = message.substr(spacePos + 1);      // The arguments are everything after the first space
//     } else {
//         command = message; // If no space, the entire message is the command
//     }

//     if (command == "KICK") {
//         size_t spacePos2 = args.find(' ');
//         if (spacePos2 != std::string::npos) {
//             std::string channelName = args.substr(0, spacePos2);
//             std::string targetNick = args.substr(spacePos2 + 1);
//             if (!channelName.empty() && !targetNick.empty()) {
//                 handleKick(clientSockfd, targetNick, channelName);
//             } else {
//                 std::cerr << "Error: Missing arguments for KICK command." << std::endl;
//             }
//         } else {
//             std::cerr << "Error: Missing arguments for KICK command." << std::endl;
//         }
//     }
//     else if (command == "INVITE") {
//         size_t spacePos2 = args.find(' ');
//         if (spacePos2 != std::string::npos) {
//             std::string targetNick = args.substr(0, spacePos2);
//             std::string channelName = args.substr(spacePos2 + 1);
//             if (!targetNick.empty() && !channelName.empty()) {
//                 handleInvite(clientSockfd, targetNick, channelName);
//             } else {
//                 std::cerr << "Error: Missing arguments for INVITE command." << std::endl;
//             }
//         } else {
//             std::cerr << "Error: Missing arguments for INVITE command." << std::endl;
//         }
//     }
//     else if (command == "TOPIC") {
//         size_t spacePos2 = args.find(' ');
//         std::string channelName = args.substr(0, spacePos2);
//         std::string newTopic = args.substr(spacePos2 + 1); // Everything after the channel name is the topic
//         if (!channelName.empty()) {
//             handleTopic(clientSockfd, channelName, newTopic);
//         } else {
//             std::cerr << "Error: Missing arguments for TOPIC command." << std::endl;
//         }
//     }
//     else if (command == "MODE") {
//         size_t spacePos2 = args.find(' ');
//         std::string channelName = args.substr(0, spacePos2);
//         args = args.substr(spacePos2 + 1);
//         spacePos2 = args.find(' ');
//         std::string mode = args.substr(0, spacePos2);
//         std::string param = args.substr(spacePos2 + 1);
//         if (!channelName.empty() && !mode.empty()) {
//             handleMode(clientSockfd, channelName, mode, param);
//         } else {
//             std::cerr << "Error: Missing arguments for MODE command." << std::endl;
//         }
//     } 
//     else {
//         std::cerr << "Unknown command: " << command << std::endl;
//     }
// }
