#include "load_balancer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring> // for memset
#include "http_message.h"
#include <thread>      // for std::thread


void LoadBalancer::addServer(Server server) {
    this->servers.push_back(server);
}

LoadBalancer::LoadBalancer(int port): currentServerIndex(0),port(port) {
    lb_fd = socket(AF_INET,SOCK_STREAM,0); 
    lb_address.sin_family = AF_INET;
    lb_address.sin_addr.s_addr = INADDR_ANY;   
    lb_address.sin_port = htons(port);
}

void LoadBalancer::start() {
    bind(lb_fd,(struct sockaddr *)&lb_address,sizeof(lb_address));
    listen(lb_fd,5);
    std::cout << "Load Balancer listening on port " << this->port << "...\n";

    while(true) {
        int addrlen = sizeof(lb_address);
        int new_socket = accept(lb_fd, (struct sockaddr *)&lb_address, (socklen_t*)&addrlen);
        handleClientRequest(new_socket);
    }
}

void LoadBalancer::handleClientRequest(int client_socket) {
    std::cout << "Handling client request on Load Balancer...\n";
    int server_socket = connectToServer(servers[currentServerIndex]);
    if (server_socket == -1) {
        std::cerr << "Could not connect to server\n";
        return;
    }

    //Todo: error handling
    // char buffer[1024];
    // ssize_t bytes_read;

    //TODO: EXPLANATION OF PUTTING A THREAD HERE 

    std::thread clientToServerThread([client_socket, server_socket]() {
        char buffer[1024];
        ssize_t bytes_read;

        // Read request from client and forward to server
        while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
            write(server_socket, buffer, bytes_read);
        }
    });

    std::thread serverToClientThread([client_socket, server_socket]() {
        char buffer[1024];
        ssize_t bytes_read;

        // Read response from server and forward to client
        while ((bytes_read = read(server_socket, buffer, sizeof(buffer))) > 0) {
            write(client_socket, buffer, bytes_read);
        }
    });

    clientToServerThread.join();
    serverToClientThread.join();
    // Close the sockets
    close(client_socket);
    close(server_socket);

    currentServerIndex = (currentServerIndex + 1) % servers.size();
 
}

int LoadBalancer::connectToServer(const Server& server) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Could not create socket";
        return -1;
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server.getPort());

    // localhost for now 

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid IP address";
        return -1;
    }

    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed";
        return -1;
    }

    std::cout << "Connected to server " << server.getId() << "\n";
    return sock;

}