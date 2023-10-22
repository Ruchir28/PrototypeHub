#include "server.h"
#include "http_message.h"
#include <iostream>
#include <unistd.h> // for close()


std::string Server::getId() const {
    return this->id;
}

Server::Server(std::string id, int port) : id(id), port(port), server_fd(-1) {}

void Server::setup() {
    // this socket is used to listen for requests
    server_fd = socket(AF_INET,SOCK_STREAM,0); 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons(this->port);

    bind(server_fd,(struct sockaddr *)&address,sizeof(address));
    listen(server_fd,5);
    std::cout << "Server " << id << " listening on port " << port << "...\n";
}

void Server::listenForRequests() {
    std::cout << "Server " << id << " is ready to listen for requests on port " << port << "...\n";
    while (true) { // Loop to keep the server running
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        if (new_socket < 0) {
            std::cerr << "Accept failed on server " << id << "\n";
            continue; // Go to next iteration to keep listening
        }

        handleRequest(new_socket);
    }
}


std::string readHttpMessage(int client_socket) {
    std::string httpMessage;
    char buffer[1024] = {0};
    ssize_t bytesRead;
    while ((bytesRead = read(client_socket, buffer, sizeof(buffer))) > 0) {
        httpMessage.append(buffer, bytesRead);

        // Check if we've reached the end of the HTTP headers
        if (httpMessage.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }

    // If Content-Length header is present, read that many bytes for the body
    HttpMessage temp;
    temp.parse(httpMessage);
    std::string contentLengthStr = temp.getHeader("Content-Length");
    if (!contentLengthStr.empty()) {
        int contentLength = std::stoi(contentLengthStr);
        int bodyLength = httpMessage.length() - httpMessage.find("\r\n\r\n") - 4;
        while (bodyLength < contentLength) {
            bytesRead = read(client_socket, buffer, sizeof(buffer));
            httpMessage.append(buffer, bytesRead);
            bodyLength += bytesRead;
        }
    }

    return httpMessage;
}

int Server::getPort() const {
    return this->port;
}


void Server::handleRequest(int socket) {
    std::cout << "Handling request on server " << id << "...\n";
    std::string httpMessage = readHttpMessage(socket);
    HttpMessage request = HttpMessage();
    request.parse(httpMessage);
    HttpMessage response = HttpMessage();
    response.setStatusCode(200);
    response.setVersion("HTTP/1.1");
    response.setStatusMessage("OK");
    response.setHeader("Content-Type", "text/plain");
    response.setBody("Hello from server " + id);
    response.setHeader("Content-Length", std::to_string(response.getBody().length()));
    response.setHeader("Connection", "close");
    std::string response_string = response.serialize();
    std::cout << "Response:\n" << response_string << std::endl;
    ssize_t bytes_sent = 0;
    ssize_t total_bytes_sent = 0;
    size_t response_length = response_string.size();

    while (total_bytes_sent < response_length) {
        bytes_sent = send(socket, response_string.c_str() + total_bytes_sent, response_length - total_bytes_sent, 0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data. Exiting.\n";
            break;
        }
        total_bytes_sent += bytes_sent;
    }

    std::cout << "Sent " << total_bytes_sent << " bytes\n";

    sleep(3);
    // Close the socket
    close(socket);
}

