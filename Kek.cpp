#include <iostream>
#include <cstring>      // For memset, strerror
#include <cstdlib>      // For strtol
#include <sys/types.h>  // For socket, bind, listen, accept
#include <sys/socket.h> // For socket, bind, listen, accept
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close
#include <vector>
#include <poll.h>

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

void handleClient(int clientSockfd, const std::string& password) {
    char buffer[512];
    ssize_t bytesReceived = recv(clientSockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0) {
        close(clientSockfd);
        return;
    }

    buffer[bytesReceived] = '\0';
    std::string message(buffer);

    // Example: Basic password check
    if (message.find("PASSWORD ") == 0) {
        std::string clientPassword = message.substr(9);
        if (clientPassword != password) {
            std::cerr << "Incorrect password." << std::endl;
            close(clientSockfd);
            return;
        } else {
            std::cout << "Password correct. Client authenticated." << std::endl;
        }
    }

    // Process other commands
    if (message.find("NICK ") == 0) {
        std::string nickname = message.substr(5);
        std::cout << "Setting nickname: " << nickname << std::endl;
    } else if (message.find("USER ") == 0) {
        std::string userinfo = message.substr(5);
        std::cout << "Setting user info: " << userinfo << std::endl;
    } else if (message.find("JOIN ") == 0) {
        std::string channel = message.substr(5);
        std::cout << "Client joining channel: " << channel << std::endl;
    } else if (message.find("PRIVMSG ") == 0) {
        std::string msg = message.substr(9);
        std::cout << "Message received: " << msg << std::endl;
    } else {
        std::cerr << "Unknown command: " << message << std::endl;
    }
}
